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
 * \brief contains the filename of a file with connections stored
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_READ_IN_HPP_GUARD
#define SIQUAN_CONNECT_READ_IN_HPP_GUARD

#include <string>

namespace siquan {   // documented in base.hpp
namespace connect {  // documented in connect.hpp

    /// \brief tracks a filename, but does not know how to read_in
    /// simulation after a certains number of steps
    /// \tparam super is the parent module
    template <typename super>
    class read_in : public super {
        using s_param = typename super::param;

    public:
        /*==using declarations===*/
        /// \brief param stage for this module
        struct param : public s_param {
            std::string file;  ///< the file to read
            /// \brief dumps \p file to \p "file" in the map \p m
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void to_map(M &m) const {
                s_param::to_map(m);
                m["file"] = file;
            }
            /// \brief loads "file" from the map \p m into \p file
            /// \param m \p std::map<std::string, std::string> like instance.
            template <typename M>
            void from_map(M const &m) {
                s_param::from_map(m);
                file = m.at("file");
            }
        };

        /*====con-/destructor====*/
        /// \brief Constructor
        /// \param p constructor argument for super and sets \p the filename
        read_in(param const &p) : super(p), filename_(p.file) {}

        /*==== const methods ====*/
        /// \brief returns filename (for use in lower modules)
        std::string const &get_filename() const { return filename_; }
        /// \brief print a short help of the super and then itself
        void help() const {
            super::help();
            std::cout << "connect::read_in:\n\
    Just holds the name \"file\" where to read from. Does not read in anything."
                      << std::endl;
        }

    private:
        std::string filename_;
    };
}  // end namespace connect
}  // end namespace siquan
#endif  // SIQUAN_CONNECT_READ_IN_HPP_GUARD
