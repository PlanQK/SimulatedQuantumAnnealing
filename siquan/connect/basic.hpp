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
 * \brief holds the connections and necessary mappings
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_BASIC_HPP_GUARD
#define SIQUAN_CONNECT_BASIC_HPP_GUARD

#include <iostream>
#include <map>
#include <string>

namespace siquan {   // documented in base.hpp
namespace connect {  // documented in connect.hpp

    /// \brief contains the connections, number of variables & connections
    /// as well as a mapping from the user type to the index type
    /// \details The mapping is necessary, since the user can specify arbitrary
    /// objects as vertices (char, stirng, int, double) and the internal
    /// mapping has to be indices from 0 to \p n_vert.
    /// \tparam super is the parent module which needs to provide the types
    /// \p connect_vec_type, \p size_type, \p vert_type and \p user_type
    template <typename super>
    class basic : public super {
        using s_param = typename super::param;  // for ctor

    public:
        /*==using declarations===*/
        using typename super::connect_vec_type;
        using typename super::size_type;
        using typename super::vert_type;
        using typename super::user_type;
        using itou_type = std::map<vert_type, user_type>;
        ///< the mapping type from internal labels to user labels

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super
        basic(s_param const& p) : super(p), n_vert_(0), n_edge_(0) {}
        /*==== const methods ====*/

        /// \brief returns the connections
        connect_vec_type const& get_connect() const { return connect_; }
        /// \brief returns the (i)nternal (to) (u)ser mapping
        itou_type const& get_itou() const { return itou_; }
        /// \brief returns the amount of vertices/spins
        size_type const& n_vert() const { return n_vert_; }
        /// \brief returns the amount of connections/interactions/edges
        size_type const& n_edge() const { return n_edge_; }

        /// \brief prints the connections
        /// (TODO: make it part of base, i.e. super::print())
        void print() const {
            std::cout << "Vertices: " << n_vert() << " Edges: " << n_edge()
                      << std::endl;
            for(auto const& iact : get_connect()) {
                std::cout << "Interaction: ";
                for(auto const& v : iact.vert_vec) {
                    std::cout << get_itou().at(v) << "(" << v << ") ";
                }
                std::cout << "with weight " << iact.weight << std::endl;
            }
        }
        /// \brief print a short help of the super and then itself
        void help() const {
            super::help();
            std::cout << "connect::basic:\n\
    This class holds connections/interactions/edges. A connections contains \n\
    a list of vertices and a weight"
                      << std::endl;
        }

    protected:
        /*===modifying methods===*/
        /// \brief only modules in the inheritance chain can change the
        /// connections
        connect_vec_type& prot_get_connect() { return connect_; }
        /// \brief only modules in the inheritance chain can change the
        /// mapping
        itou_type& prot_get_itou() { return itou_; }
        /// \brief only modules in the inheritance chain can change the
        /// amount of vertices/spins
        size_type& prot_n_vert() { return n_vert_; }
        /// \brief only modules in the inheritance chain can change the
        /// amount of connections/interactions/edges
        size_type& prot_n_edge() { return n_edge_; }

    private:
        size_type n_vert_;
        size_type n_edge_;

        connect_vec_type connect_;

        itou_type itou_;
    };
}  // end namespace connect
}  // end namespace siquan
#endif  // SIQUAN_CONNECT_BASIC_HPP_GUARD
