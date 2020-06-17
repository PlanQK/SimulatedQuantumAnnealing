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
 * \brief Analyzes the energies of trotter slices and selects the best one
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_ALGO_BEST_TROTTER_HPP_GUARD
#define SIQUAN_ALGO_BEST_TROTTER_HPP_GUARD

#include "simulated_quantum_anealing.hpp"

#include <fsc/stdSupport.hpp>

#include <iostream>
#include <map>
#include <vector>

namespace siquan {  // documented in base.hpp

/// \brief Small helper for ^= operation
/// \param lhs as a vector-like type
/// \param rhs is the right hand side of the xor operation
template <typename T>
T &operator^=(T &lhs, std::vector<bool> const &rhs) {
    for(uint32_t i = 0; i < lhs.size(); ++i) {
        lhs[i] = lhs[i] ^ rhs[i];
    }
    return lhs;
}

namespace algo {  // documented in algo.hpp

    /// \brief Functionality to measure broken constraints
    /// \tparam super is the parent module which needs to provide the types
    /// \p size_type, \p state_type and \p weight_type
    template <typename super>
    class analyze_energy_trotter : public super {
        // mskoenz: this takes the convention of sqa regarding spin up
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        using typename super::size_type;
        using typename super::state_type;
        using typename super::weight_type;

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super
        analyze_energy_trotter(s_param const &p) : super(p) {}
        /*===modifying methods===*/
        /*==== const methods ====*/
        /// \brief get the energy distributions of all trotter slices
        std::vector<std::map<size_type, weight_type>> const &get_energy_distr()
            const {
            return ener_;
        }
        /// \brief get the total energy of all trotter slices
        std::vector<weight_type> const get_energy() const {
            std::vector<weight_type> res;
            for(auto const &a : get_energy_distr()) {
                res.push_back(a.at(0));
            }
            return res;
        }
        /// \brief return the absolute sum of all interactions
        weight_type const &weight_sum() const { return weight_sum_; }
        /// \brief get the index of the first trotter slice with minimal energy
        size_type const &min_index() const { return min_index_; }
        /// \brief get the amount of trotter slices with minimal energy
        size_type const &degen() const { return degen_; }

        /// \brief dumps \p trotter_degen to \p "trotter_degen"
        /// and \p trotter_min_index to \p "trotter_min_index" in the map \p m
        /// \param m \p std::map<std::string, std::string> like instance.
        template <typename M>
        void to_map(M &m) const {
            super::to_map(m);
            std::string add = "";
            if(m.count("trotter_degen")) {
                add = "_1";
            }
            m["trotter_degen" + add] = std::to_string(degen());
            m["trotter_min_index" + add] = std::to_string(min_index());
            // m["trotter_energy" + add] = fsc::to_string(get_energy());
        }

    protected:
        /// \brief calculate properties of super and then the own
        void calc() {
            super::calc();

            // calc all energies
            ener_.resize(super::get_nt());
            auto const &lat = super::get_state();
            auto const &con = super::get_connect();

            typename state_type::value_type allingment(super::get_nt());

            // set total energy (i.e. size = 0) to 0, since there may be no
            // connections
            for(size_t ts = 0; ts < super::get_nt(); ++ts) ener_[ts][0] = 0;

            weight_sum_ = 0;
            for(size_type i = 0; i < con.size(); ++i) {
                auto const &iact = con[i];
                auto const size = iact.vert_vec.size();
                weight_sum_ += iact.weight;

                allingment.reset();

                for(auto const &s : iact.vert_vec) allingment ^= lat[s];

                for(size_t ts = 0; ts < super::get_nt(); ++ts) {
                    auto epart = get_interaction_contribution(allingment[ts],
                                                              iact.weight);
                    ener_[ts][size] += epart;  // TODO: maybe add option
                    ener_[ts][0] += epart;
                }
            }

            // find degen and min_index
            min_index_ = 0;
            degen_ = 1;
            weight_type energy = ener_[0].at(0);
            for(size_type ts = 1; ts < super::get_nt(); ++ts) {
                if(ener_[ts].at(0) < energy) {
                    min_index_ = ts;
                    energy = ener_[ts].at(0);
                    degen_ = 1;
                } else if(ener_[ts].at(0) == energy)
                    ++degen_;
            }
        }

    private:
        std::vector<std::map<size_type, weight_type>> ener_;
        weight_type weight_sum_;
        size_type degen_;
        size_type min_index_;
    };

    /// \brief Selects the best trotter slice
    /// \tparam super is the parent module which needs to provide the types
    /// \p size_type and \p weight_type
    template <typename super>
    class best_trotter : public super {
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        using typename super::size_type;
        using typename super::weight_type;
        using state_type = std::vector<bool>;  ///< the type for the best state
        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super
        best_trotter(s_param const &p) : super(p) {}
        /*===modifying methods===*/
        /*==== const methods ====*/
        /// \brief get the state of the best trotter state
        state_type const &get_state() const { return state_; }

        /// \brief get the energy distribution of the best state
        std::map<size_type, weight_type> const &get_energy_distr() const {
            return super::get_energy_distr()[super::min_index()];
        }
        /// \brief dumps \p state to \p "state" and
        /// \p energy_distr to \p "energy_distr" and in the map \p m
        /// \param m \p std::map<std::string, std::string> like instance.
        template <typename M>
        void to_map(M &m) const {
            super::to_map(m);
            m["energy_distr"] = fsc::to_string(get_energy_distr());
            m["state"] = fsc::to_string(get_state());
        }

    protected:
        /// \brief calls super::calc and then selects the best state
        void calc() {
            super::calc();
            state_.resize(super::n_vert());

            auto const &lat = super::get_state();

            for(size_type i = 0; i < super::n_vert(); ++i) {
                state_[i] = (lat[i][super::min_index()] == super::spin_up());
            }
        }

    private:
        state_type state_;
    };

}  // end namespace algo
}  // end namespace siquan

#endif  // SIQUAN_ALGO_BEST_TROTTER_HPP_GUARD
