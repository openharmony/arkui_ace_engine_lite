/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
import { backPage, routePage } from '../../../../common/js/general';
export default {
  ...backPage('pages/index/index'),
  changeDiv: routePage('pages/component/div/index/index').changePage,
  changeList: routePage('pages/component/list/index/index').changePage,
  changeListItem: routePage('pages/component/list/list-item/index/index').changePage,
  changeStack: routePage('pages/component/stack/index/index').changePage,
  changeSwiper: routePage('pages/component/swiper/index/index').changePage
};