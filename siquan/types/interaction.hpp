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
 * \brief basic connection type defined here
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_TYPES_INTERACTION_HPP_GUARD
#define SIQUAN_TYPES_INTERACTION_HPP_GUARD

namespace siquan {  // documented in base.hpp

/// \brief Contains the basic interaction types
namespace types {
    /// \brief Models one connection \f$J_{i,j,k,...}\f$
    /// \tparam weight_type is the type of interaction strength
    /// \tparam vert_vec_type is the type of the spin-index vector of the spins
    /// involved in the interaction
    template <typename weight_type, typename vert_vec_type>
    struct connect_type {
        /// \brief Constructor
        connect_type() : weight(), vert_vec() {}

        weight_type weight;      ///< strength of interaction \f$J\f$
        vert_vec_type vert_vec;  ///< index of spins involved in the interaction
                                 ///< \f$i,j,k,...\f$
    };

}  // end namespace types
}  // end namespace siquan

#endif  // SIQUAN_TYPES_INTERACTION_HPP_GUARD
