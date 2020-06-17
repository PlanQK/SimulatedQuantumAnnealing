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

/** \file
 * \brief back_inserter for array
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_TECHNICAL_HPP_GUARD
#define SIQUAN_CONNECT_TECHNICAL_HPP_GUARD

#include <array>
#include <vector>

namespace siquan {   // documented in base.hpp
namespace connect {  // documented in connect.hpp
    /// \cond IMPLEMENTATION_DETAIL_DOC
    namespace detail {

        // this gets the backinserter...
        template <typename C>
        struct back_inserter {
            back_inserter(C &arg) : arg_(arg) {}

            auto get() const { return std::back_inserter(arg_); }

            bool enough_space_for(size_t const &) const {
                return true;  // since we can always push_back
            }

        private:
            C &arg_;
        };

        // unless its an array, then its just the begin() iterator
        template <typename T, size_t N>
        struct back_inserter<std::array<T, N>> {
            using C = std::array<T, N>;
            back_inserter(C &arg) : arg_(arg) {}

            auto get() const { return arg_.begin(); }
            bool enough_space_for(size_t const &size) const {
                return size <= arg_.size();
            }

        private:
            C &arg_;
        };
    }  // end namespace detail
    /// \endcond
}  // namespace connect
}  // end namespace siquan

#endif  // SIQUAN_CONNECT_TECHNICAL_HPP_GUARD
