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
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Modification Copiright 2020, d-fine GmbH, Daniel Herr                       *
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
 * 2020      | d-fine GmbH, Daniel Herr
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_READ_IN_TXT_HPP_GUARD
#define SIQUAN_CONNECT_READ_IN_TXT_HPP_GUARD

#include "read_in.hpp"
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
        /// \brief reads a connection file and sets connections/interactions/edges
        /// \tparam s_super is the parent module of another mixed in module
        /// \ref read_in. This modules is one of very few that mixin other modules.
        /// \p super needs to provide the types \p connect_type, \p weight_type,
        /// \p size_type, \p vert_type, \p user_type and \p vert_vec_type
        template <typename s_super>
        class read_in_txt : public read_in<s_super>
        {
            using super = read_in<s_super>;
            using s_param = typename super::param;

        public:
            /*==using declarations===*/
            using typename super::connect_type;
            using typename super::size_type;
            using typename super::user_type;
            using typename super::vert_type;
            using typename super::vert_vec_type;
            using typename super::weight_type;

            /*====con-/destructor====*/
            /// \brief Constructor
            /// \param p constructor argument for super
            read_in_txt(s_param const &p) : super(p) {}
            /*===modifying methods===*/
            /// \brief initializes super and then the module
            /// \details reads the file specified by \p super::get_filename()
            /// and sets connections/edges and vertices-labels
            void init()
            {
                super::init();

                std::ifstream ifs(super::get_filename());

                if (not ifs.is_open())
                    throw std::runtime_error("read_in_txt: file not found!");

                std::string line;

                for (uint32_t nr = 0; ifs; ++nr)
                {
                    std::getline(ifs, line);

                    if (nr == 0)
                    { // first line, get vertices
                        std::istringstream iss(line);
                        std::vector<std::string> v{
                            std::istream_iterator<std::string>{iss},
                            std::istream_iterator<std::string>()};

                        if (v.size() == 3)
                        { // "# 5 0"
                            super::prot_n_vert() = fsc::sto<size_type>(v[1]);
                            super::prot_n_edge() = fsc::sto<size_type>(v[2]);
                        }
                        else if (v.size() == 2 and v[0] != "#")
                        { // "#5 0"
                            super::prot_n_vert() =
                                fsc::sto<size_type>(v[0].substr(1));
                            super::prot_n_edge() = fsc::sto<size_type>(v[1]);
                        }
                        else
                        {
                            throw std::runtime_error(
                                "read_in_txt: headline '" + line +
                                "' not formatted correctly (# n_vert n_edge)");
                        }
                    }

                    if (line[0] == '#' or line == "")
                        continue; // skip comments and empty lines

                    // split the line
                    std::istringstream iss(line);
                    std::vector<std::string> v{
                        std::istream_iterator<std::string>{iss},
                        std::istream_iterator<std::string>()};

                    if (v.size() < 2)
                        throw std::runtime_error(
                            "read_in_txt: bad format on line " +
                            std::to_string(nr) +
                            ". Need at least one vertex and the interaction "
                            "weight");

                    connect_type iact;
                    iact.weight = fsc::sto<weight_type>(v.back());

                    v.pop_back();
                    //~ iact.vert_vec.resize(v.size());

                    // this is needed, since I want to support array and vector
                    auto inserter =
                        detail::back_inserter<vert_vec_type>(iact.vert_vec);

                    if (not inserter.enough_space_for(v.size()))
                    {
                        throw std::runtime_error(
                            "read_in_txt: not enough space in the container "
                            "(space: " +
                            std::to_string(iact.vert_vec.size()) + ") for " +
                            std::to_string(v.size()) + " vertices!");
                    }

                    auto &itou = super::prot_get_itou();

                    std::transform(v.begin(), v.end(), inserter.get(),
                                   [&itou](std::string const &text) {
                                       static vert_type internal_label = 0;
                                       auto vert = internal_label++;
                                       itou[vert] = fsc::sto<user_type>(text);
                                       return vert;
                                   });

                    super::prot_get_connect().push_back(std::move(iact));
                }

                if (super::prot_get_connect().size() != super::n_edge())
                {
                    std::clog
                        << "Warning: file " << super::get_filename()
                        << " containes an inconsistent amount of interactions,"
                        << " header says: " << super::n_edge()
                        << " acutally found: " << super::prot_get_connect().size()
                        << std::endl
                        << "Ignoring header value " << super::n_edge() << std::endl;

                    super::prot_n_edge() = super::prot_get_connect().size();
                }
            }
            /// \brief print a short help of the super and then itself
            void help() const
            {
                super::help();
                std::cout << "connect::read_in_txt:\n\
    Reads a txt file with the following line specifications: v1, ..., vn, weight\n\
    where v1, ..., vn are vertices and weight the energy cost if v1*...*vn == 1\n\
    For negative weights, v1*...*vn preferes to be -1"
                          << std::endl;
            }
        };
    } // end namespace connect
} // end namespace siquan
#endif // SIQUAN_CONNECT_READ_IN_TXT_HPP_GUARD
