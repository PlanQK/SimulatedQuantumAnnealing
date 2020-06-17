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
 * \brief a simple spin state (e.g. for classical annealing)
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_STATE_SIMPLE_HPP_GUARD
#define SIQUAN_STATE_SIMPLE_HPP_GUARD

#include <fsc/stdSupport.hpp>
#include <vector>

namespace siquan {  // documented in base.hpp
/// \brief hold the state types for the spins
namespace state {

    /// \brief A simple spin to bit representation with connections
    /// \tparam super is the parent module, which needs to provide the types \p
    /// state_type and \p connect_type
    template <typename super>
    class simple : public super {
    public:
        /*==using declarations===*/
        using typename super::state_type;    ///< loaded from super, usually \p
                                             ///< std::vector<bool>
        using typename super::connect_type;  ///< loaded from super
        using weak_connect_vec_type = std::vector<connect_type const *>;
        ///< a mapping that tracks all connections a spin is part of

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p parameter for super module
        simple(typename super::param const &p) : super(p) {}
        /*===modifying methods===*/
        /// \brief initializes super and then the module
        /// \details resizes the state to \p super::n_vert() and links
        /// \p super::get_connect() to the corresponding spins. Make sure
        /// this module is inserted after a connect module.
        void init() {
            super::init();
            state_.resize(super::n_vert());
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

        /// \brief get the connection vectors
        /// (i.e. get_state_connect()[spin_index] = {iact1, iact2, iact3, ...})
        std::vector<weak_connect_vec_type> const &get_state_connect() const {
            return state_connect_;
        }
        /// \brief prints super module, then itself
        void print() const {
            super::print();
            std::cout << "State: " << state_ << std::endl;
        }

    protected:
        /// \brief only modules in the inheritance chain can change the state
        state_type &prot_get_state() { return state_; }

    private:
        state_type state_;
        std::vector<weak_connect_vec_type> state_connect_;
    };
}  // end namespace state
}  // end namespace siquan

#endif  // SIQUAN_STATE_SIMPLE_HPP_GUARD
