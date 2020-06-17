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
 * \brief uniform random generator with a local static engine
 * \author
 * Year      |  Name
 * ----      | -----
 * 2015 | ETH Zurich, D-PHYS, Programming Techniques for Physical Simulations II
 * \copyright For free use, no rights reserved. */

#ifndef UTIL_RANDOM_HEADER
#define UTIL_RANDOM_HEADER

#include <time.h>
#include <random>
#include <type_traits>

/// \brief Holds local static seeds and engines, for consistent seeding across
/// compilation units.
namespace util {
/// \cond IMPLEMENTATION_DETAIL_DOC
namespace detail {
    using seed_type = uint64_t;

    template <typename ENGINE>
    ENGINE& get_engine(seed_type const& seed) {
        static ENGINE engine(seed);
        return engine;
    }
    template <typename ENGINE>
    seed_type& get_seed(seed_type const& init = 0) {
        static seed_type seed = init;
        return seed;
    }

    template <typename T, bool B>
    struct get_distr {
        using type = std::uniform_int_distribution<T>;
    };
    template <typename T>
    struct get_distr<T, false> {
        using type = std::uniform_real_distribution<T>;
    };
}  // end namespace detail
/// \endcond

/// \brief seeds a local static engine
/// \tparam ENGINE is the type of the engine we want to seed
/// \param seed
template <typename ENGINE = std::mt19937>
void seed(detail::seed_type const& seed) {
    auto& seedref = detail::get_seed<ENGINE>();
    auto& engineref = detail::get_engine<ENGINE>(seed);
    seedref = seed;
    engineref.seed(seedref);
}

/// \brief get the seed of a local static engine
/// \tparam ENGINE is the type of the engine we want to get the seed from
template <typename ENGINE = std::mt19937>
detail::seed_type seed() {
    return detail::get_seed<ENGINE>();
}

/// \brief a simple fast uniform random number generator using a local static
/// engine
/// \tparam T type of the random number generator \tparam ENGINE type of
/// the engine to use
/// \details Be aware that there is only ever one engine of
/// one type, i.e. it you create two identical rng_class instances, they will
/// use the same engine and even if seeded the same not return the same values.
template <typename T, typename ENGINE = std::mt19937>
class rng_class {
    using seed_type = detail::seed_type;
    using distr_type =
        typename detail::get_distr<T, std::is_integral<T>::value>::type;

public:
    /// \brief
    /// \param lower,upper the bounds of the random intervall
    /// \details For integral types \p T, the upper bound is included, as for
    /// floating point types it is not.
    rng_class(T const& lower, T const& upper)
        : seedref_(detail::get_seed<ENGINE>(uint64_t(time(NULL)))),
          engineref_(detail::get_engine<ENGINE>(seedref_)),
          distr_(lower, upper) {}

    /// \brief Get a random number
    T operator()() { return distr_(engineref_); }
    /// \brief get the seed
    seed_type const& seed() noexcept { return seedref_; }
    /// \brief set the seed
    /// \param seed the desired value
    void seed(seed_type const& seed) noexcept {
        seedref_ = seed;
        engineref_.seed(seedref_);
    }
    /// \brief change the range of the uniform distribution
    /// \param lower,upper the new range
    void set_range(T const& lower, T const& upper) {
        distr_ = distr_type(lower, upper);
    }

private:
    seed_type& seedref_;
    ENGINE& engineref_;
    distr_type distr_;
};
}  // end namespace util
#endif  // UTIL_RANDOM_HEADER
