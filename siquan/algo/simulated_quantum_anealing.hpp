/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2016-2018, ETH Zurich, ITP, Mario S. Koenz                        *
 * Copyright 2014     , ETH Zurich, ITP, Bettina Heim                          *
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
 * \brief a bit coded simulated quantum annealing implementation
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * 2014      | ETH Zurich, D-PHYS, Bettina Heim
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_ALGO_SIMULATED_QUANTUM_ANEALING_HPP_GUARD
#define SIQUAN_ALGO_SIMULATED_QUANTUM_ANEALING_HPP_GUARD

#include "legacy/fastmath.hpp"
#include "legacy/randombits.hpp"

#include <fsc/stdSupport.hpp>

#include "../tools/random.hpp"

#include <iostream>
#include <vector>

namespace siquan {  // documented in base.hpp
namespace algo {    // documented in algo.hpp

    /// \brief fixes convention with signs
    /// \param prod_s_ij is equal to s_i * s_j * ...
    /// \param iact_stren is the strength of the interaction
    inline double get_interaction_contribution(bool const &prod_s_ij,
                                               double const &iact_stren) {
        // prod_s_ij = 1 <--> s_i * s_j * .... == -1
        // prod_s_ij = 0 <--> s_i * s_j * .... ==  1
        // + == -- (-1) because s_i * s_j ... == -1 and (-1) bc of convention
        return prod_s_ij ? +iact_stren : -iact_stren;
    }
    /// \brief fixes the convention for the signs of field distributions
    /// \param s_i the state of spin i
    /// \param field_stren is the corresponding field strength
    inline double get_field_contribution(bool const &s_i,
                                         double const &field_stren) {
        // s_i see conventions at the beginning of the file - s_i * H_i
        // + == -- (-1) because s_i == -1 and (-1) bc of convention
        return get_interaction_contribution(s_i, field_stren);
    }

    /// \brief This module provides a bit coded simulated quantum annealing
    /// implementation
    /// \tparam super is the parent module which needs to provide the types
    /// \p state_type and \p size_type
    template <typename super>
    class simulated_quantum_anealing : public super {
        using s_param = typename super::param;
        using trotter_line_type = typename super::state_type::value_type;

    public:
        /*==using declarations===*/
        using typename super::state_type;
        using typename super::size_type;

        /// \brief param stage for this module
        struct param : public s_param {
            bool periodic;
            ///< specifies if the world-lines should periodic (1) or not (0)

            /// \brief dumps \p periodic to \p "periodic" in the map \p m
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void to_map(M &m) const {
                s_param::to_map(m);
                m["periodic"] = std::to_string(periodic);
            }
            /// \brief loads "periodic" from the map \p m into \p periodic
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void from_map(M const &m) {
                s_param::from_map(m);
                periodic = fsc::sto<int>(fsc::get(m, "periodic", "1"));
            }
        };

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super and seeds own RNGs
        simulated_quantum_anealing(param const &p)
            : super(p),
              NT(super::get_nt()),
              bit_distribution(),
              drand(0, 1),
              formed_clusters(NT),
              updates(NT) {
            rnd32_generator.seed(p.seed_rng());
            get_rel_orientation_first_block =
                p.periodic
                    ? &simulated_quantum_anealing::rel_orientation_periodic
                    : &simulated_quantum_anealing::rel_orientation_nonperiodic;
        }
        /*===modifying methods===*/
        /// \brief initializes super and then the module
        /// \details resizes the state to correct length and sets ranges of RNGs
        void init() {
            super::init();

            ener_.resize(super::n_vert(),
                         typename decltype(ener_)::value_type(NT));

            auto &lat = super::prot_get_state();
            auto &con = super::prot_get_state_connect();

            std::mt19937_64 random(
                rnd32_generator());  // fixme: maybe use a distribution to
                                     // get a random int with a suitable
                                     // nr of bits...
            for(size_t idx = 0; idx < super::n_vert(); ++idx) {
                uint64_t l = 0;
                uint64_t r = 0;
                for(uint j = 0; j < NT; ++j) {
                    if(j % 64 == 0) {
                        r = random();
                        l = r;
                    }
                    lat[idx][j] = ((r & 1) == 0);
                    r >>= 1;
                }
                random.seed(l);
            }
            // calculate the h_field energy and remove those from the
            // interaction pool
            for(uint32_t idx = 0; idx < super::n_vert(); ++idx) {
                auto &site = lat[idx];

                double h_field = 0;

                // find h_fields (spins.size() == 1) and remove them
                auto it = std::find_if(con[idx].begin(), con[idx].end(),
                                       [](auto const &iptr) {
                                           return iptr->vert_vec.size() == 1;
                                       });
                if(it != con[idx].end()) {
                    h_field = (*it)->weight;
                    con[idx].erase(it);
                }
                for(size_t ts = 0; ts < NT; ++ts) {  // for all trotter slides
                    ener_[idx][ts] = get_field_contribution(site[ts], h_field);

                    for(size_t i = 0; i < con[idx].size();
                        ++i) {  // for all interactions
                        ener_[idx][ts] += get_coupling(con[idx][i], ts);
                    }
                }
            }
        }

