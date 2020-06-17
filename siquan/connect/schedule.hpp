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
 * \brief changes the constraint interaction weights on every update according
 * to the constraint schedulers
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_SCHEDULE_HPP_GUARD
#define SIQUAN_CONNECT_SCHEDULE_HPP_GUARD

#include <algorithm>
#include <vector>

namespace siquan {   // documented in base.hpp
namespace connect {  // documented in connect.hpp

    /// \brief modifies the constraints according to the constraint
    /// schedulers, i.e.
    /// <tt>constr = base_constraint() + sum_constraint()*constr_weight</tt>
    /// \tparam super is the parent module which needs to provide the types
    /// \p connect_vec_type, \p constr_type and \p size_type
    template <typename super>
    class schedule : public super {
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        using typename super::connect_vec_type;
        using typename super::constr_type;
        using typename super::size_type;
        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super
        schedule(s_param const& p) : super(p) {}
        /*===modifying methods===*/

        /// \brief initializes super and then the module
        /// \details copies the constraints previously initialized. All modules
        /// below this won't see the original constraints anymore.
        void init() {
            super::init();

            auto const& pure_iact = super::get_connect();
            auto const& constr_iact = super::get_constraint();

            std::copy(pure_iact.begin(), pure_iact.end(),
                      std::back_inserter(connect_));
            first_constraint_ = connect_.size();
            std::copy(constr_iact.begin(), constr_iact.end(),
                      std::back_inserter(connect_));
        }

        /// \brief updates super and then the constraints
        void update() {
            super::update();
            if(base_constraint_ == super::base_constraint() and
               sum_constraint_ == super::sum_constraint())
                return;

            base_constraint_ = super::base_constraint();
            sum_constraint_ = super::sum_constraint();
            auto const& constraint = super::get_constraint();

            for(size_type i = first_constraint_, j = 0; i < connect_.size();
                ++i, ++j) {
                connect_[i].weight =
                    base_constraint_ + constraint[j].weight * sum_constraint_;
            }
        }
        /*==== const methods ====*/
        /// \brief get all connections (normal and constraints)
        connect_vec_type const& get_connect() const { return connect_; }
        /// \brief get athe position where the constraints start.
        size_type const& first_constraint() const { return first_constraint_; }
        /// \brief print a short help of the super and then itself
        void help() const { super::help(); }

    private:
        constr_type base_constraint_;
        constr_type sum_constraint_;
        size_type first_constraint_;
        connect_vec_type connect_;
    };

}  // end namespace connect
}  // end namespace siquan
#endif  // SIQUAN_CONNECT_SCHEDULE_HPP_GUARD
