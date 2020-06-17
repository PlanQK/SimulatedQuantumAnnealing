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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ** */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Modification Copyright 2020, d-fine GmbH, Daniel Herr                       *
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ** */

/** \file
 * \brief inclides all connect headers
 * \author
 * Year      |  Name
 * ----      | -----
 * 2020      | d-fine GmbH, Daniel Herr
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_CONNECT_HPP_GUARD
#define SIQUAN_CONNECT_CONNECT_HPP_GUARD

namespace siquan
{ // documented in base.hpp
    /// \brief Deals with input, preprocessing and encoding of connections
    namespace connect
    {
    }
} // namespace siquan

#include "basic.hpp"
#include "merge_equal.hpp"
#include "n_connect_sorted.hpp"
#include "normalize_weight.hpp"
#include "read_in_txt.hpp"
#include "readPythonStructure.hpp"
#include "remap.hpp"
#include "schedule.hpp"
#include "unmap.hpp"

#endif // SIQUAN_CONNECT_CONNECT_HPP_GUARD
