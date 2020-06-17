/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright 2020, d-fine GmbH, Daniel Herr                                    *
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
 * \brief reads a txt file with connections/interactions
 * \author
 * Year      |  Name
 * ----      | -----
 * 2020 | d-fine AG, Zürich, Daniel Herr
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_READ_PYTHON_STRUCTURE_HPP_GUARD
#define SIQUAN_CONNECT_READ_PYTHON_STRUCTURE_HPP_GUARD

#include "technical.hpp"

#include <fsc/stdSupport.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace siquan
{ // documented in base.hpp
    namespace connect
    { // documented in connect.hpp
        /// \brief reads in the python problem dictionary and sets connections/interactions/edges
        /// \ref read_in. This modules is one of very few that mixin other modules.
        /// \p super needs to provide the types \p connect_type, \p weight_type,
        /// \p size_type, \p vert_type, \p user_type and \p vert_vec_type
        template <typename super>
        class readPythonStructure : public super
        {
            using s_param = typename super::param;

        public:
            /*==using declarations===*/
            using typename super::connect_type;
            using typename super::size_type;
            using typename super::user_type;
            using typename super::vert_type;
            using typename super::vert_vec_type;
            using typename super::weight_type;

            /*==using declarations===*/
            /// \brief param stage for this module
            struct param : public s_param
            {
                std::vector<std::pair<weight_type, vert_vec_type>> problem;
                vert_type n_vert;
                weight_type n_edge;
                template <typename M>
                void to_map(M &m) const
                {
                    s_param::to_map(m);
                }
                template <typename M>
                void from_map(M const &m)
                {
                    s_param::from_map(m);
                }
            };

            /*====con-/destructor====*/
            /// \brief Constructor
            /// \param p constructor argument for super
            readPythonStructure(const param &p) : super(p) {}
            /*===modifying methods===*/
            /// \brief initializes super and then the module
            /// \details problem dict and sets connections/edges and vertices-labels
            void init()
            {
                super::init();
            }

            void setProblem(const std::vector<std::pair<weight_type, vert_vec_type>> &problem,
                            uint32_t n_vert)
            {
                super::prot_n_vert() = n_vert;
                super::prot_n_edge() = problem.size();

                // iterate over the vector pairs
                // each element describes a coupling
                for (auto pair : problem)
                {
                    // the first element in the pair is the coupling strength
                    // the second element is a vector of indices
                    connect_type iact;
                    iact.weight = pair.first;

                    auto &itou = super::prot_get_itou();
                    auto inserter =
                        detail::back_inserter<vert_vec_type>(iact.vert_vec);

                    std::transform(pair.second.begin(), pair.second.end(), inserter.get(),
                                   [&itou](vert_type const &v) {
                                       static vert_type internal_label = 0;
                                       auto vert = internal_label++;
                                       itou[vert] = v;
                                       return vert;
                                   });
                    super::prot_get_connect().push_back(std::move(iact));
                }

                if (super::prot_get_connect().size() != super::n_edge())
                {
                    std::clog
                        << "Warning: the problem specification containes an inconsistent amount of interactions,"
                        << " header says: " << super::n_edge()
                        << " acutally found: " << super::prot_get_connect().size()
                        << std::endl
                        << "Ignoring header value " << super::n_edge() << std::endl;

                    super::prot_n_edge() = super::prot_get_connect().size();
                }
            }
            std::string get_filename()
            {
                return "python";
            }
            /// \brief print a short help of the super and then itself
            void help() const
            {
                super::help();
                std::cout << "connect::readPythonStructure:\n\
    Interprets the structure obtained through the python interface following: [(weight, [1, ..., vn, weight])]\n\
    where v1, ..., vn are vertices and weight the energy cost if v1*...*vn == 1\n\
    For negative weights, v1*...*vn preferes to be -1"
                          << std::endl;
            }
        };
    } // end namespace connect
} // end namespace siquan
#endif // SIQUAN_CONNECT_READ_PYTHON_STRUCTURE_HPP_GUARD
