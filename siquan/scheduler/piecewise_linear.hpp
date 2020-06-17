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
 * \brief defines a macro for piecewise linear scheduler
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_SCHEDULER_PIECEWISE_LINEAR_HPP_GUARD
#define SIQUAN_SCHEDULER_PIECEWISE_LINEAR_HPP_GUARD

#include <fsc/stdSupport.hpp>

#include <cmath>
#include <iostream>
#include <vector>

namespace siquan {     // documented in base.hpp
namespace scheduler {  // documented in scheduler.hpp

    /// \cond IMPLEMENTATION_DETAIL_DOC
    namespace detail {
        template <typename V, typename SZ = std::size_t>
        class piecewise_linear {
        public:
            //==using declarations===//
            using size_type = SZ;
            using value_type = V;
            using vec_type = std::vector<value_type>;
            //====con-/destructor====//
            piecewise_linear(value_type const &start = 0,
                             value_type const &end = 1,
                             vec_type const &coeff = vec_type())
                : coeff_(coeff), start_(start), end_(end) {}

            //===modifying methods===//
            value_type &operator[](size_type const &idx) {
                if(coeff_.size() <= idx) {
                    coeff_.resize(idx + 1);
                }
                return coeff_[idx];
            }
            //==== const methods ====//
            value_type const &operator[](size_type const &idx) const {
                return coeff_[idx];
            }
            template <typename S>
            void print(S &os) const {
                os << "piecewise_linear: start=" << start_ << ", stop=" << end_
                   << ", coeff=" << coeff_;
            }
            value_type operator()(value_type const &x) {
                if(x < start_ && x > end_) return std::nan("");

                size_type const N = coeff_.size() - 1;

                // find the linear slice
                size_type idx = (N) * (x - start_) / (end_ - start_);

                if(idx == N) return coeff_[idx];

                // find the progress between those two points
                value_type piece_len = (end_ - start_) / N;
                value_type progress =
                    std::max(0., (x - (idx * piece_len)) / piece_len);
                assert(x + 1e-10 >= (idx * piece_len) &&
                       x < (idx + 1) * piece_len);

                return coeff_[idx] + progress * (coeff_[idx + 1] - coeff_[idx]);
            }

        private:
            vec_type coeff_;
            value_type start_;
            value_type end_;
        };

        template <typename V, typename SZ>
        inline std::ostream &operator<<(std::ostream &os,
                                        piecewise_linear<V, SZ> const &arg) {
            arg.print(os);
            return os;
        }
    }  // end namespace detail
    /// \endcond
}  // end namespace scheduler
}  // end namespace siquan
/// \brief see \ref scheduler.hpp documentation for scheduler overview
#define SCHEDULER_GENERATE_PIECEWISE_LINEAR(T, temp_type, temperature)         \
    template <typename super>                                                  \
    class piecewise_linear_##T : public super {                                \
        using s_param = typename super::param;                                 \
                                                                               \
    public:                                                                    \
        /*==using declarations===*/                                            \
        using typename super::temp_type;                                       \
        using typename super::size_type;                                       \
                                                                               \
        struct param : public s_param {                                        \
            std::vector<temp_type> T;                                          \
            template <typename M>                                              \
            void to_map(M &m) const {                                          \
                s_param::to_map(m);                                            \
                m[#T] = fsc::to_string(T);                                     \
            }                                                                  \
                                                                               \
            template <typename M>                                              \
            void from_map(M const &m) {                                        \
                s_param::from_map(m);                                          \
                T = fsc::sto<std::vector<temp_type>>(m.at(#T));                \
            }                                                                  \
        };                                                                     \
        /*====con-/destructor====*/                                            \
        piecewise_linear_##T(param const &p)                                   \
            : super(p), T##_(), T##_pl_(0, super::steps() - 1, p.T) {          \
            T##_ = T##_pl_(0);                                                 \
        }                                                                      \
        /*===modifying methods===*/                                            \
        template <typename O>                                                  \
        void advance(O const &o) {                                             \
            super::advance(o);                                                 \
            T##_ = T##_pl_(super::counter());                                  \
        }                                                                      \
        /*==== const methods ====*/                                            \
        double const &temperature() const noexcept { return T##_; }            \
                                                                               \
        void help() const {                                                    \
            super::help();                                                     \
            std::cout << #T << ":  a parameter with many entries [" << #T      \
                      << "_1, ...,  " << #T << "_N]. \n"                       \
                      << "    This scheduler will change the " << #temperature \
                      << " \n"                                                 \
                      << "    piecewise-linearly between " << #T << "_n -> "   \
                      << #T << "_n+1" << std::endl;                            \
        }                                                                      \
                                                                               \
    private:                                                                   \
        double T##_;                                                           \
        scheduler::detail::piecewise_linear<temp_type, size_type> T##_pl_;     \
    };

#endif  // SIQUAN_SCHEDULER_PIECEWISE_LINEAR_HPP_GUARD
