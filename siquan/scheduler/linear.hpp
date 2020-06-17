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
 * \brief defines a macro for linear schedulers
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_SCHEDULER_LINEAR_HPP_GUARD
#define SIQUAN_SCHEDULER_LINEAR_HPP_GUARD

#include <fsc/stdSupport.hpp>

#include <assert.h>
/// \brief see \ref scheduler.hpp documentation for scheduler overview
#define SCHEDULER_GENERATE_LINEAR(T, temp_type, temperature)                   \
    template <typename super>                                                  \
    class linear_##T : public super {                                          \
        using s_param = typename super::param;                                 \
                                                                               \
    public:                                                                    \
        /*==using declarations===*/                                            \
        using typename super::temp_type;                                       \
                                                                               \
        struct param : public s_param {                                        \
            temp_type T##_begin;                                               \
            temp_type T##_end;                                                 \
                                                                               \
            template <typename M>                                              \
            void to_map(M &m) const {                                          \
                s_param::to_map(m);                                            \
                m[#T] = fsc::to_string(                                        \
                    std::vector<temp_type>{T##_begin, T##_end});               \
            }                                                                  \
                                                                               \
            template <typename M>                                              \
            void from_map(M const &m) {                                        \
                s_param::from_map(m);                                          \
                auto unpack = fsc::sto<std::vector<temp_type>>(m.at(#T));      \
                /* The T input, must have exactly two entries */               \
                assert(unpack.size() == 2);                                    \
                T##_begin = unpack.front();                                    \
                T##_end = unpack.back();                                       \
            }                                                                  \
        };                                                                     \
        /*====con-/destructor====*/                                            \
        linear_##T(param const &p)                                             \
            : super(p),                                                        \
              T##_(p.T##_begin),                                               \
              T##_incr_((p.T##_end - p.T##_begin) / (super::steps() - 1)) {}   \
        /*===modifying methods===*/                                            \
        template <typename O>                                                  \
        void advance(O const &o) {                                             \
            super::advance(o);                                                 \
            T##_ += T##_incr_;                                                 \
        }                                                                      \
        /*==== const methods ====*/                                            \
        temp_type const &temperature() const noexcept { return T##_; }         \
                                                                               \
        void help() const {                                                    \
            super::help();                                                     \
            std::cout << #T << ":  a parameter with two entries [" << #T       \
                      << "_start, " << #T << "_stop]. \n"                      \
                      << "    This scheduler will change the " << #temperature \
                      << " \n"                                                 \
                      << "    linearly between " << #T << "_start -> " << #T   \
                      << "_end" << std::endl;                                  \
        }                                                                      \
                                                                               \
    private:                                                                   \
        temp_type T##_;                                                        \
        temp_type T##_incr_;                                                   \
    };

#endif  // SIQUAN_SCHEDULER_LINEAR_HPP_GUARD
