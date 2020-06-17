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
 * \brief the complete trotter state
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_STATE_TROTTER_HPP_GUARD
#define SIQUAN_STATE_TROTTER_HPP_GUARD

#include <fsc/stdSupport.hpp>
#include <vector>

namespace siquan {  // documented in base.hpp
namespace state {   // documented in simple.hpp

    /// \brief A trotter-line spin representation with connections
    /// \tparam super is the parent module, which needs to provide the types \p
    /// size_type, \p state_type and \p connect_type
    template <typename super>
    class trotter : public super {
        using s_param = typename super::param;
        using trotter_line_type = typename super::state_type::value_type;

    public:
        /*==using declarations===*/
        using typename super::size_type;     ///< loaded from super
        using typename super::state_type;    ///< loaded from super, usually a
                                             ///< std::vector<trotter_line> type
        using typename super::connect_type;  ///< loaded from super
        using weak_connect_vec_type = std::vector<connect_type const *>;
        ///< a mapping that tracks all connections a spin is part of

        /// \brief param stage for this module
        struct param : public s_param {
            size_type nt;  ///< number of trotter slices

            /// \brief dumps \p nt to \p "nt" in the map \p m
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void to_map(M &m) const {
                s_param::to_map(m);
                m["nt"] = std::to_string(nt);
            }

            /// \brief loads "nt" from the map \p m into \p nt
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void from_map(M const &m) {
                s_param::from_map(m);
                nt = fsc::sto<size_type>(m.at("nt"));
            }
        };

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super and sets \p nt
        trotter(param const &p) : super(p), nt_(p.nt) {}
        /*===modifying methods===*/

        /// \brief initializes super and then the module
        /// \details resizes the state to be \p super::n_vert()*nt large and
        /// links connections to spins
        void init() {
            super::init();

            trotter_line_type def(nt_);
            def.flip();

            state_.resize(super::n_vert(), def);
            state_connect_.resize(super::n_vert());

            for(auto const &iact : super::get_connect()) {
                for(auto const &vert : iact.vert_vec) {
                    state_connect_[vert].push_back(&iact);
                }
            }
        }
        /*==== const methods ====*/
        /// \brief get the current configuration state
        state_type const &get_state() const { return state_; }
        /// \brief returns nt
        size_type const &get_nt() const { return nt_; }
        /// \brief get the connection vectors
        /// (i.e. get_state_connect()[spin_index] = {iact1, iact2, iact3, ...})
        std::vector<weak_connect_vec_type> const &get_state_connect() const {
            return state_connect_;
        }
        /// \brief prints super module, then itself
        void print() const {
            super::print();
            std::cout << "State: " << std::endl;
            for(auto const &a : state_) {
                for(size_type i = 0; i < a.size(); ++i) {
                    std::cout << (a[i] ? "-" : "+");
                }
                std::cout << std::endl;
            }
        }

    protected:
        /// \brief only modules in the inheritance chain can change the state
        state_type &prot_get_state() { return state_; }
        /// \brief only modules in the inheritance chain can change
        /// the connections
        std::vector<weak_connect_vec_type> &prot_get_state_connect() {
            return state_connect_;
        }

    private:
        size_type nt_;
        state_type state_;
        std::vector<weak_connect_vec_type> state_connect_;
    };
}  // end namespace state
}  // end namespace siquan

#endif  // SIQUAN_STATE_TROTTER_HPP_GUARD
