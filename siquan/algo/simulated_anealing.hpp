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
 * \brief a simple classical annealing code
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_ALGO_SIMULATED_ANEALING_HPP_GUARD
#define SIQUAN_ALGO_SIMULATED_ANEALING_HPP_GUARD

#include "../tools/random.hpp"

#include <iostream>
#include <vector>

namespace siquan {  // documented in base.hpp
namespace algo {    // documented in algo.hpp

    /// \brief Basic classical annealing implementation (metropolis montecarlo)
    /// \tparam super is the parent module which needs to provide the types
    /// \p size_type, \p state_type, \p weight_type and \p temp_type
    template <typename super>
    class simulated_anealing : public super {
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        using typename super::state_type;
        using typename super::weight_type;
        using typename super::temp_type;
        using typename super::size_type;

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super and seeds own RNGs
        simulated_anealing(s_param const &p)
            : super(p),
              T_(-1),
              sol_(),
              accept_cache_(),
              energy_(0),
              maxcut_(0),
              pos_(0, 1),
              prob_(0, 1) {
            pos_.seed(p.seed_rng());
            prob_.seed(p.seed_rng());
        }
        /*===modifying methods===*/
        /// \brief initializes super and then the module
        /// \details resizes the state to correct length, sets ranges of RNGs
        /// and precalculates the lookup table
        void init() {
            super::init();

            auto const &con = super::get_state_connect();

            sol_.resize(con.size());
            pos_.set_range(0, con.size() - 1);

            double max_abs_weight(0);
            for(uint i = 0; i < con.size(); ++i) {
                double abs_weight = 0;
                for(auto const &iact : con[i]) {
                    abs_weight += std::abs(iact->weight);
                    energy_ -= iact->weight / double(iact->vert_vec.size());
                }
                max_abs_weight = std::max(max_abs_weight, abs_weight);
            }
            // resize lookup (for weights int) accordingly
            accept_cache_.resize(max_abs_weight + 1);
        }
        /// \brief advances the state of the super and then the module's state
        /// \details attempts to do single spins updates N_vert() times
        void step() {
            super::step();
            auto const &con = super::get_state_connect();

            auto &lat = super::prot_get_state();
            for(uint i = 0; i < lat.size(); ++i) {
                // generate random position
                uint pos = pos_();
                // generate energy differece if swapped
                double E_diff = 0;
                for(auto const &iact : con[pos]) {
                    bool same = iact->vert_vec.size() % 2;
                    for(auto const &e : iact->vert_vec) {
                        same ^= lat[e];
                    }
                    if(not same)
                        E_diff += iact->weight;
                    else
                        E_diff -= iact->weight;
                }
                // swap-decision
                if(accept_(E_diff)) {
                    lat[pos] = !lat[pos];
                    maxcut_ -= E_diff;
                    energy_ += 2 * E_diff;
                }
            }
        }

        // scheduler
        /// \brief updates the super and the recalculates the lookup table
        void update() {
            super::update();

            if(super::temperature() != T_) {
                T_ = super::temperature();
                // recalc lookup-table
                for(uint i = 0; i < accept_cache_.size(); ++i) {
                    double const k = 1;
                    double const E_diff = i;
                    double const expo = E_diff / (k * T_);
                    accept_cache_[i] = std::exp(-expo);
                }
            }
        }
        /*==== const methods ====*/
        /// \brief get the current energy
        double const &energy() const { return energy_; }
        /// \brief get the current maxcut
        double const &maxcut() const { return maxcut_; }

        /// \brief print the super and then some information about the module
        void print() {
            super::print();

            std::cout << "Simulated Annealing: " << std::endl;
            std::cout << "energy:   " << energy() << std::endl;
            std::cout << "maxcut:   " << maxcut() << std::endl;
        }

    private:
        inline bool accept_(double const &E_diff) {
            if(E_diff <= 0)
                return true;
            else if(accept_cache_[E_diff] > prob_())
                return true;

            return false;
        }

    private:
        temp_type T_;
        state_type sol_;
        std::vector<double> accept_cache_;
        weight_type energy_;
        weight_type maxcut_;
        util::rng_class<size_type> pos_;
        util::rng_class<double> prob_;
    };

}  // end namespace algo
}  // end namespace siquan

#endif  // SIQUAN_ALGO_SIMULATED_ANEALING_HPP_GUARD
