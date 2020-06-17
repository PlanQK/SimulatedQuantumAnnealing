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
 * \brief Observe the energy
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_OBSERVER_ENERGY_HPP_GUARD
#define SIQUAN_OBSERVER_ENERGY_HPP_GUARD

namespace siquan {    // documented in base.hpp
namespace observer {  // documented in observer.hpp
    /// \brief tracks the energy of the super module (can be a decoded trotter
    /// or simple state)
    /// \tparam super is the parent module which needs to
    /// provide the type \p weight_type
    template <typename super>
    struct energy : public super {
    public:
        using typename super::param;
        using typename super::weight_type;
        // all
        /// \brief Constructor
        /// \param p constructor argument for super
        energy(param const &p) : super(p) {}

        /// \brief updates the state of super, then measures the energy
        void update() {
            super::update();
            super::calc();
            energy_.push_back(super::energy());
        }
        /// \brief returns a vector of energies
        std::vector<weight_type> const &get_energy_evolution() const {
            return energy_;
        }
        /// \brief dumps the energies to \p "energy_evolution" in the map \p m
        /// \param m \p std::map<std::string, std::string> like instance.
        template <typename M>
        void to_map(M &m) const {
            super::to_map(m);
            m["energy_evolution"] = fsc::to_string(get_energy_evolution());
        }

    private:
        std::vector<weight_type> energy_;
    };
}  // namespace observer
}  // end namespace siquan

#endif  // SIQUAN_OBSERVER_ENERGY_HPP_GUARD
