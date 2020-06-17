/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2016-2018, ETH Zurich, ITP, Mario S. Koenz                        *
 *                                                                             *
 * Licensed under the Apache License, Version 2.0 (the "License");             *
 * you may not use this file except in compliance with the License.            *
 * You may obtain a copy of the License at                                     *
 *                                                                             *
 *   http://www.apache.org/licenses/LICENSE-2.0                                *
 *                                                                             *
 * Unless required by applicable law or agreed to in writing, software         *
 * distributed under the License is distributed on an "AS IS" BASIS,           *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    *
 * See the License for the specific language governing permissions and         *
 * limitations under the License.                                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

/** \file
 * \brief Small helper function for the frescolino::rdtsc_timer
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_HELPER_HPP_GUARD
#define SIQUAN_HELPER_HPP_GUARD

#include <fsc/profiler.hpp>

namespace siquan {  // documented in base.hpp
/// \brief Dumps the information from a \p fsc::rdtsc_timer
/// \tparam M \p std::map<std::string, std::string> like type where the
/// information gets dumped to \tparam t the timer
template <typename M>
void to_map(M &m, fsc::rdtsc_timer const &t) {
    m["runtime_sec"] = std::to_string(t.sec());
    m["runtime_cycles"] = std::to_string(int64_t(t.cycles()));
}
}  // end namespace siquan

#endif  // SIQUAN_HELPER_HPP_GUARD
