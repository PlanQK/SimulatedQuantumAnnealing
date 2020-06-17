/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2014, ETH Zurich, ITP, Bettina Heim                               *
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
 * 2014      | ETH Zurich, D-PHYS, Bettina Heim
 * 2015-2018 | ETH Zurich, ITP, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

// based on L. Pierre, T. Giamarchi, H.J. Schulz (1987), Journal of Statistical
// Physics, vol. 48, no. 1, pages 135-149

/// \cond IMPLEMENTATION_DETAIL_DOC

#ifndef RANDOMBITS_HPP
#define RANDOMBITS_HPP

#include <bitset>
#include <cassert>
#include <iostream>
#include <limits>
#include <random>

template <class IntegerType, int Precision = 32>
class Randombits {
private:
    double prob;
    IntegerType probtable[Precision];
    mutable std::uniform_int_distribution<IntegerType> uniform_dist;

public:
    static const int precision = Precision;

    Randombits(double p = 0.5)
        : prob(p), uniform_dist(0, std::numeric_limits<IntegerType>::max()) {
        assert(p >= 0. && p <= 1.);
        assert(precision > 0 && precision < 64);  // to use a higher precision,
                                                  // replace all 1ull by a type
                                                  // of suitable size
        uint64_t probbits = prob * (1ull << precision);  // replace here
        probbits ^= (probbits >> 1);
        for(int i = 0; i < precision; ++i)
            probtable[i] = probbits & (1ull << i)
                               ? ~static_cast<IntegerType>(0)
                               : static_cast<IntegerType>(0);  // replace here
    }

    template <class Engine>
    inline IntegerType operator()(Engine&) const;

    template <class Engine>
    inline IntegerType operator()(Engine& e, double p) const {
        Randombits dist(p);
        return dist(e);
    }

    inline void param(double p) {
        *this = Randombits<IntegerType, Precision>(p);
    }

    inline bool operator==(const Randombits& rhs) {
        return this->param() == rhs.param();
    }

    inline bool operator!=(const Randombits& rhs) { return not(this == rhs); }
};

template <class IntegerType, int Precision>
template <class Engine>
IntegerType Randombits<IntegerType, Precision>::operator()(Engine& e) const {
    IntegerType a = ~static_cast<IntegerType>(0);
    IntegerType val = probtable[precision - 1];
    for(int i = precision - 2; a != 0 && i >= 0; --i) {
        a &= uniform_dist(e);
        val ^= (a & probtable[i]);
    }
    return val;
}

//~ template <class IntegerType, int Precision>
//~ std::ostream& operator<<(std::ostream& os, Randombits<IntegerType,Precision>
// const& rhs){
//~ return os << rhs.param();
//~ }

//~ template <class IntegerType, int Precision>
//~ std::istream& operator>>(std::istream& is,
// Randombits<IntegerType,Precision>& rhs){

//~ double p;
//~ is >> p;
//~ rhs.param(p);
//~ return is;
//~ }

#endif

/// \endcond
