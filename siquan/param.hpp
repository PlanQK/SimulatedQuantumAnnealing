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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

/** \file
 * \brief Holds siquan::seed_param, the base of all \p param structs
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_PARAM_HPP_GUARD
#define SIQUAN_PARAM_HPP_GUARD

#include <cstdint>
#include <random>

#include <fsc/stdSupport.hpp>

namespace siquan {  // documented in base.hpp

/// \brief This base param class holds the masterseed

/// \details Holds a random number generator and takes the masterseed with
/// set_seed. The method seed_rng should then be used to seed any other random
/// number generator/engine the project may have. This way, only one master seed
/// needs to be tracked. siquan::base uses this class.
class seed_param {
public:
    /*==using declarations===*/
    /*====con-/destructor====*/
    /// \brief Constructor, that seeds with 0
    seed_param() : seed_(0), seed_rng_(seed_) {}
    /*===modifying methods===*/

    /// \brief set a specific seed for the seed_rng
    /// \param s the desired seed
    void set_seed(uint32_t const &s) {
        seed_ = s;
        seed_rng_.seed(seed_);
    }

    /// \brief sets the seed \p "seed" from a map
    /// \param m \p std::map<std::string, std::string> like instance.
    template <typename M>
    void from_map(M const &m) {
        set_seed(fsc::sto<uint64_t>(fsc::get(m, "seed", "0")));
    }

    /*==== const methods ====*/
    /// \brief dumps the masterseed \p "seed" to a map
    /// \param m \p std::map<std::string, std::string> like instance.
    template <typename M>
    void to_map(M &m) const {
        m["seed"] = std::to_string(seed_);
    }
    /// \brief returns a random number that can be used to seed other
    /// generators/engines
    uint64_t seed_rng() const { return seed_rng_(); }

private:
    uint32_t seed_;
    mutable std::mt19937 seed_rng_;
};

}  // end namespace siquan

#endif  // SIQUAN_PARAM_HPP_GUARD
