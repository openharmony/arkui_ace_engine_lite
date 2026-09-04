/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_ACELITE_PAGE_TRANSITION_H
#define OHOS_ACELITE_PAGE_TRANSITION_H

// The whole page-transition engine is feature code: it is only compiled when the
// gate is ON. With the gate OFF this header is empty, so even whole-engine test
// builds carry no animation implementation.
#ifdef ENABLE_PAGE_TRANSITION_EFFECT
#include "animator.h"
#include "js_fwk_common.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {

/**
 * @brief Page transition type, maps to the 10 valid values of router.replace animation.type.
 *        UNKNOWN marks invalid / not-set type and degrades to no animation.
 */
enum class PageTransitionType : uint8_t {
    FADE,
    SCALE,
    SLIDE_LEFT,
    SLIDE_RIGHT,
    SLIDE_UP,
    SLIDE_DOWN,
    SLIDE_OVER_LEFT,
    SLIDE_OVER_RIGHT,
    SLIDE_OVER_UP,
    SLIDE_OVER_DOWN,
    UNKNOWN
};

/**
 * @brief Page transition configuration: type + duration(ms).
 *        Default is UNKNOWN + 0, i.e. "no animation", keeping the original behavior when
 *        animation parameter is absent.
 */
struct PageTransitionConfig {
    PageTransitionType type;
    uint16_t duration;        // transition duration(ms), 0~5000, 0 means no animation

    PageTransitionConfig() : type(PageTransitionType::UNKNOWN), duration(0) {}

    bool IsValid() const
    {
        return (type != PageTransitionType::UNKNOWN) && (duration > 0);
    }
};

/**
 * @brief Convert a transition type to its string name for diagnostic logging.
 *        Used by both the transition engine and the Router to print readable type in logs.
 */
const char *PageTransitionTypeToString(PageTransitionType type);

/**
 * @brief Page transition engine, driven frame-by-frame by AnimatorManager through Callback(),
 *        simultaneously updating the opacity/translate/scale of the old and new page root views.
 *
 * This class is intentionally decoupled from the Router: it reports completion through an
 * injected callback instead of holding a Router pointer, so the animation module stays reusable
 * and independently testable. The Router owns a running transition: it creates it, tracks it, and
 * is the only one that releases it.
 *
 * Lifecycle contract (verified against ui_lite source):
 * - Animator::Stop() synchronously invokes OnStop(), so OnStop() keeps its default empty
 *   implementation; normal completion is driven inside Callback().
 * - Normal completion: Callback() writes the final state, calls StopAnimator() and then invokes
 *   the completion callback. It never deletes itself - Callback() runs inside Animator::Run(),
 *   which runs inside AnimatorManager's list walk, so freeing this object there would leave the
 *   manager iterating over freed storage. The Router takes ownership in the callback and releases
 *   the object once the animator frame has unwound.
 * - Interruption: the Router calls StopAnimator() and then deletes the object, from a non-animator
 *   context (Hide / Router destruction).
 * - StopAnimator() frees nothing, so it is the only teardown step that is safe to call from
 *   inside Callback().
 *
 * Allocation: explicit operator new/delete route to the ACE memory heap (ace_malloc/ace_free).
 * Not inheriting MemoryHeap on purpose - AnimatorCallback already brings a HeapBase operator new
 * and a second one from MemoryHeap would be ambiguous.
 */
class PageTransition final : public AnimatorCallback {
public:
    using CompleteCallback = void (*)(void *);

    ACE_DISALLOW_COPY_AND_MOVE(PageTransition);
    PageTransition();
    ~PageTransition() override;

    // noexcept is required, not decorative: a non-throwing allocation function signals failure by
    // returning a null pointer, which is what makes the caller's null check well defined. Without
    // it the function is the throwing form and must not return null.
    void *operator new(size_t size) noexcept;
    void operator delete(void *p) noexcept;

    /**
     * @brief Start a page transition and self-register its completion through the injected callback.
     * @param config transition configuration (type + duration), must be IsValid()
     * @param oldView old page root view, already mounted on RootView
     * @param newView new page root view
     * @param completeCallback completion notification, invoked when the animation finishes;
     *        called from the animator context, must not touch this PageTransition afterwards -
     *        the Router uses it to take ownership and schedule the release
     * @param completeCtx context passed verbatim to completeCallback
     * @return true if the transition started, false on allocation failure (caller owns cleanup)
     */
    bool Start(const PageTransitionConfig &config, UIView *oldView, UIView *newView,
               CompleteCallback completeCallback, void *completeCtx);

    /**
     * @brief Snap the new page to its final state and unregister the animator. Frees nothing, so
     *        it is safe to call from inside Callback(); the caller keeps ownership of this object
     *        and must release it afterwards (see the lifecycle contract above).
     */
    void StopAnimator();

    /**
     * @brief Unregister the animator without touching the page views. Use this one whenever the
     *        views may already be gone (a transition released after its pages), StopAnimator()
     *        would write the final frame into them.
     */
    void StopAnimatorOnly();

#ifdef TDD_ASSERTIONS
    /**
     * @brief Test-only hooks to run a single interpolation frame without an Animator or a screen.
     *        TDD_ASSERTIONS is defined only by unit-test builds (test/ace_test_config.gni), so this
     *        block compiles out of product binaries. Never call it from production code.
     */
    void InitForTest(const PageTransitionConfig &config, UIView *oldView, UIView *newView);
    void ApplyFrameForTest(uint16_t elapsed);
    /**
     * @brief Test-only forwarder to the animator callback, so a case can drive one Callback()
     *        step without waiting for an AnimatorManager tick. Once the configured duration has
     *        elapsed the callback stops the animator and reports completion, exactly as in
     *        production; it does not release the object, so the caller still owns it.
     */
    void CallbackForTest(UIView *view);
    /**
     * @brief Test-only counter of the frames applied so far by every PageTransition of this run,
     *        so a case can assert that a compensating release does not write into the views again.
     *        Compiled out of product binaries, see the block comment above.
     */
    static uint32_t GetAppliedFrameCountForTest();
#endif

private:
    void Callback(UIView *view) override;
    // normalize elapsed time to easing progress, then dispatch to the per-type apply function
    void ApplyFrame(uint16_t elapsed);
    void ApplyFade(float progress);                  // fade both pages in/out
    void ApplyScale(float progress);                 // geometric scale only, both pages stay opaque
    void ApplySlide(float progress);                 // slide both pages simultaneously
    void ApplySlideOver(float progress);             // overlay slide: only the new page moves

    UIView *oldView_;
    UIView *newView_;
    PageTransitionConfig config_;
    Animator *animator_;
    CompleteCallback completeCallback_;
    void *completeCtx_;
    int16_t screenW_;
    int16_t screenH_;
};
} // namespace ACELite
} // namespace OHOS

#endif // ENABLE_PAGE_TRANSITION_EFFECT
#endif // OHOS_ACELITE_PAGE_TRANSITION_H
