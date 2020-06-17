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
 * \brief Bruteforce algorithm for exploring small systems
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_ALGO_BRUTE_FORCE_HPP_GUARD
#define SIQUAN_ALGO_BRUTE_FORCE_HPP_GUARD

#include <fsc/stdSupport.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace siquan {  // documented in base.hpp
namespace algo {    // documented in algo.hpp

    /// \brief This module tries all configurations and returns the optimum
    /// \tparam super is the parent module which needs to provide the types
    ///  \p state_type, \p weight_type, \p size_type and \p user_type
    template <typename super>
    class brute_force : public super {
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        using typename super::state_type;
        using typename super::weight_type;
        using typename super::size_type;
        using typename super::user_type;

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super
        brute_force(s_param const &p)
            : super(p),
              idx_(0),
              max_idx_(0),
              cut_(),
              degen_(),
              maxcut_(),
              to_flip_(),
              gray_() {}

        /*===modifying methods===*/
        /// \brief initializes super and then the module
        void init() {
            super::init();
            auto const &N = super::n_vert();
            gray_.resize(N);
            binary_.resize(N);

            max_idx_ = std::pow(2, N);  //-1 for set inversion of the problem
        }

        /// \brief advances the state of the super and then the module's state
        /// \details We traverse the configuration space in a gray code manner
        /// which minimizes the spins to flip.
        void step() {
            super::step();

            auto const &con = super::get_state_connect();

            gray_[to_flip_] = !gray_[to_flip_];
            for(auto const &iact : con[to_flip_]) {
                bool prod = iact->vert_vec.size() % 2;
                for(auto const &a : iact->vert_vec) {
                    prod ^= gray_[a];
                }
                if(prod)
                    cut_ -= iact->weight;
                else
                    cut_ += iact->weight;
            }

            if(cut_ > maxcut_) {
                maxcut_ = cut_;
                super::prot_get_state() = gray_;
                degen_ = 1;
            } else if(cut_ == maxcut_) {
                ++degen_;
            }
            //~ std::cout << gray << " " << idx_ << std::endl;
        }

        /// \brief advances the super and then itself
        /// \param o is an observable (or the entire simulation) for feedback
        template <typename O>
        void advance(O const &o) {
            super::advance(o);

            to_flip_ = get_first_zero_();
            incr_binary_();
            ++idx_;
        }
        /*==== const methods ====*/
        /// \brief stops the simulation as soon as all configurations where
        /// tested
        bool stop() const { return (idx_ == max_idx_) or super::stop(); }

        /// \brief get the current maxcut
        weight_type const &maxcut() const { return maxcut_; }
        /// \brief get the degeneracy of the current best maxcut
        size_type const &degen() const { return degen_; }

        /// \brief dumps \p degen to \p "degen" and \p maxcut to \p "maxcut"
        /// in the map \p m
        /// \param m \p std::map<std::string, std::string> like instance.
        template <typename M>
        void to_map(M &m) const {
            super::to_map(m);
            m["degen"] = std::to_string(degen());
            m["maxcut"] = std::to_string(maxcut());
        }

        /// \brief print the super and then the modules informations
        void print() {
            super::print();

            std::cout << "Brute Force: " << std::endl;
            std::cout << "maxcut:   " << maxcut() << std::endl;
            std::cout << "degen:    " << degen() << std::endl;
        }

    private:
        void incr_binary_() {
            for(size_type i = 0; i < binary_.size(); ++i) {
                if(binary_[i] == true)
                    binary_[i] = false;
                else {
                    binary_[i] = true;
                    break;
                }
            }
        }
        size_type get_first_zero_() {
            for(size_type i = 0; i < binary_.size(); ++i) {
                if(binary_[i] == 0) return i;
            }
            return -1;
        }

    private:
        // scheduler
        size_type idx_;
        size_type max_idx_;
        std::vector<bool> binary_;

        // algo
        weight_type cut_;
        size_type degen_;
        weight_type maxcut_;
        size_type to_flip_;
        state_type gray_;
    };
}  // end namespace algo
}  // end namespace siquan

#endif  // SIQUAN_ALGO_BRUTE_FORCE_HPP_GUARD
