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
 * \brief defines a macro for inverse linear schedulers
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_SCHEDULER_INVERSE_HPP_GUARD
#define SIQUAN_SCHEDULER_INVERSE_HPP_GUARD

#include "linear.hpp"

/// \brief see \ref scheduler.hpp documentation for scheduler overview
#define SCHEDULER_GENERATE_INVERSE(T, temp_type, temperature)                  \
    template <typename super>                                                  \
    class inverse_##T : public super {                                         \
    public:                                                                    \
        /*==using declarations===*/                                            \
        using param = typename linear_##T<super>::param;                       \
        using typename super::temp_type;                                       \
        /*====con-/destructor====*/                                            \
        inverse_##T(param const &p)                                            \
            : super(p),                                                        \
              T##_(1 / p.T##_begin),                                           \
              T##_incr_((1 / p.T##_end - 1 / p.T##_begin) /                    \
                        (super::steps() - 1)) {}                               \
        /*===modifying methods===*/                                            \
        template <typename O>                                                  \
        void advance(O const &o) {                                             \
            super::advance(o);                                                 \
            T##_ += T##_incr_;                                                 \
        }                                                                      \
        /*==== const methods ====*/                                            \
        temp_type temperature() const noexcept { return 1. / T##_; }           \
                                                                               \
        void help() const {                                                    \
            super::help();                                                     \
            std::cout << #T << ":  a parameter with two entries [" << #T       \
                      << "_start, " << #T << "_stop]. \n"                      \
                      << "    This scheduler will change the " << #temperature \
                      << " \n"                                                 \
                      << "    inversely (1/x) between " << #T << "_start -> "  \
                      << #T << "_end" << std::endl;                            \
        }                                                                      \
                                                                               \
    private:                                                                   \
        temp_type T##_;                                                        \
        temp_type T##_incr_;                                                   \
    };

#endif  // SIQUAN_SCHEDULER_INVERSE_HPP_GUARD
