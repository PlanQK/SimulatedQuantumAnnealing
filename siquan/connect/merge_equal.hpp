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
 * \brief merges the weights of identical connections into one connection.
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_MERGE_EQUAL_HPP_GUARD
#define SIQUAN_CONNECT_MERGE_EQUAL_HPP_GUARD

#include <algorithm>
#include <functional>
#include <set>

namespace siquan {   // documented in base.hpp
namespace connect {  // documented in connect.hpp
    /// \brief sums up identical constraints
    /// \tparam super is the parent module which needs to provide the types
    /// \p size_type, \p connect_type, \p connect_vec_type, \p weight_type and
    /// \p vert_vec_type
    template <typename super>
    class merge_equal : public super {
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        using typename super::size_type;
        using typename super::connect_type;
        using typename super::connect_vec_type;
        using typename super::weight_type;
        using typename super::vert_vec_type;

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super
        merge_equal(s_param const& p) : super(p) {}
        /*===modifying methods===*/
        /// \brief initializes super and then the module
        /// \details deletes connections that have identical spins participating
        /// and sums all weigths to the one remaining connection.
        void init() {
            super::init();

            connect_vec_type& connect = super::prot_get_connect();

            auto compare = [](std::reference_wrapper<vert_vec_type> const& a,
                              std::reference_wrapper<vert_vec_type> const& b) {
                return a.get() > b.get();
            };

            std::map<std::reference_wrapper<vert_vec_type>, weight_type*,
                     decltype(compare)>
                m(compare);

            // add a vert_vec to m and add the weight if an identical one shows
            // up
            for(auto& iact : connect) {
                auto it = m.find(iact.vert_vec);

                if(it != m.end()) {  // it is in the map
                    *(it->second) += iact.weight;
                    iact.weight = 0;
                    --super::prot_n_edge();
                } else {
                    m[std::ref(iact.vert_vec)] = &iact.weight;
                }
            }

            // remove all entries where the weight is 0
            auto const& new_end =
                std::remove_if(connect.begin(), connect.end(),
                               [](connect_type& a) { return a.weight == 0.; });
            connect.erase(new_end, connect.end());
        }
        /// \brief print a short help of the super and then itself
        void help() const {
            super::help();
            std::cout << "connect::merge_equal:\n\
    Merges the weights of identical connections. Best used after remap, since \n\
    remapping sorts the vertices in the connections in canonical order"
                      << std::endl;
        }
    };
}  // end namespace connect
}  // end namespace siquan
#endif  // SIQUAN_CONNECT_MERGE_EQUAL_HPP_GUARD
