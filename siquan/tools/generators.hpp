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
 * \brief two generic permutation generators
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef GENERIC_GENERATORS_HPP_GUARD
#define GENERIC_GENERATORS_HPP_GUARD

#include <algorithm>
#include <vector>

/// \brief generic permutation generators of certain symmetries
namespace generic {

/// \brief returns all ordered X-sets out of a set of size N
/// \details example, N=3, X=2, returns {0,1}, {0,2}, {1,2}
class generate_cardiradic_subsets {
public:
    //==using declarations===//
    //====con-/destructor====//
    /// \brief Constructor
    /// \param N the size of the set
    /// \param X the size of the selection
    /// \pre X <= N
    generate_cardiradic_subsets(uint32_t const &N, uint32_t const &X)
        : N_(N), X_(X), select_(X) {
        reset();
    }
    //===modifying methods===//
    /// \brief resets the generator
    void reset() noexcept {
        for(uint32_t i = 0; i < X_; ++i) {
            select_[i] = i;
        }
    }
    /// \brief increments the generator
    /// \param pos is the position to increment
    /// \return false if the current state cannot be incremented further (e.g.
    /// {5,4,3} for N=6, X=3)
    bool increment(uint32_t const &pos = 0) {
        select_[pos] += 1;
        if(pos < N_ - 1) {
            if(select_[pos] == select_[pos + 1]) {
                select_[pos] = pos;
                return increment(pos + 1);
            }
        }

        if(select_[X_ - 1] >= N_)
            return false;
        else
            return true;
    }

    //==== const methods ====//
    /// \brief returns the current state
    std::vector<uint32_t> const &state() const { return select_; }

private:
    uint32_t const N_;
    uint32_t const X_;
    std::vector<uint32_t> select_;
};

/// \brief returns all permutations modulo trasposition and reflection
class mirror_cycle_perm {
public:
    //==using declarations===//
    //====con-/destructor====//
    /// \brief Constructor
    /// \param state is the original state we want to create permutations from
    mirror_cycle_perm(std::vector<uint32_t> const &state)
        : X_(state.size()),
          left_(1),
          right_(left_ + 1),
          state_(state),
          red_state_(X_) {
        reset(state);
    }

    //===modifying methods===//
    /// \brief resets the generator
    void reset(std::vector<uint32_t> const &state) {
        state_ = state;
        left_ = 1;
        right_ = left_ + 1;

        for(uint32_t i = 3; i < X_; ++i) red_state_[i - 1] = state_[i];

        red_state_[0] = state_[0];
        red_state_[1] = state_[1];
        red_state_[X_ - 1] = state_[2];

        it1 = red_state_.begin();
        it2 = red_state_.end();
        std::advance(it1, 2);
        std::advance(it2, -1);
    }
    /// \brief increments the generator
    /// \return false if all permutations have been explored
    bool increment() {
        if(std::next_permutation(it1, it2)) return true;

        if(right_ < X_ - 1) {
            ++right_;
            fill_red_state_();
            return true;
        }

        if(left_ < X_ - 2) {
            ++left_;
            right_ = left_ + 1;
            fill_red_state_();
            return true;
        }

        return false;
    }
    //==== const methods ====//
    /// \brief returns the current state
    std::vector<uint32_t> const &state() const { return red_state_; }

private:
    void fill_red_state_() {
        uint32_t j = 2;
        for(uint32_t i = 1; i < X_; ++i) {
            if(i != left_ and i != right_) {
                red_state_[j] = state_[i];
                ++j;
            }
        }
        red_state_[0] = state_[0];
        red_state_[1] = state_[left_];
        red_state_[X_ - 1] = state_[right_];
    }

private:
    uint32_t X_;
    uint32_t left_;
    uint32_t right_;
    std::vector<uint32_t> state_;
    std::vector<uint32_t>::iterator it1;
    std::vector<uint32_t>::iterator it2;
    std::vector<uint32_t> red_state_;
};

}  // end namespace generic

#endif  // GENERIC_GENERATORS_HPP_GUARD
