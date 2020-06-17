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
 * \brief remaps user labels to internal labels * \author
 * Year      |  Name
 * 2020      | d-fine GmbH, Daniel Herr
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_CONNECT_REMAP_HPP_GUARD
#define SIQUAN_CONNECT_REMAP_HPP_GUARD

#include <algorithm>
#include <map>

#include <fsc/stdSupport.hpp>

namespace siquan
{ // documented in base.hpp
    namespace connect
    { // documented in connect.hpp

        /// \brief remaps user types to internal types for faster data types
        /// \tparam super is the parent module which needs to provide the types
        /// \p vert_type and \p user_type.
        template <typename super>
        class remap : public super
        {
            using s_param = typename super::param;

        public:
            /*==using declarations===*/
            using typename super::user_type;
            using typename super::vert_type;

            /// \brief param stage for this module
            /// \details remap (to_map(), from_map()) can be specified as a string.
            /// Here a few examples:
            /// * \p "sort,no_fill"
            /// * \p "encounter,fill,0"
            /// * \p "encounter,no_fill"
            struct param : public s_param
            {
                /// \brief specifies the sorting options
                enum class sort_modus
                {
                    sorted,
                    encounter
                };

                /// \brief specifies the filling options
                /// \details can be useful if there are some spins that have no
                /// interactions (and would never be seen with \p no_fill) but
                /// should still be tracked. This only works it the labels are
                /// integer betweenfill_start and max(index found).
                /// (TODO: fill_end maybe?)
                enum class fill_modus
                {
                    fill,
                    no_fill
                };

                sort_modus sort;      ///< how to sort
                fill_modus fill;      ///< how to fill missing indices
                vert_type fill_start; ///< what the lowest index is supposed to be

                /// \brief dumps \p sort, \p fill and \p fill_start (if \p fill is
                /// set to "fill") to \p "remap" in the map \p m
                /// \param m \p std::map<std::string, std::string> like instance.
                template <typename M>
                void to_map(M &m) const
                {
                    s_param::to_map(m);
                    switch (sort)
                    {
                    case (sort_modus::sorted):
                        m["remap"] = "sorted";
                        break;
                    case (sort_modus::encounter):
                        m["remap"] = "encounter";
                        break;
                    default:
                        throw std::runtime_error(
                            "remap: sort option not implemented");
                    }

                    switch (fill)
                    {
                    case (fill_modus::fill):
                        m["remap"] += ",fill," + std::to_string(fill_start);
                        break;
                    case (fill_modus::no_fill):
                        m["remap"] += ",no_fill";
                        break;
                    default:
                        throw std::runtime_error(
                            "remap: fill option not implemented");
                    }
                }
                /// \brief loads "remap" from the map \p m into \p sort, \p fill
                /// and \p fill_start (if \p fill is set to "fill")
                /// \param m \p std::map<std::string, std::string> like instance.
                template <typename M>
                void from_map(M const &m)
                {
                    s_param::from_map(m);

                    std::string in_str =
                        fsc::get(m, "remap", "sorted,fill,0"); // setting default
                    auto in = fsc::split(in_str, ",");

                    if (in[0] == "sorted")
                    {
                        sort = sort_modus::sorted;
                    }
                    else if (in[0] == "encounter")
                    {
                        sort = sort_modus::encounter;
                    }
                    else
                    {
                        throw std::runtime_error("remap: sort option '" + in[0] +
                                                 "' not implemented");
                    }

                    if (in[1] == "fill")
                    {
                        fill = fill_modus::fill;
                        fill_start = fsc::sto<vert_type>(in[2]);
                    }
                    else if (in[1] == "no_fill")
                    {
                        fill = fill_modus::no_fill;
                    }
                    else
                    {
                        throw std::runtime_error("remap: fill option '" + in[1] +
                                                 "' not implemented");
                    }
                }
            };
            /// \brief Constructor
            /// \param p constructor argument for super and for setting sort, fill
            /// and fill_start
            remap(param const &p)
                : super(p),
                  sort_(p.sort),
                  fill_(p.fill),
                  fill_start_(p.fill_start) {}