        /// \brief advances the state of the super and then the module's state
        /// \details attempts to do cluster updates over the whole state
        void step() {
            super::step();
            //~ MIB_START("step_cluster")

            auto &lat = super::prot_get_state();

            for(size_type i = 0; i < lat.size(); ++i) {
                auto &site = lat[i];
                // form_bonds
                form_bonds(site);
                // breaks
                auto breaks = formed_clusters.begin();
                auto first_break = *breaks;
                auto last_break = first_break;
                ++breaks;  // here a dumb operator++ would be better maybe

                // while
                for(; breaks != formed_clusters.end(); ++breaks) {
                    // calculate h-field energy of the cluster if flipped
                    // (B in AAAABAAA)
                    double energy_difference =
                        std::accumulate(ener_[i].begin() + last_break,
                                        ener_[i].begin() + *breaks, 0.);
                    //~ std::cout << "Ediff1: " << energy_difference  << " f "
                    //<< last_break << "/" << *breaks << std::endl;
                    // energy sign here
                    if(drand(rnd32_generator) <= fexp(energy_difference)) {
                        add_spins_to_update(last_break, *breaks);
                    }
                    last_break = *breaks;  // index, at which cluster starts
                }
                // Ediff
                // calc the opposite energy of the trotter line (A in AAAABAAA)
                // and flip it
                double energy_difference =
                    std::accumulate(ener_[i].begin(),
                                    ener_[i].begin() + first_break, 0.) +
                    std::accumulate(ener_[i].begin() + last_break,
                                    ener_[i].end(), 0.);
                //~ std::cout << "Ediff2: " << energy_difference << " g " <<
                // last_break << "/" << first_break << std::endl;
                // energy sign here
                if(drand(rnd32_generator) <= fexp(energy_difference)) {
                    add_spins_to_update(last_break, *formed_clusters.end());
                    add_spins_to_update(0, first_break);
                }
                // update_site
                update_site(site, i);
                updates.reset();
            }
            //~ std::cout << lat << std::endl;
            //~ MIB_STOP("step_cluster")
        }

        /// \brief updates the super and gets the new state of the schedulers
        void update() {
            super::update();
            // calc_tau
            double tau = 1 / (super::temperature() * NT);  // tau = beta/nr_ts
            fexp.set_coefficient(2 * tau);  // factor 2 as we only half the
                                            // energy difference ener is
                                            // saved and updated
            // probability for cluster breakup
            bit_distribution.param(
                tanh(tau * std::abs(super::transverse_field())));
        }
        /*==== const methods ====*/
        /// \brief what we refer to a up spin
        int spin_up() const { return 0; }

        /// \brief print information about super, then of the own module
        void print() {
            super::print();

            std::cout << "Simulated Quantum Annealing: " << std::endl;
            //~ std::cout << "energy:   " << energy() << std::endl;
            //~ std::cout << "maxcut:   " << maxcut() << std::endl;
        }

