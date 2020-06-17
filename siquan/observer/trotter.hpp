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
 * \brief Observer the trotter states
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_OBSERVER_TROTTER_HPP_GUARD
#define SIQUAN_OBSERVER_TROTTER_HPP_GUARD

namespace siquan {    // documented in base.hpp
namespace observer {  // documented in observer.hpp
    /// \brief tracks the trotter state and energies over the simulation
    /// \tparam super is the parent module which needs to provide the type
    /// \p weight_type
    template <typename super>
    struct trotter : public super {
    public:
        using typename super::param;
        using typename super::weight_type;
        // all

        /// \brief Constructor
        /// \param p constructor argument for super
        trotter(param const &p) : super(p) {}

        /// \brief updates the state of super, then measures the trotter state
        /// and energies
        void update() {
            super::update();
            super::calc();
            auto const &lat = super::get_state();
            auto const &ener = super::get_energy_distr();

            ts_states_.push_back(std::vector<std::vector<bool>>());
            ts_ener_.push_back(std::vector<weight_type>());
            for(uint32_t j = 0; j < lat[0].size(); ++j)
                ts_ener_.back().push_back(ener[j].at(0));

            for(uint32_t i = 0; i < lat.size(); ++i) {
                ts_states_.back().push_back(std::vector<bool>());
                for(uint32_t j = 0; j < lat[i].size(); ++j) {
                    ts_states_.back().back().push_back(lat[i][j]);
                }
            }
        }
        /// \brief returns a vector of trotter states
        std::vector<std::vector<std::vector<bool>>> const &
        get_trotter_state_evolution() const {
            return ts_states_;
        }
        /// \brief returns a vector of trotter state energies
        std::vector<std::vector<weight_type>> const &
        get_trotter_energy_evolution() const {
            return ts_ener_;
        }
        /// \brief dumps the trotter states to \p "trotter_state_evolution"
        /// and the energies to \p "trotter_energy_evolution" in the map \p m
        /// \param m \p std::map<std::string, std::string> like instance.
        template <typename M>
        void to_map(M &m) const {
            super::to_map(m);

            std::vector<std::string> part;
            for(auto const &a : get_trotter_state_evolution()) {
                std::vector<std::string> part2;
                for(auto const &x : a) {
                    part2.push_back(fsc::to_string(x));
                }
                part.push_back(fsc::to_string(part2));
            }
            m["trotter_state_evolution"] = fsc::to_string(part);

            std::vector<std::string> part2;
            for(auto const &x : get_trotter_energy_evolution()) {
                part2.push_back(fsc::to_string(x));
            }
            part.push_back(fsc::to_string(part2));
            m["trotter_energy_evolution"] = fsc::to_string(part2);
        }

    private:
        std::vector<std::vector<std::vector<bool>>> ts_states_;
        std::vector<std::vector<weight_type>> ts_ener_;
    };
}  // namespace observer
}  // end namespace siquan

#endif  // SIQUAN_OBSERVER_TROTTER_HPP_GUARD
