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
 * \brief Analyzes the energy of a state
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_ALGO_ANALYZE_ENERGY_HPP_GUARD
#define SIQUAN_ALGO_ANALYZE_ENERGY_HPP_GUARD

#include <fsc/stdSupport.hpp>

#include <map>

namespace siquan {  // documented in base.hpp
namespace algo {    // documented in algo.hpp
    /// \brief analyzes the energy of a state
    /// \tparam super is the parent module which needs to provide the types
    /// \p size_type, \p state_type and \p weight_type
    template <typename super>
    class analyze_energy : public super {
        // mskoenz: this assumes 1 == spin up!
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        using typename super::size_type;
        using typename super::state_type;
        using typename super::weight_type;
        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super
        analyze_energy(s_param const &p) : super(p) {}
        /*===modifying methods===*/
        /*==== const methods ====*/
        /// \brief
        /// \brief get the energy distribution
        std::map<size_type, weight_type> const &get_energy_distr() const {
            return ener_;
        }
        /// \brief get the energy of the state
        weight_type const &energy() const { return get_energy_distr().at(0); }
        /// \brief get the maxcut value of the system
        weight_type maxcut() const { return (weight_sum() + energy()) / (-2); }

        /// \brief return the absolute sum of all interactions
        weight_type const &weight_sum() const { return weight_sum_; }

        /// \brief dumps \p energy_distr to \p "energy_distr", \p maxcut to
        /// \p "maxcut" and \p energy to \p "energy" in the map \p m
        /// \param m \p std::map<std::string, std::string> like instance.
        template <typename M>
        void to_map(M &m) const {
            super::to_map(m);
            m["energy_distr"] = fsc::to_string(get_energy_distr());
            m["energy"] = std::to_string(energy());
            m["maxcut"] = std::to_string(maxcut());
        }

    protected:
        /// \brief calculate properties of super and then the own
        void calc() {
            super::calc();
            auto const &lat = super::get_state();
            auto const &con = super::get_connect();

            // set total energy (i.e. size = 0) to 0, since there may be no
            // connections
            ener_.clear();
            ener_[0] = 0;
            weight_sum_ = 0;

            for(size_type i = 0; i < con.size(); ++i) {
                int allingment = 0;
                auto const &iact = con[i];
                auto const size = iact.vert_vec.size();
                weight_sum_ += iact.weight;

                for(auto const &s : iact.vert_vec) allingment ^= (lat[s] == 0);

                auto epart = allingment ? iact.weight : -iact.weight;
                ener_[size] += epart;  // TODO: maybe add option
                ener_[0] += epart;
            }
        }

    private:
        std::map<size_type, weight_type> ener_;
        weight_type weight_sum_;
    };

}  // end namespace algo
}  // end namespace siquan

#endif  // SIQUAN_ALGO_ANALYZE_ENERGY_HPP_GUARD
