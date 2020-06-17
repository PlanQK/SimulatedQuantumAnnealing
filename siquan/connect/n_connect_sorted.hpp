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
 * \brief sort the spins, s.t. the spin with most interactions has highest index
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_N_CONNECT_SORTED_HPP_GUARD
#define SIQUAN_CONNECT_N_CONNECT_SORTED_HPP_GUARD

#include <algorithm>
#include <vector>

#include <fsc/stdSupport.hpp>

namespace siquan {   // documented in base.hpp
namespace connect {  // documented in connect.hpp
    /// \brief Relabels/sorts the spins s.t. the spins with least interactions
    /// have lowest index.
    /// \details This is used in the \ref brute_force.hpp algorithm together
    /// with gray encoding to speed up the algorithm, since lowers spins flip
    /// more often than higher index spins.
    /// \tparam super is the parent module which needs to provide the types
    /// \p vert_type and \p size_type
    template <typename super>
    class n_connect_sorted : public super {
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        using typename super::vert_type;
        using typename super::size_type;
        /// \brief Constructor
        /// \param p constructor argument for super
        n_connect_sorted(s_param const &p) : super(p) {}
        /// \brief initializes super and then the module
        /// \details sort the spins, s.t. the spin with most interactions
        /// has highest index
        void init() {
            super::init();

            auto &connect = super::prot_get_connect();

            std::vector<std::pair<vert_type, size_type>> count;

            for(size_type i = 0; i < super::n_vert(); ++i) {
                count.push_back(std::make_pair(i, 0));
            }

            for(auto &iact : connect) {
                for(auto &vert : iact.vert_vec) ++count[vert].second;
            }

            std::sort(count.begin(), count.end(),
                      [](auto const &a, auto const &b) {
                          return a.second < b.second;
                      });

            auto ttou = super::get_itou();
            std::map<vert_type, vert_type> ttoi;

            for(size_type i = 0; i < count.size(); ++i) {
                auto const &a = count[i];
                ttoi[a.first] = i;
            }

            // fix interactions
            for(auto &iact : connect) {
                // apply mapping
                for(auto &vert : iact.vert_vec) vert = ttoi[vert];

                // sort spins for canonical form
                std::sort(iact.vert_vec.begin(), iact.vert_vec.end());
            }

            super::prot_get_itou().clear();

            for(auto const &p : ttoi)
                super::prot_get_itou()[p.second] = ttou.at(p.first);
        }
        /// \brief print a short help of the super and then itself (TODO)
        void help() const { super::help(); }

    private:
    };
}  // end namespace connect
}  // end namespace siquan
#endif  // SIQUAN_CONNECT_N_CONNECT_SORTED_HPP_GUARD
