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
 * \brief Remaps the internal labels back to the initial user labels
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_UNMAP_HPP_GUARD
#define SIQUAN_CONNECT_UNMAP_HPP_GUARD

#include <algorithm>
#include <map>

namespace siquan {   // documented in base.hpp
namespace connect {  // documented in connect.hpp
    /// \brief Remaps the internal labels back to the initial user labels
    /// \tparam super is the parent module which needs to
    /// provide the types \p vert_type, \p user_type, \p size_type and
    /// \p weight_type
    template <typename super>
    class unmap : public super {
        using s_param = typename super::param;

    public:
        /// \brief param stage for this module
        struct param : public s_param {
            /// \brief dumps \p first_in to \p "first_in" in the map \p m
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void to_map(M &m) const {
                s_param::to_map(m);
                m["first_in"] = fsc::to_string(first_in);
            }
            /// \brief loads "first_in" from the map \p m into \p first_in
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void from_map(M const &m) {
                s_param::from_map(m);
                std::string in_str = fsc::get(m, "first_in", "0");
                first_in = fsc::sto<int>(in_str);
            }
            ///  since there is a high chance that we don't care about
            /// spin inversion (no field terms), we can set \p first_in
            /// to \p true, s.t. the first spin is always in the vector of
            /// outputted indexes.
            bool first_in;
        };
        /*==using declarations===*/
        using typename super::vert_type;
        using typename super::user_type;
        using typename super::size_type;
        using typename super::weight_type;

        /// \brief Constructor
        /// \param p constructor argument for super and sets first_in
        unmap(param const &p) : super(p), first_in_(p.first_in) {}

        /// \brief runs super::finish() and generates the output state
        void finish() {
            super::finish();

            // return state canonical, i.e. spin inversions get mapped to
            // the same. Use unmap option to change this behavior

            auto const &first_state =
                first_in_ ? super::get_state()[super::get_itou().begin()->first]
                          : 1;

            // mskoenz: create user state
            for(size_type i = 0; i < super::n_vert(); ++i) {
                if(super::get_state()[i] == first_state) {
                    user_state_.push_back(super::get_itou().at(i));
                }
            }
            std::sort(user_state_.begin(), user_state_.end());
        }

        /// \brief returns the output user state
        /// \pre finish() was called
        std::vector<user_type> const &get_state() const { return user_state_; }

        /// \brief print a short help of the super and then itself
        void help() const { super::help(); }

        /// \brief dumps the output user state to \p "state" in the map \p m
        /// \param m \p std::map<std::string, std::string> like instance.
        template <typename M>
        void to_map(M &m) const {
            super::to_map(m);
            m["state"] = fsc::to_string(get_state());
        }

    private:
        bool first_in_;
        weight_type energy_;
        std::vector<user_type> user_state_;
    };
}  // end namespace connect
}  // end namespace siquan
#endif  // SIQUAN_CONNECT_UNMAP_HPP_GUARD
