// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_COMPRESSED_PAIR_HPP
#define RANGES_V3_UTILITY_COMPRESSED_PAIR_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T, typename Enable = void>
            struct first_base
            {
                T first;
                first_base() = default;
                template<typename U,
                    typename std::enable_if<std::is_constructible<T, U &&>::value, int>::type = 0>
                constexpr explicit first_base(U && u)
                  : first((U &&) u)
                {}
            };

            template<typename T>
            struct first_base<T, typename std::enable_if<std::is_empty<T>::value &&
                std::is_trivial<T>::value>::type>
            {
                static T first;
                first_base() = default;
                template<typename U,
                    typename std::enable_if<std::is_constructible<T, U &&>::value, int>::type = 0>
                constexpr explicit first_base(U &&)
                {}
            };

            template<typename T>
            T first_base<T, typename std::enable_if<std::is_empty<T>::value &&
                std::is_trivial<T>::value>::type>::first{};

            template<typename T, typename Enable = void>
            struct second_base
            {
                T second;
                second_base() = default;
                template<typename U,
                    typename std::enable_if<std::is_constructible<T, U &&>::value, int>::type = 0>
                constexpr explicit second_base(U && u)
                  : second((U &&) u)
                {}
            };

            template<typename T>
            struct second_base<T, typename std::enable_if<std::is_empty<T>::value &&
                std::is_trivial<T>::value>::type>
            {
                static T second;
                second_base() = default;
                template<typename U,
                    typename std::enable_if<std::is_constructible<T, U &&>::value, int>::type = 0>
                constexpr explicit second_base(U &&)
                {}
            };

            template<typename T>
            T second_base<T, typename std::enable_if<std::is_empty<T>::value &&
                std::is_trivial<T>::value>::type>::second{};

            template<typename T>
            struct pair_element
              : meta::if_<std::is_same<decay_t<T>, T>, meta::id<T>, pair_element<decay_t<T>>>
            {};

            template<typename T>
            struct pair_element<std::reference_wrapper<T>>
            {
                using type = T &;
            };

            template<typename T>
            using pair_element_t = meta::eval<pair_element<T>>;
        }
        /// \endcond

        template<typename First, typename Second>
        struct compressed_pair
          : private detail::first_base<First>
          , private detail::second_base<Second>
        {
            using first_type = First;
            using second_type = Second;
            using detail::first_base<First>::first;
            using detail::second_base<Second>::second;

            compressed_pair() = default;

            constexpr compressed_pair(First f, Second s)
              : detail::first_base<First>{(First &&) f}
              , detail::second_base<Second>{(Second &&) s}
            {}

            template<typename F, typename S,
                typename std::enable_if<std::is_constructible<First, F &&>::value &&
                                        std::is_constructible<Second, S &&>::value, int>::type = 0>
            constexpr compressed_pair(F && f, S && s)
              : detail::first_base<First>{(F &&) f}
              , detail::second_base<Second>{(S &&) s}
            {}

            template<typename F, typename S,
                typename std::enable_if<std::is_constructible<F, First const &>::value &&
                                        std::is_constructible<S, Second const &>::value, int>::type = 0>
            constexpr operator std::pair<F, S> () const
            {
                return std::pair<F, S>{first, second};
            }
        };

        struct make_compressed_pair_fn
        {
            template<typename First, typename Second>
            constexpr auto operator()(First && f, Second && s) const ->
                compressed_pair<detail::pair_element_t<First>, detail::pair_element_t<Second>>
            {
                return {detail::forward<First>(f), detail::forward<Second>(s)};
            }
        };

        /// \ingroup group-utility
        /// \sa `make_compressed_pair_fn`
        constexpr make_compressed_pair_fn make_compressed_pair {};

        /// \brief Tuple-like access of `compressed_pair`
        // TODO Switch to variable template when available
        template<std::size_t I, typename First, typename Second,
            CONCEPT_REQUIRES_(I == 0)>
        constexpr auto get(compressed_pair<First, Second> & p) ->
            decltype((p.first))
        {
            return p.first;
        }

        /// \overload
        template<std::size_t I, typename First, typename Second,
            CONCEPT_REQUIRES_(I == 0)>
        constexpr auto get(compressed_pair<First, Second> const & p) ->
            decltype((p.first))
        {
            return p.first;
        }

        /// \overload
        template<std::size_t I, typename First, typename Second,
            CONCEPT_REQUIRES_(I == 0)>
        constexpr auto get(compressed_pair<First, Second> && p) ->
            decltype((detail::move(p).first))
        {
            return detail::move(p).first;
        }

        /// \overload
        template<std::size_t I, typename First, typename Second,
            CONCEPT_REQUIRES_(I == 1)>
        constexpr auto get(compressed_pair<First, Second> & p) ->
            decltype((p.second))
        {
            return p.second;
        }

        /// \overload
        template<std::size_t I, typename First, typename Second,
            CONCEPT_REQUIRES_(I == 1)>
        constexpr auto get(compressed_pair<First, Second> const & p) ->
            decltype((p.second))
        {
            return p.second;
        }

        /// \overload
        template<std::size_t I, typename First, typename Second,
            CONCEPT_REQUIRES_(I == 1)>
        constexpr auto get(compressed_pair<First, Second> && p) ->
            decltype((detail::move(p).second))
        {
            return detail::move(p).second;
        }
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wmismatched-tags"

namespace std
{
    template<typename First, typename Second>
    struct tuple_size< ::ranges::v3::compressed_pair<First, Second>>
      : std::integral_constant<size_t, 2>
    {};

    template<typename First, typename Second>
    struct tuple_element<0, ::ranges::v3::compressed_pair<First, Second>>
    {
        using type = First;
    };

    template<typename First, typename Second>
    struct tuple_element<1, ::ranges::v3::compressed_pair<First, Second>>
    {
        using type = Second;
    };
}

#pragma GCC diagnostic pop

#endif
