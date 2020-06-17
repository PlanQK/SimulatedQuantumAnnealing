/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2014, ETH Zurich, ITP, Troels F. Roennow                          *
 * Copyright 2015-2018, ETH Zurich, ITP, Mario S. Koenz                        *
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
 * \brief includes all algo headers
 * \author
 * Year      |  Name
 * ----      | -----
 * 2014      | ETH Zurich, D-PHYS, Troels F. Roennow
 * 2015-2018 | ETH Zurich, ITP, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

/// \cond IMPLEMENTATION_DETAIL_DOC

#ifndef SQA_FASTEXP_HPP
#define SQA_FASTEXP_HPP
#include <math.h>
#include <stdlib.h>
#include <cassert>
#include "fastmath_detail.hpp"

#ifndef M_LN2
#define M_LN2 0.693147180559945309417
#define KILL_MLN2 true
#endif
namespace fastmath {

template <bool handle_overflow = false, size_t n = 8,
          typename data = __detail__::exp_correction<n>,
          typename type_t = double, typename int_t = int32_t,
          size_t significant_bits = 20, int c = 60801, size_t little_edian = 1>
class exp {
    int_t a, b;
    enum { mask = (1 << n) - 1 };
    int_t upper_limit, lower_limit, sign, first_sign;
    union {
        int_t uval[2];
        type_t dval;
    } bounds;

public:
    /**
       @brief creates an instance of an exponential class.
     **/
    exp() : b(1023 * (1 << significant_bits) - c) {
        set_coefficient();
        bounds.uval[little_edian] = (0x7ff << 20);
        bounds.uval[1 - little_edian] = 0;
    }

    /**
       @brief sets the coeffiecient c in the exponent, exp(c*x).

       @param coef is the coefficient.
     **/
    inline void set_coefficient(double const& coef = 1) {
        a = type_t(1 << significant_bits) / M_LN2 * coef;
        sign = 1;
        if(coef < 0) sign = -1;
        upper_limit = 1022 * M_LN2 / coef * sign;
        lower_limit = -1022 * M_LN2 / coef * sign;
        first_sign = sign < 0;
    }

    /**
       @brief computes exp(c*x) where c = 1 if not changed by
     set_coefficient.

       @param x is the argument of the exponential function.
     **/

    template <typename arg_t>
    inline type_t operator()(arg_t const& x) const {
        int_t tmp = a * x + b;
        union {
            int_t uval[2];
            type_t dval;
        } y;

        if(handle_overflow) {
            // TODO: Optimise and remove branching if possible
            if(x > upper_limit) {
                if(sign < 0) return 0;
                return bounds.dval;
            } else if(x < lower_limit) {
                //          y.uval[ little_edian ] = (0x7ff << 20) ;
                if(sign < 0) return bounds.dval;
                return 0;
            }
        }
        y.uval[1 - little_edian] = 0;
        y.uval[little_edian] = tmp;
        assert((mask & (tmp >> (significant_bits - n))) < (1 << n));
        if(n != 0)
            //~ y.dval *= data::data[mask & (tmp >> (significant_bits -
            // n))];
            y.dval *= data::data(mask & (tmp >> (significant_bits - n)));
        return y.dval;
    }
};

template <typename data, typename type_t, typename int_t,
          size_t significant_bits, int c, size_t little_edian>
class exp<true, 1024, data, type_t, int_t, significant_bits, c, little_edian> {
    double cf;

public:
    exp() { set_coefficient(); }
    inline void set_coefficient(double const& coef = 1) { cf = coef; }
    template <typename arg_t>
    inline type_t operator()(arg_t const& x) const {
        return ::exp(cf * x);
    }
};

//~ template< size_t n = 8, typename data = __detail__::exp_correction<n>,
// typename type_t = double, typename int_t = uint32_t,  size_t
// significant_bits = 20, int c = -1, size_t little_edian = 1>
//~ class log {
//~ type_t a, b, ia;
//~ enum { mask = (1 << n) - 1 };

//~ public:
//~ log() :  b( 1023 * (1 << significant_bits) - c ) { set_coefficient();  }
//~ inline void set_coefficient(double const& coef = 1) { a = type_t(1<<
// significant_bits )/M_LN2*coef; ia = 1./a; }

//~ template<typename arg_t >
//~ inline type_t operator()(arg_t const &x) const  {
//~ // Note that the error has symmetry around x=1 as
//~ // log(1/x) = 1 - log(x). It is sufficient to consider x < 1
//~ // when correcting for the error
//~ if(x == 1.0) return 0.0;

//~ union { int_t uval[2]; type_t dval; } y;
//~ y.dval = x ;
//~ int_t tmp = y.uval[ little_edian ];

//~ if(n != 0) {
//~ // fixme: here one needs to create the new mapped
//~ //        y.dval /=  data::data[ mask & (tmp >> (significant_bits - n )
//)  ];
//~ //        tmp = y.uval[ little_edian ];
//~ }
//~ // TODO: Add correction table
//~ return (type_t(tmp) - b) *ia;
//~ }
//~ };
}  // namespace fastmath
#ifdef KILL_MLN2
#undef M_LN2
#undef KILL_MLN2
#endif
#endif

/// \endcond
