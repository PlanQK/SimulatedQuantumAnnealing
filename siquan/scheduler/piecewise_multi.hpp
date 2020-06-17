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
 * \brief defines a macro for piecewise multifunctional scheduler
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_SCHEDULER_PIECEWISE_MULTI_HPP_GUARD
#define SIQUAN_SCHEDULER_PIECEWISE_MULTI_HPP_GUARD

#include <fsc/stdSupport.hpp>

#include <cmath>
#include <iostream>
#include <vector>

namespace siquan {     // documented in base.hpp
namespace scheduler {  // documented in scheduler.hpp
    /// \cond IMPLEMENTATION_DETAIL_DOC
    namespace detail {
        template <typename V, typename SZ = std::size_t>
        class piecewise_multi {
        public:
            //==using declarations===//
            using size_type = SZ;
            using value_type = V;
            using vec_type = std::vector<value_type>;
            enum class method_enum {
                linear,
                inverseF,
                inverseS,
                squareF,
                squareS
            };

            //====con-/destructor====//
            piecewise_multi(value_type const &start = 0,
                            value_type const &end = 1,
                            vec_type const &coeff = {},
                            std::vector<std::string> const &meth = {})
                : start_(start), end_(end), coeff_(coeff), meth_() {
                assert(meth.size() + 1 == coeff_.size());
                for(auto const &m : meth) {
                    if(m == "l") {
                        meth_.push_back(method_enum::linear);
                    } else if(m == "iF") {
                        meth_.push_back(method_enum::inverseF);
                    } else if(m == "iS") {
                        meth_.push_back(method_enum::inverseS);
                    } else if(m == "sS") {
                        meth_.push_back(method_enum::squareS);
                    } else if(m == "sF") {
                        meth_.push_back(method_enum::squareF);
                    } else {
                        throw std::runtime_error("Option " + m +
                                                 " is not supported");
                    }
                }
            }

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
                os << "piecewise_multi: start=" << start_ << ", stop=" << end_
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
                double progress =
                    std::max(0., (x - (idx * piece_len)) / piece_len);
                assert(x + 1e-10 >= (idx * piece_len) &&
                       x < (idx + 1) * piece_len);

                return dispatch_function_(coeff_[idx], coeff_[idx + 1],
                                          progress, meth_[idx]);
            }

        private:
            value_type dispatch_function_(value_type const &a,
                                          value_type const &b, double const &p,
                                          method_enum const &meth) {
                switch(meth) {
                    case(method_enum::linear):
                        return a + (b - a) * p;
                    case(method_enum::inverseS):
                        if(a < b)
                            return (a * b) / (b + (a - b) * p);
                        else
                            return a + b - (a * b) / (a - (a - b) * p);
                    case(method_enum::inverseF):
                        if(a > b)
                            return (a * b) / (b + (a - b) * p);
                        else
                            return a + b - (a * b) / (a - (a - b) * p);
                    case(method_enum::squareS):
                        return a + (b - a) * std::pow(p, 2);
                    case(method_enum::squareF):
                        return (b + (a - b) * std::pow(p - 1, 2));
                    default:
                        throw std::runtime_error("invalid method");
                }
                return value_type();
            }

        private:
            value_type start_;
            value_type end_;
            vec_type coeff_;
            std::vector<method_enum> meth_;
        };

        template <typename V, typename SZ>
        inline std::ostream &operator<<(std::ostream &os,
                                        piecewise_multi<V, SZ> const &arg) {
            arg.print(os);
            return os;
        }
    }  // end namespace detail
    /// \endcond
}  // end namespace scheduler
}  // end namespace siquan
/// \brief see \ref scheduler.hpp documentation for scheduler overview
#define SCHEDULER_GENERATE_PIECEWISE_MULTI(T, temp_type, temperature)          \
    template <typename super>                                                  \
    class piecewise_multi_##T : public super {                                 \
        using s_param = typename super::param;                                 \
                                                                               \
    public:                                                                    \
        /*==using declarations===*/                                            \
        using typename super::temp_type;                                       \
        using typename super::size_type;                                       \
                                                                               \
        struct param : public s_param {                                        \
            std::vector<temp_type> T##_val;                                    \
            std::vector<std::string> T##_meth;                                 \
            std::vector<std::string> T;                                        \
            template <typename M>                                              \
            void to_map(M &m) const {                                          \
                s_param::to_map(m);                                            \
                m[#T] = fsc::to_string(T);                                     \
            }                                                                  \
                                                                               \
            template <typename M>                                              \
            void from_map(M const &m) {                                        \
                s_param::from_map(m);                                          \
                                                                               \
                T = fsc::sto<std::vector<std::string>>(m.at(#T));              \
                for(auto const &k : T) {                                       \
                    if(T##_val.size() == T##_meth.size() + 1) {                \
                        if(k == "iF" or k == "iS" or k == "sS" or k == "sF" or \
                           k == "l") {                                         \
                            T##_meth.push_back(k);                             \
                            continue;                                          \
                        }                                                      \
                        T##_meth.push_back("l");                               \
                    }                                                          \
                    T##_val.push_back(fsc::sto<temp_type>(k));                 \
                }                                                              \
            }                                                                  \
        };                                                                     \
        /*====con-/destructor====*/                                            \
        piecewise_multi_##T(param const &p)                                    \
            : super(p),                                                        \
              T##_(),                                                          \
              T##_pl_(0, super::steps() - 1, p.T##_val, p.T##_meth) {          \
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
        scheduler::detail::piecewise_multi<temp_type, size_type> T##_pl_;      \
    };

#endif  // SIQUAN_SCHEDULER_PIECEWISE_MULTI_HPP_GUARD
