/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_COMMON_TYPE_HPP
#define RANGES_V3_UTILITY_COMMON_TYPE_HPP

#include <utility>

#include <meta/meta.hpp>

#include <concepts/type_traits.hpp>

#include <range/v3/range_fwd.hpp>

// Sadly, this is necessary because of:
//  - std::common_type is !SFINAE-friendly, and
//  - The specification of std::common_type makes it impossibly
//    difficult to specialize on user-defined types without spamming
//    out a bajillion copies to handle all combinations of cv and ref
//    qualifiers.

#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
// Non-portable forward declarations of standard containers
RANGES_BEGIN_NAMESPACE_STD
    RANGES_BEGIN_NAMESPACE_VERSION
        template<typename... Ts>
        class tuple;
    RANGES_END_NAMESPACE_VERSION
RANGES_END_NAMESPACE_STD
#else
#include <tuple>
#endif

namespace ranges
{
    template<typename... Ts>
    using common_type = concepts::common_type<Ts...>;

    template<typename... Ts>
    using common_type_t = concepts::common_type_t<Ts...>;

    template<typename... Ts>
    using common_reference = concepts::common_reference<Ts...>;

    template<typename... Ts>
    using common_reference_t = concepts::common_reference_t<Ts...>;
} // namespace ranges

namespace concepts
{
    /// \cond
    // Specializations for pair and tuple
    template<typename F, typename S>
    struct common_pair;

    template<typename... Ts>
    struct common_tuple;

    // common_type for std::pairs
    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<std::pair<F1, S1>, common_pair<F2, S2>>;

    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<common_pair<F1, S1>, std::pair<F2, S2>>;

    template<typename F1, typename S1, typename F2, typename S2>
    struct common_type<common_pair<F1, S1>, common_pair<F2, S2>>;

    // common_type for std::tuples
    template<typename... Ts, typename... Us>
    struct common_type<common_tuple<Ts...>, std::tuple<Us...>>;

    template<typename... Ts, typename... Us>
    struct common_type<std::tuple<Ts...>, common_tuple<Us...>>;

    template<typename... Ts, typename... Us>
    struct common_type<common_tuple<Ts...>, common_tuple<Us...>>;

    // A common reference for std::pairs
    template<typename F1, typename S1, typename F2, typename S2,
             template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<common_pair<F1, S1>, std::pair<F2, S2>, Qual1, Qual2>;

    template<typename F1, typename S1, typename F2, typename S2,
             template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<std::pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>;

    template<typename F1, typename S1, typename F2, typename S2,
             template<typename> class Qual1, template<typename> class Qual2>
    struct basic_common_reference<common_pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>;

    // A common reference for std::tuples
    template<typename... Ts, typename... Us, template<typename> class Qual1,
             template<typename> class Qual2>
    struct basic_common_reference<common_tuple<Ts...>, std::tuple<Us...>, Qual1, Qual2>;

    template<typename... Ts, typename... Us, template<typename> class Qual1,
             template<typename> class Qual2>
    struct basic_common_reference<std::tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>;

    template<typename... Ts, typename... Us, template<typename> class Qual1,
             template<typename> class Qual2>
    struct basic_common_reference<common_tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>;
    /// \endcond
} // namespace concepts

#endif
