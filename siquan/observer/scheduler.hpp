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
 * \brief Observe the schedulers
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_OBSERVER_SCHEDULER_HPP_GUARD
#define SIQUAN_OBSERVER_SCHEDULER_HPP_GUARD

/// \brief Observes a scheduler
/// \details Since the \ref scheduler.hpp are instanciated with macros, another
/// macro for observing them is necessary. The access function is
/// \p get_Y_evolution() and the string in \p to_map is \p "Y_evolution"
#define GENERATE_SCHEDULER_OBSERVER(temp_type, T, temperature)      \
    template <typename super>                                       \
    struct T##_scheduler : public super {                           \
    public:                                                         \
        using typename super::param;                                \
        using typename super::temp_type;                            \
        T##_scheduler(param const &p) : super(p) {}                 \
                                                                    \
        void update() {                                             \
            super::update();                                        \
            sched_.push_back(super::temperature());                 \
        }                                                           \
        std::vector<temp_type> const &get_##T##_evolution() const { \
            return sched_;                                          \
        }                                                           \
        template <typename M>                                       \
        void to_map(M &m) const {                                   \
            super::to_map(m);                                       \
            m[std::string(#T) + "_evolution"] =                     \
                fsc::to_string(get_##T##_evolution());              \
        }                                                           \
                                                                    \
    private:                                                        \
        std::vector<temp_type> sched_;                              \
    };

namespace siquan {    // documented in base.hpp
namespace observer {  // documented in observer.hpp
    GENERATE_SCHEDULER_OBSERVER(temp_type, T, temperature)
    GENERATE_SCHEDULER_OBSERVER(magn_type, H, transverse_field)
}  // namespace observer
}  // end namespace siquan

#undef GENERATE_SCHEDULER_OBSERVER

#endif  // SIQUAN_OBSERVER_SCHEDULER_HPP_GUARD