            /// \brief initializes super and then the module
            /// \details remaps and fills missing indices as specified by the
            /// param members
            void init()
            {
                super::init();

                auto &connect = super::prot_get_connect();

                // for clearer code
                using temp_type = vert_type;

                // temporary relabeling
                std::map<user_type, temp_type> utot; // user to temporary labels
                temp_type temporary_label = 0;
                user_type maximal_label = user_type(); // just for nice error

                // secondary relabelling
                std::map<temp_type, vert_type>
                    ttoi; // temporary to internal labels
                vert_type internal_label = 0;

                auto subscribe = [&maximal_label, &temporary_label,
                                  &utot](user_type const &us) {
                    if (utot.find(us) == utot.end())
                    { // us not in utot
                        utot[us] = temporary_label;
                        maximal_label = std::max(maximal_label, us);
                        ++temporary_label;
                    }
                };

                // first temporary labeling
                auto const &itou = super::get_itou();
                for (auto &iact : connect)
                {
                    for (auto &vert : iact.vert_vec)
                    {
                        auto const &us = itou.at(vert);
                        subscribe(us);
                        vert = utot[us];
                    }
                }

                // check if we use all spins
                if (super::n_vert() > temporary_label)
                {
                    // add empty spins
                    if (fill_ == param::fill_modus::fill)
                    {
                        vert_type i = fill_start_;
                        while (super::n_vert() > temporary_label)
                        {
                            subscribe(fsc::sto<user_type>(std::to_string(i)));
                            ++i;
                        }
                        // if we use fill, we assume that the labels are smaller
                        // than
                        // super::n_vert(). Here we check this
                    }
                    else
                    {
                        std::clog
                            << "Warning: " << super::n_vert() - temporary_label
                            << " spins do not couple to anything!" << std::endl;
                    }
                }

                // if we fill, make sure there is no larger label than
                // super::n_vert()
                if (fill_ == param::fill_modus::fill)
                {
                    // this check makes only sense for integral user_types
                    if (std::is_integral<user_type>::value)
                    {
                        if (super::n_vert() + fill_start_ <=
                            fsc::sto<vert_type>(fsc::to_string(maximal_label)))
                            throw std::runtime_error(
                                std::string(
                                    "remap: inconsistent labels for option") +
                                " fill. Make sure the labels are smaller than " +
                                std::to_string(super::n_vert() + fill_start_) +
                                ". A label with value " +
                                fsc::to_string(maximal_label) + " was found");
                    }
                }
                // if the amount of unique spins is larger than super::n_vert(),
                // throw
                if (super::n_vert() < temporary_label)
                {
                    throw std::runtime_error(
                        "remap: the number of spins declared: " +
                        std::to_string(super::n_vert()) +
                        " is lower than the number " +
                        "of spins found: " + std::to_string(temporary_label));
                }
                // issue warning if file was empty
                if (temporary_label == 0)
                {
                    std::clog << "Warning: file " << super::get_filename()
                              << " contained no interactions!" << std::endl;
                }

                // build second relabelling mapping (sorted user_labels)
                for (auto const &x : utot)
                {
                    if (sort_ == param::sort_modus::encounter)
                    {
                        // identity (to match legacy code)
                        ttoi[x.second] = x.second;
                    }
                    else if (sort_ == param::sort_modus::sorted)
                    {
                        ttoi[x.second] = internal_label;
                        ++internal_label;
                    }
                    else
                    {
                        throw std::runtime_error(
                            "remap: sort option not implemented");
                    }
                }

                // fix interactions
                for (auto &iact : connect)
                {
                    // apply mapping
                    for (auto &vert : iact.vert_vec)
                        vert = ttoi[vert];

                    // sort spins for canonical form
                    std::sort(iact.vert_vec.begin(), iact.vert_vec.end());
                }
                super::prot_get_itou().clear();
                for (auto const &p : utot)
                    super::prot_get_itou()[ttoi.at(p.second)] = p.first;
            }
            /// \brief print a short help of the super and then itself
            void help() const
            {
                super::help();
                std::cout << "connect::remap:\n\
    Sorts the vertices in the connections in canonical order. Further remaps\n\
    vertices eighter sorted or encounter and fills holes (only if user_type is\n\
    integral) starting from fill_start. no_fill turns this off. The input format\n\
    for \"remap\" is \"sort_modus fill_modus fill_start\" e.g. \"sorted fill 0\""
                          << std::endl;
            }

        private:
            typename param::sort_modus sort_;
            typename param::fill_modus fill_;
            vert_type fill_start_;
        };
    } // end namespace connect
} // end namespace siquan
#endif // SIQUAN_CONNECT_REMAP_HPP_GUARD
