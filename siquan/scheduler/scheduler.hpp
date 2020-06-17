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
 * \brief instantiates the macros for various schedulers
 * \details Since there are many parameters that need to be changed during an
 * anneal, multiple scheduling strategies are available, and also
 * feedback-schedulers are possible (e.g. change temperature slower if a
 * measurement indicates high values). We used macros, since many schedulers
 * only differ in name and it's functions, not in the implementation. Here the
 * available schedulers, see \ref sim_step.hpp for the API. A scheduler is
 * combinded by the scheduler type X and the name Y, i.e. X_Y:
 *
 * X                | param members | functionality
 * ----             | -----          | -------------
 * \p linear           | \p Y_start, \p Y_end | Linear change from \p Y_start to \p Y_end over the time \p super::steps()
 * \p inverse          | \p Y_start, \p Y_end | Returns \p 1/s where \p s changes linearly between \p 1/Y_start to \p 1/Y_end over the time \p super::steps()
 * \p stepped          | \p Y_start, \p Y_end, \p Y_step    | Stepped linear change from \p Y_start to \p Y_end over the time \p super::steps() in increments of \p Y_step
 * \p piecewise_linear | \p Y    | Takes a vector \p Y of length L and moves linearly from <tt>T[n]</tt> to <tt>T[n+1]</tt> during the time intervall <tt>super::steps()*n/(L-1)</tt> and <tt>super::steps()*(n+1)/(L-1)</tt>.
 * \p piecewise_multi  | \p Y_val, \p Y_meth    | Works similar to piecewise_linear, but the methods can be specified as follows: linear (l), inverse fast (iF), inverse slow (iS), square fast (sF), square slow (sS). See below for convenient initialization.
 * A few examples of valid schedulers: \p linear_T, \p inverse_H, \p piecewise_multi_BC.
 * Y         | typedef needed | function name
 * ----      | -----          | -------------
 * \p T         | \p temp_type      | \p temperature()
 * \p H         | \p magn_type      | \p transverse_field()
 * \p BC        | \p constr_type    | \p base_constraint()
 * \p SC        | \p constr_type    | \p sum_constraint()
 *
 * For the initialization of \p piecewise_linear and \p piecewise_multi, one can
 * passan argument \p Y to the map m that is used in
 * <tt>param p; p.from_map(m);</tt> to set up the \p param instance.
 * * For \p piecewise_linear: <tt>m["Y"] = "[1,2,4.5,5]"</tt> works
 * * For \p piecewise_multi: <tt>m["Y"] = "[1,l,2,4.5,iS,5,sF,4]"</tt> works.
 * The string between numbers specify the method, linear if not specified.


 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_SCHEDULER_SCHEDULER_HPP_GUARD
#define SIQUAN_SCHEDULER_SCHEDULER_HPP_GUARD

#include "sim_step.hpp"

#include "inverse.hpp"
#include "linear.hpp"
#include "piecewise_linear.hpp"
#include "piecewise_multi.hpp"
#include "stepped.hpp"

/// \brief collect all scheduler macros
#define SCHEDULER_GENERATE_ALL(T, temp_type, temperature)          \
SCHEDULER_GENERATE_LINEAR(T, temp_type, temperature)               \
SCHEDULER_GENERATE_INVERSE(T, temp_type, temperature)              \
SCHEDULER_GENERATE_PIECEWISE_LINEAR(T, temp_type, temperature)     \
SCHEDULER_GENERATE_PIECEWISE_MULTI(T, temp_type, temperature)      \
SCHEDULER_GENERATE_STEPPED(T, temp_type, temperature)

namespace siquan {  // documented in base.hpp

/// \brief Holds multiple schedulers for the algorithms
namespace scheduler {
    SCHEDULER_GENERATE_ALL(T, temp_type, temperature)
    SCHEDULER_GENERATE_ALL(H, magn_type, transverse_field)
}  // end namespace scheduler
}  // end namespace siquan

#endif  // SIQUAN_SCHEDULER_SCHEDULER_HPP_GUARD