    private:
        trotter_line_type rel_orientation_periodic(
            trotter_line_type const &spin_state) {
            auto temp = spin_state;
            temp <<= 1;
            temp[0] = spin_state[NT - 1];
            temp ^= spin_state;
            return temp;
        }

        trotter_line_type rel_orientation_nonperiodic(
            trotter_line_type const &spin_state) {
            auto temp = spin_state;
            temp <<= 1;
            temp ^= spin_state;
            temp[0] = 1;  // after xor
            return temp;
        }

        void add_spins_to_update(size_t const &start, size_t const &end) {
            for(size_t i = start; i < end; ++i) {
                assert(end <= NT);
                updates.flip(i);
            }
        }

        void form_bonds(trotter_line_type const &spin_state) {
            uint64_t r = 0;
            for(size_t ts = 0; ts < NT; ++ts) {
                if(ts % 64 == 0) {
                    r = bit_distribution(rnd32_generator);
                }

                formed_clusters[ts] = r & 1;
                r >>= 1;  // order?
            }
            auto temp = (this->*get_rel_orientation_first_block)(spin_state);

            // frm       1100101 // rng with break_up_prob
            //           i       i-1
            // dat       0110001 10001000
            // shift     1100011 ...
            // xor       1010010 //shows where the changes are
            // xor | frm 1110111
            formed_clusters |= temp;
        }

        void update_site(trotter_line_type &site, uint32_t const &idx) {
            // update_nbr
            // vec-alloc
            trotter_line_type alignment(NT);

            auto const &con = super::get_state_connect();
            auto const &lat = super::get_state();

            for(auto const &iptr : con[idx]) {  // for each interaction
                // allign=0
                alignment.reset();  // fixed it now since 1 is spin down

                // xor
                for(auto const &nb :
                    iptr->vert_vec) {  // neighbors = spins that couple
                    // get allignment for all trotter slides
                    alignment ^= lat[nb];
                    // todo: maybe save and update alignment (makes sense if a
                    // lot of spins couple)
                }

                // moreup
                for(auto const &nb : iptr->vert_vec) {
                    if(nb == idx) continue;  // and update the energies up-nbr
                    for(auto const &ts : updates)
                        ener_[nb][ts] -= get_interaction_contribution(
                            alignment[ts], 2 * iptr->weight);
                }
            }
            // update state
            site ^= updates;
            //~ std::transform(spin[site].state.begin(), spin[site].state.end(),
            // updates.begin(), spin[site].state.begin(),
            // std::bit_xor<base_type>());

            // h-field?
            // flip energy if updated?!
            for(uint32_t ts = 0; ts < NT; ++ts) {
                ener_[idx][ts] = updates[ts] ? -ener_[idx][ts] : ener_[idx][ts];
            }
        }
        template <typename iact_type>
        double get_coupling(iact_type const &iptr, size_type const &ts) {
            int negative = 0;
            auto const &lat = super::get_state();
            for(auto const &nb :
                iptr->vert_vec) {  // flip coupling #down spin times
                negative ^= lat[nb][ts];
            }
            return get_interaction_contribution(negative, iptr->weight);
        }

    private:
        std::vector<std::vector<double>> ener_;

        const uint32_t NT;
        Randombits<uint64_t> bit_distribution;
        std::uniform_real_distribution<double> drand;
        trotter_line_type formed_clusters;
        trotter_line_type updates;
        fastmath::exp<true, 12> fexp;  // 12: precision of error correction
        //~ slow_exp fexp;
        mutable std::mt19937 rnd32_generator;  // 32 bit random ints, mersenne
                                               // twister-> fixme: take
                                               // something more efficient

        trotter_line_type (
            simulated_quantum_anealing::*get_rel_orientation_first_block)(
            trotter_line_type const &);
    };

}  // end namespace algo
}  // end namespace siquan

#endif  // SIQUAN_ALGO_SIMULATED_QUANTUM_ANEALING_HPP_GUARD
