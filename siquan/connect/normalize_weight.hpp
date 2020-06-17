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
 * \brief rescales all weights s.t. the the highest theoretical energy to filp
 * the worst case spin is 1.
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_NORMALIZE_WEIGHT_HPP_GUARD
#define SIQUAN_CONNECT_NORMALIZE_WEIGHT_HPP_GUARD

#include <algorithm>

namespace siquan {   // documented in base.hpp
namespace connect {  // documented in connect.hpp
    /// \brief rescales the connection weights in every step
    /// \tparam super is the parent module which needs to provide the types
    /// \p size_type, \p connect_vec_type and \p weight_type
    template <typename super>
    class normalize_weight : public super {
        using s_param = typename super::param;

    public:
        using parent2 = super;
        ///< stores the current super to retrieve the original connections later

        /*==using declarations===*/
        using typename super::size_type;
        using typename super::connect_vec_type;
        using typename super::weight_type;

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super
        normalize_weight(s_param const& p) : super(p), connect_() {}
        /*===modifying methods===*/
        /// \brief initializes super and then the module
        /// \details copies the connections previously initialized. All modules
        /// below this won't see the original connections anymore.
        void init() {
            super::init();
            connect_ = super::get_connect();
        }
        /// \brief updates super and then the connections
        /// \details rescales all weights s.t. \f$ max_i(\sum
        /// |J_{\ldots,i,\ldots}|)=1\f$
        void update() {
            super::update();
            connect_vec_type const& connect = super::get_connect();

            // add up all iact weight for each vert
            std::map<size_type, weight_type> vert_to_abs_weight;

            for(auto const& iact : connect) {
                weight_type abs_weight = std::abs(iact.weight);
                for(auto const& vert : iact.vert_vec) {
                    vert_to_abs_weight[vert] += abs_weight;
                }
            }
            // find the maximum abs weight sum
            abs_weight_max_ = 0;
            for(auto const& pair : vert_to_abs_weight)
                abs_weight_max_ = std::max(abs_weight_max_, pair.second);

            // divide all iact-weights by max
            for(size_type i = 0; i < connect_.size(); ++i) {
                connect_[i].weight = connect[i].weight / abs_weight_max_;
            }
        }
        /*===const methods===*/
        /// \brief print a short help of the super and then itself
        void help() const {
            super::help();
            std::cout << "connect::normalize_weight:\n\
    Finds the highest edge abs weight: max(sum(abs(w_...i...), i) and divides \n\
    all weights by this number."
                      << std::endl;
        }
        /// \brief returns the connections
        connect_vec_type const& get_connect() const { return connect_; }

    private:
        weight_type abs_weight_max_;
        connect_vec_type connect_;
    };

    /// \brief retrieves the original constraints again
    /// \tparam super is the parent module which needs to provide the type
    /// \p connect_vec_type and \p parent2 (TODO: change to better name)
    template <typename super>
    class unnormalize_weight : public super {
        using s_param = typename super::param;

    public:
        using typename super::connect_vec_type;
        using typename super::parent2;
        /// \brief Constructor
        /// \param p constructor argument for super
        unnormalize_weight(s_param const& p) : super(p) {}

        /// \brief returns the original unmodified connections
        connect_vec_type const& get_connect() const {
            return parent2::get_connect();
        }
    };
}  // end namespace connect
}  // end namespace siquan
#endif  // SIQUAN_CONNECT_NORMALIZE_WEIGHT_HPP_GUARD
