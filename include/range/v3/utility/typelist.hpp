// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_TYPELIST_HPP
#define RANGES_V3_UTILITY_TYPELIST_HPP

#include <cstddef>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/integer_sequence.hpp>

namespace ranges
{
    inline namespace v3
    {
        ////////////////////////////////////////////////////////////////////////////////////
        // typelist
        template<typename...Ts>
        struct typelist
        {
            using type = typelist;
            static constexpr std::size_t size() noexcept
            {
                return sizeof...(Ts);
            }
        };

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_concat
        template<typename List0, typename List1>
        struct typelist_concat;

        template<typename ...List1, typename ...List2>
        struct typelist_concat<typelist<List1...>, typelist<List2...>>
        {
            using type = typelist<List1..., List2...>;
        };

        template<typename List0, typename List1>
        using typelist_concat_t = typename typelist_concat<List0, List1>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // make_typelist
        // Generate lists<_,_,_,..._> with N arguments in O(log N)
        template<std::size_t N, typename T = void>
        struct make_typelist
          : typelist_concat<
                typename make_typelist<N / 2, T>::type,
                typename make_typelist<N - N / 2, T>::type>
        {};

        template<typename T>
        struct make_typelist<0, T>
        {
            using type = typelist<>;
        };

        template<typename T>
        struct make_typelist<1, T>
        {
            using type = typelist<T>;
        };

        template<std::size_t N, typename T = void>
        using make_typelist_t = typename make_typelist<N, T>::type;

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // typelist_element_
            template<typename VoidPtrs>
            struct typelist_element_;

            template<typename ...VoidPtrs>
            struct typelist_element_<typelist<VoidPtrs...>>
            {
                static empty eval(...);

                template<typename T, typename ...Us>
                static T eval(VoidPtrs..., T *, Us *...);
            };
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_element
        template<std::size_t N, typename List>
        struct typelist_element;

        template<std::size_t N, typename ...Ts>
        struct typelist_element<N, typelist<Ts...>>
          : decltype(detail::typelist_element_<make_typelist_t<N, void *>>
                ::eval((detail::identity<Ts> *)nullptr...))
        {};

        template<std::size_t N, typename List>
        using typelist_element_t = typename typelist_element<N, List>::type;

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // typelist_drop_
            template<typename VoidPtrs>
            struct typelist_drop_;

            template<typename ...VoidPtrs>
            struct typelist_drop_<typelist<VoidPtrs...>>
            {
                static empty eval(...);

                template<typename...Ts>
                static typelist<Ts...> eval(VoidPtrs..., identity<Ts> *...);
            };
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_drop
        template<std::size_t N, typename List>
        struct typelist_drop;

        template<std::size_t N, typename ...Ts>
        struct typelist_drop<N, typelist<Ts...>>
          : decltype(detail::typelist_drop_<make_typelist_t<N, void *>>
                ::eval((detail::identity<Ts> *)nullptr...))
        {};

        template<std::size_t N, typename List>
        using typelist_drop_t = typename typelist_drop<N, List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_front
        template<typename List>
        struct typelist_front
        {};

        template<typename Head, typename ...List>
        struct typelist_front<typelist<Head, List...>>
        {
            using type = Head;
        };

        template<typename List>
        using typelist_front_t = typename typelist_front<List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_push_front
        template<typename T, typename List>
        struct typelist_push_front;

        template<typename T, typename ...List>
        struct typelist_push_front<T, typelist<List...>>
        {
            using type = typelist<T, List...>;
        };

        template<typename T, typename List>
        using typelist_push_front_t = typename typelist_push_front<T, List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_pop_front
        template<typename List>
        struct typelist_pop_front
        {};

        template<typename Head, typename ...List>
        struct typelist_pop_front<typelist<Head, List...>>
        {
            using type = typelist<List...>;
        };

        template<typename List>
        using typelist_pop_front_t = typename typelist_pop_front<List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_back
        template<typename List>
        struct typelist_back
        {};

        template<typename Head, typename ...List>
        struct typelist_back<typelist<Head, List...>>
          : typelist_element<sizeof...(List), typelist<Head, List...>>
        {};

        template<typename List>
        using typelist_back_t = typename typelist_back<List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_push_back
        template<typename T, typename List>
        struct typelist_push_back;

        template<typename T, typename ...List>
        struct typelist_push_back<T, typelist<List...>>
        {
            using type = typelist<List..., T>;
        };

        template<typename T, typename List>
        using typelist_push_back_t = typename typelist_push_back<T, List>::type;

        // typelist_pop_back not provided because it cannot be made to meet the complixity
        // guarantees one would expect.

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_is_empty
        template<typename List>
        struct typelist_is_empty
          : std::false_type
        {};

        template<>
        struct typelist_is_empty<typelist<>>
          : std::true_type
        {};

        namespace detail
        {
            template<typename T, typename List>
            struct typelist_in_
            {
                static std::false_type has(void*);
            };

            template<typename T, typename Head, typename...List>
            struct typelist_in_<T, typelist<Head, List...>>
              : typelist_in_<T, typelist<List...>>
            {
                using typelist_in_<T, typelist<List...>>::has;
                static std::true_type has(detail::identity<Head>*);
            };
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_in
        template<typename T, typename List>
        struct typelist_in;

        template<typename T, typename ...List>
        struct typelist_in<T, typelist<List...>>
          : decltype(detail::typelist_in_<T, typelist<List...>>
                ::has((detail::identity<T>*)nullptr))
        {};

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // typelist_unique
            template<typename List, typename Result>
            struct typelist_unique_
            {
                using type = Result;
            };

            template<typename Head, typename...List, typename Result>
            struct typelist_unique_<typelist<Head, List...>, Result>
              : typelist_unique_<
                    typelist<List...>,
                    lazy_conditional_t<
                        typelist_in<Head, Result>::value,
                        identity<Result>,
                        typelist_push_back<Head, Result>>>
            {};
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_unique
        template<typename List>
        struct typelist_unique
          : detail::typelist_unique_<List, typelist<>>
        {};

        template<typename List>
        using typelist_unique_t = typename typelist_unique<List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_replace
        template<typename T, typename U, typename List>
        struct typelist_replace;

        template<typename T, typename U, typename...List>
        struct typelist_replace<T, U, typelist<List...>>
        {
            using type = typelist<
                detail::conditional_t<std::is_same<T, List>::value, U, List>...>;
        };

        template<typename T, typename U, typename List>
        using typelist_replace_t = typename typelist_replace<T, U, List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_expand
        template<template<typename...> class C, typename List>
        struct typelist_expand;

        template<template<typename...> class C, typename ...List>
        struct typelist_expand<C, typelist<List...>>
        {
            using type = C<List...>;
        };

        template<template<typename...> class C, typename List>
        using typelist_expand_t = typename typelist_expand<C, List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_transform
        template<typename List, template<typename...> class Fun>
        struct typelist_transform;

        template<typename ...List, template<typename...> class Fun>
        struct typelist_transform<typelist<List...>, Fun>
        {
            using type = typelist<Fun<List>...>;
        };

        template<typename List, template<typename...> class Fun>
        using typelist_transform_t = typename typelist_transform<List, Fun>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_transform2
        template<typename List0, typename List1, template<typename...> class Fun>
        struct typelist_transform2;

        template<typename ...List0, typename ...List1, template<typename...> class Fun>
        struct typelist_transform2<typelist<List0...>, typelist<List1...>, Fun>
        {
            using type = typelist<Fun<List0, List1>...>;
        };

        template<typename List0, typename List1, template<typename...> class Fun>
        using typelist_transform2_t = typename typelist_transform2<List0, List1, Fun>::type;

        namespace detail
        {
            template<typename IntegerSequence>
            struct typelist_integer_sequence_;

            template<std::size_t...Is>
            struct typelist_integer_sequence_<integer_sequence<Is...>>
            {
                using type = typelist<size_t<Is>...>;
            };
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_integer_sequence
        template<std::size_t N>
        struct typelist_integer_sequence
          : detail::typelist_integer_sequence_<integer_sequence_t<N>>
        {};

        template<std::size_t N>
        using typelist_integer_sequence_t = typename typelist_integer_sequence<N>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_find
        template<typename T, typename List>
        struct typelist_find
        {
            using type = typelist<>;
        };

        template<typename T, typename Head, typename ...List>
        struct typelist_find<T, typelist<Head, List...>>
          : typelist_find<T, typelist<List...>>
        {};

        template<typename T, typename ...List>
        struct typelist_find<T, typelist<T, List...>>
        {
            using type = typelist<T, List...>;
        };

        template<typename T, typename List>
        using typelist_find_t = typename typelist_find<T, List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_find
        template<template<typename...> class Fun, typename List>
        struct typelist_find_if
        {
            using type = typelist<>;
        };

        template<template<typename...> class Fun, typename Head, typename ...List>
        struct typelist_find_if<Fun, typelist<Head, List...>>
          : detail::conditional_t<
                Fun<Head>::value,
                detail::identity<typelist<Head, List...>>,
                typelist_find_if<Fun, typelist<List...>>>
        {};

        template<template<typename...> class Fun, typename List>
        using typelist_find_if_t = typename typelist_find_if<Fun, List>::type;
    }
}

#endif
