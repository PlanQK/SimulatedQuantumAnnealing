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
 * \brief Hold the base and the last class (finalizer) for the mixin
 * inheritance, as well as syntactic suger for deriving.
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015-2018 | ETH Zurich, D-PHYS, Mario S. Koenz
 * \copyright  Apache License, Version 2.0 */

#ifndef SIQUAN_BASE_HPP_GUARD
#define SIQUAN_BASE_HPP_GUARD

#include "param.hpp"

/// \brief (si)mulated (qu)antum (an)nealing namespace
/// \details
/// siquan (working title) it the outer namespace that houses all algorithms and
/// supporting functions.
namespace siquan {  // documented in base.hpp

/// \brief Implementation detail, the base class of the mixin inheritance chain.
/// \tparam type_carrier Should be a stateless struct only containing typedefs
/// required by the selected modules.
template <typename type_carrier>
struct base : public type_carrier {
    using param = seed_param;  ///< The base param type for any inheritance
                               ///< chain. The \p type_carrier should not have a
                               ///< param type.

    // all
    /// \brief Constructor
    base(param const &) {}

    /// \brief stops super::help calls, by not calling anything anymore.
    void help() const {}

    // connect/state
    /// \brief stops super::init calls, by not calling anything anymore.
    void init() {}

    // algo
    /// \brief stops super::update calls, by not calling anything anymore.
    void update() {}
    /// \brief stops super::step calls, by not calling anything anymore.
    void step() {}
    /// \brief stops super::finish calls, by not calling anything anymore.
    void finish() {}

    // scheduler
    /// \brief stops super::advance calls, by not calling anything anymore.
    template <typename O>
    void advance(O const &) {}
    /// \brief stops super::stop calls, by returning false (i.e. this module
    /// never stops the simulation)
    bool stop() const { return false; }

    // readout
    /// \brief stops super::to_map calls, by not calling anything anymore.
    template <typename M>
    void to_map(M &) const {}

protected:
    /// \brief stops super::calc calls, by not calling anything anymore.
    void calc() {}
};

/// \brief Implementation detail, the last/bottom class of the mixin inheritance
/// chain. \tparam super Is a module that inherites ultimatly from siquan::base.
template <typename super>
struct finalizer : public super {
public:
    using param =
        typename super::param;  ///< \p finalizer does just use the super::param

    // all
    /// \brief Constructor, just passing through \p p
    /// \param p Instance of \p param
    finalizer(param const &p) : super(p) {}

    /// \brief Calls super::calc() followed by super::finish()
    void finish() {
        super::calc();
        super::finish();
    }
};

/// \cond IMPLEMENTATION_DETAIL_DOC
namespace detail {
    template <typename type_carrier, template <typename> class... Args>
    struct recursive_deriver;

    template <typename type_carrier>
    struct recursive_deriver<type_carrier> {
        using type = type_carrier;
    };

    template <typename type_carrier, template <typename> class A,
              template <typename> class... Args>
    struct recursive_deriver<type_carrier, A, Args...> {
        using type = typename recursive_deriver<A<type_carrier>, Args...>::type;
    };
}  // end namespace detail
/// \endcond

/// \brief syntactic suger for inheritance mechanism
/// \tparam type_carrier see siquan::base
/// \tparam Args... all the modules that should be mixin inherited
/// \details This typedef provides a more readable version for \p
/// finalizer<C<B<A<base<type_carrier>>>>> which is \p compose<type_carrier, A,
/// B, C>. \p A has no knowledge of \p B, but \p B can see measurements/values
/// of \p A
template <typename type_carrier, template <typename> class... Args>
using compose = typename detail::recursive_deriver<base<type_carrier>, Args...,
                                                   finalizer>::type;
}  // end namespace siquan

#endif  // SIQUAN_BASE_HPP_GUARD
