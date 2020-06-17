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
 * \brief Defines a simulation counter that stops after a certains step number
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_SCHEDULER_STEP_HPP_GUARD
#define SIQUAN_SCHEDULER_STEP_HPP_GUARD

#include <fsc/stdSupport.hpp>

namespace siquan {     // documented in base.hpp
namespace scheduler {  // documented in scheduler.hpp

    /// \brief a simple scheduler that increments a step counter and stops the
    /// simulation after a certains number of steps
    /// \tparam super is the parent module which needs to provide the type
    /// \p size_type
    template <typename super>
    class sim_step : public super {
        using s_param = typename super::param;

    public:
        //==using declarations===//
        using typename super::size_type;

        /// \brief param stage for this module
        struct param : public s_param {
            size_type steps;  ///< after this many steps, the simulation stops

            /// \brief dumps \p steps to \p "steps" in the map \p m
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void to_map(M &m) const {
                s_param::to_map(m);
                m["steps"] = std::to_string(steps);
            }

            /// \brief loads "steps" from the map \p m into \p steps
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void from_map(M const &m) {
                s_param::from_map(m);
                steps = fsc::sto<size_type>(m.at("steps"));
            }
        };
        //====con-/destructor====//
        /// \brief Constructor
        /// \param p constructor argument for super and sets \p steps
        sim_step(param const &p) : super(p), steps_(p.steps), counter_(0) {}
        //===modifying methods===//

        /// \brief advances the super and then itself
        /// \param o is an observable (or the entire simulation) for feedback
        template <typename O>
        void advance(O const &o) {
            super::advance(o);
            ++counter_;
        }
        //==== const methods ====//
        /// \brief stops the simulations after \p steps steps
        bool stop() const { return super::stop() or counter_ >= steps_; }

        /// \brief returns the current step count
        size_type const &counter() const noexcept { return counter_; }
        /// \brief returns the step count when the simulation stops
        size_type const &steps() const noexcept { return steps_; }

        /// \brief print a short help of the super and then itself
        void help() const {
            super::help();
            std::cout << "steps: this scheduler will increment a counter() \n"
                      << "    when advanced, until steps has been reached. \n"
                      << "    Then stop() will return true." << std::endl;
        }

    private:
        size_type steps_;
        size_type counter_;
    };
}  // end namespace scheduler
}  // end namespace siquan
#endif  // SIQUAN_SCHEDULER_STEP_HPP_GUARD
