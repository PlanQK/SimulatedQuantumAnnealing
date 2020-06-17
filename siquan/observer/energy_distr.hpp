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
 * \brief Observe the energy distribution between different n-body terms
 * (e.g. 2 vs 3 body)
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_OBSERVER_ENERGY_DISTR_HPP_GUARD
#define SIQUAN_OBSERVER_ENERGY_DISTR_HPP_GUARD

namespace siquan {    // documented in base.hpp
namespace observer {  // documented in observer.hpp
    /// \brief tracks the energy distribution of the super module
    /// \tparam super is the parent module which needs to provide the type
    /// \p weight_type and \p size_type
    template <typename super>
    struct energy_distr : public super {
    public:
        using typename super::param;
        using typename super::weight_type;
        using typename super::size_type;

        // all
        /// \brief Constructor
        /// \param p constructor argument for super
        energy_distr(param const &p) : super(p) {}

        /// \brief updates the state of super, then measures the energy
        /// distribution
        void update() {
            super::update();
            super::calc();
            energy_.push_back(super::get_energy_distr());
        }
        /// \brief returns a vector of energy distributions
        /// \details a single distribution has the number of bodies as key,
        /// i.e. for the energy of all 2-body terms,
        /// <tt>get_energy_distr_evolution()[2]</tt>. The 0-body term is the
        /// total energy.
        std::vector<std::map<size_type, weight_type>> const &
        get_energy_distr_evolution() const {
            return energy_;
        }
        /// \brief dumps the energy distributions to \p "energy_distr_evolution"
        /// in the map \p m
        /// \param m \p std::map<std::string, std::string> like instance.
        template <typename M>
        void to_map(M &m) const {
            super::to_map(m);
            std::stringstream ss;
            auto const &ener = get_energy_distr_evolution();
            uint period = ener.size();
            for(auto const &a : ener) {
                ss << "[";
                uint period2 = a.size();
                for(auto const &b : a) {
                    ss << b.first << "," << b.second;
                    if(--period2) ss << ",";
                }
                ss << "]";
                if(--period) ss << ",";
            }
            m["energy_distr_evolution"] = "[" + ss.str() + "]";
        }

    private:
        std::vector<std::map<size_type, weight_type>> energy_;
    };
}  // namespace observer
}  // end namespace siquan

#endif  // SIQUAN_OBSERVER_ENERGY_DISTR_HPP_GUARD
