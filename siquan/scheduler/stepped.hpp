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
 * \brief Defines a macro for a linear scheduler that jumps in defined
 * incremets, not continuously
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_SCHEDULER_STEPPED_HPP_GUARD
#define SIQUAN_SCHEDULER_STEPPED_HPP_GUARD

#include <fsc/stdSupport.hpp>

#include <assert.h>
#include <cmath>

/// \brief see \ref scheduler.hpp documentation for scheduler overview
#define SCHEDULER_GENERATE_STEPPED(T, term_type, temperature)             \
    template <typename super>                                             \
    class stepped_##T : public super {                                    \
        using s_param = typename super::param;                            \
                                                                          \
    public:                                                               \
        using typename super::temp_type;                                  \
                                                                          \
        /*==using declarations===*/                                       \
        struct param : public s_param {                                   \
            temp_type T##_step;                                           \
                                                                          \
            template <typename M>                                         \
            void to_map(M &m) const {                                     \
                s_param::to_map(m);                                       \
                m[std::string(#T) + "_step"] = std::to_string(T##_step);  \
            }                                                             \
                                                                          \
            template <typename M>                                         \
            void from_map(M const &m) {                                   \
                s_param::from_map(m);                                     \
                T##_step =                                                \
                    fsc::sto<temp_type>(m.at(std::string(#T) + "_step")); \
            }                                                             \
        };                                                                \
        /*====con-/destructor====*/                                       \
        stepped_##T(param const &p)                                       \
            : super(p),                                                   \
              T##_step_(p.T##_step),                                      \
              T##_allign_(p.T##_end),                                     \
              T##_(round_##T(super::temperature())) {}                    \
        /*===modifying methods===*/                                       \
        template <typename O>                                             \
        void advance(O const &o) {                                        \
            super::advance(o);                                            \
            T##_ = round_##T(super::temperature());                       \
        }                                                                 \
        /*==== const methods ====*/                                       \
        temp_type const &temperature() const noexcept { return T##_; }    \
        void help() const {                                               \
            super::help();                                                \
            std::cout                                                     \
                << #T << "_step: this will change the behavior of the \n" \
                << "    scheduler. The " << #temperature                  \
                << " will now jump in\n"                                  \
                << "    steps of " << #T << "_step. Note that the value " \
                << #T << "_begin \n"                                      \
                << "    might not be taken, since it's more important \n" \
                << "    to hit " << #T << "_end exact." << std::endl;     \
        }                                                                 \
                                                                          \
    private:                                                              \
        temp_type round_##T(temp_type const &T##_in) const {              \
            return std::round((T##_in - T##_allign_) / T##_step_) *       \
                       T##_step_ +                                        \
                   T##_allign_;                                           \
        }                                                                 \
                                                                          \
        temp_type T##_step_;                                              \
        temp_type T##_allign_;                                            \
        temp_type T##_;                                                   \
    };

#endif  // SIQUAN_SCHEDULER_STEPPED_HPP_GUARD
