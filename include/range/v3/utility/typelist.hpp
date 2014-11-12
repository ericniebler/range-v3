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

#ifndef RANGES_V3_UTILITY_TYPELIST_HPP
#define RANGES_V3_UTILITY_TYPELIST_HPP

#include <cstddef>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/nullval.hpp>
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
        // typelist_size
        template<typename List>
        struct typelist_size
        {
            using type = index_t<List::size()>;
        };

        template<typename List>
        using typelist_size_t = typename typelist_size<List>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_cat
        template<typename...Lists>
        struct typelist_cat
        {};

        template<>
        struct typelist_cat<>
        {
            using type = typelist<>;
        };

        template<typename...List1>
        struct typelist_cat<typelist<List1...>>
        {
            using type = typelist<List1...>;
        };

        template<typename ...List1, typename ...List2>
        struct typelist_cat<typelist<List1...>, typelist<List2...>>
        {
            using type = typelist<List1..., List2...>;
        };

        template<typename ...List1, typename ...List2, typename...Rest>
        struct typelist_cat<typelist<List1...>, typelist<List2...>, Rest...>
          : typelist_cat<typelist<List1..., List2...>, Rest...>
        {};

        template<typename...Lists>
        using typelist_cat_t = typename typelist_cat<Lists...>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // make_typelist
        // Generate lists<_,_,_,..._> with N arguments in O(log N)
        template<std::size_t N, typename T = void>
        struct make_typelist
          : typelist_cat<
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
        struct typelist_element
        {};

        template<std::size_t N, typename ...Ts>
        struct typelist_element<N, typelist<Ts...>>
          : decltype(detail::typelist_element_<make_typelist_t<N, void *>>
                ::eval(nullval<detail::identity<Ts>>()...))
        {};

        template<std::size_t N, typename List>
        using typelist_element_t = typename typelist_element<N, List>::type;

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // typelist_drop_
            template<typename VoidPtrs>
            struct typelist_drop_
            {
                static empty eval(...);
            };

            template<typename ...VoidPtrs>
            struct typelist_drop_<typelist<VoidPtrs...>>
            {
                static empty eval(...);

                template<typename...Ts>
                static identity<typelist<Ts...>> eval(VoidPtrs..., identity<Ts> *...);
            };
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_drop
        template<std::size_t N, typename List>
        struct typelist_drop
        {};

        template<std::size_t N, typename ...Ts>
        struct typelist_drop<N, typelist<Ts...>>
          : decltype(detail::typelist_drop_<make_typelist_t<N, void *>>
                ::eval(nullval<detail::identity<Ts>>()...))
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
        template<typename List, typename T>
        struct typelist_push_front
        {};

        template<typename ...List, typename T>
        struct typelist_push_front<typelist<List...>, T>
        {
            using type = typelist<T, List...>;
        };

        template<typename List, typename T>
        using typelist_push_front_t = typename typelist_push_front<List, T>::type;

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
        template<typename List, typename T>
        struct typelist_push_back
        {};

        template<typename ...List, typename T>
        struct typelist_push_back<typelist<List...>, T>
        {
            using type = typelist<List..., T>;
        };

        template<typename List, typename T>
        using typelist_push_back_t = typename typelist_push_back<List, T>::type;

        // typelist_pop_back not provided because it cannot be made to meet the complexity
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

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_in
        template<typename List, typename T>
        struct typelist_in
          : std::false_type
        {};

        template<typename ...List, typename T, typename U>
        struct typelist_in<typelist<U, List...>, T>
          : typelist_in<typelist<List...>, T>
        {};

        template<typename ...List, typename T>
        struct typelist_in<typelist<T, List...>, T>
          : std::true_type
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
                        typelist_in<Result, Head>::value,
                        identity<Result>,
                        typelist_push_back<Result, Head>>>
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
        template<typename List, typename T, typename U>
        struct typelist_replace;

        template<typename...List, typename T, typename U>
        struct typelist_replace<typelist<List...>, T, U>
        {
            using type = typelist<
                detail::conditional_t<std::is_same<T, List>::value, U, List>...>;
        };

        template<typename List, typename T, typename U>
        using typelist_replace_t = typename typelist_replace<List, T, U>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_expand
        template<typename List, template<typename...> class C>
        struct typelist_expand
        {};

        template<typename ...List, template<typename...> class C>
        struct typelist_expand<typelist<List...>, C>
        {
            using type = C<List...>;
        };

        template<typename List, template<typename...> class C>
        using typelist_expand_t = typename typelist_expand<List, C>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_transform
        template<typename List, typename Fun>
        struct typelist_transform
        {};

        template<typename ...List, typename Fun>
        struct typelist_transform<typelist<List...>, Fun>
        {
            using type = typelist<meta_apply<Fun, List>...>;
        };

        template<typename List, typename Fun>
        using typelist_transform_t = typename typelist_transform<List, Fun>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_transform2
        template<typename List0, typename List1, typename Fun>
        struct typelist_transform2
        {};

        template<typename ...List0, typename ...List1, typename Fun>
        struct typelist_transform2<typelist<List0...>, typelist<List1...>, Fun>
        {
            using type = typelist<meta_apply<Fun, List0, List1>...>;
        };

        template<typename List0, typename List1, typename Fun>
        using typelist_transform2_t = typename typelist_transform2<List0, List1, Fun>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // as_typelist
        template<typename Sequence>
        struct as_typelist
        {};

        template<typename T, T...Is>
        struct as_typelist<integer_sequence<T, Is...>>
        {
            using type = typelist<std::integral_constant<T, Is>...>;
        };

        template<typename Sequence>
        using as_typelist_t = typename as_typelist<Sequence>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_find
        template<typename List, typename T>
        struct typelist_find
        {
            using type = typelist<>;
        };

        template<typename Head, typename ...List, typename T>
        struct typelist_find<typelist<Head, List...>, T>
          : typelist_find<typelist<List...>, T>
        {};

        template<typename ...List, typename T>
        struct typelist_find<typelist<T, List...>, T>
        {
            using type = typelist<T, List...>;
        };

        template<typename List, typename T>
        using typelist_find_t = typename typelist_find<List, T>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_find
        template<typename List, typename Fun>
        struct typelist_find_if
        {
            using type = typelist<>;
        };

        template<typename Head, typename ...List, typename Fun>
        struct typelist_find_if<typelist<Head, List...>, Fun>
          : detail::conditional_t<
                meta_apply<Fun, Head>::value,
                detail::identity<typelist<Head, List...>>,
                typelist_find_if<typelist<List...>, Fun>>
        {};

        template<typename List, typename Fun>
        using typelist_find_if_t = typename typelist_find_if<List, Fun>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_foldl
        template<typename List, typename State, typename Fun>
        struct typelist_foldl
        {
            using type = State;
        };

        template<typename Head, typename ...List, typename State, typename Fun>
        struct typelist_foldl<typelist<Head, List...>, State, Fun>
          : typelist_foldl<typelist<List...>, meta_apply<Fun, State, Head>, Fun>
        {};

        template<typename List, typename State, typename Fun>
        using typelist_foldl_t = typename typelist_foldl<List, State, Fun>::type;

        ////////////////////////////////////////////////////////////////////////////////////
        // typelist_foldr
        template<typename List, typename State, typename Fun>
        struct typelist_foldr
        {
            using type = State;
        };

        template<typename Head, typename ...List, typename State, typename Fun>
        struct typelist_foldr<typelist<Head, List...>, State, Fun>
          : meta_apply<Fun, typelist_foldr<typelist<List...>, State, Fun>, Head>
        {};

        template<typename List, typename State, typename Fun>
        using typelist_foldr_t = typename typelist_foldr<List, State, Fun>::type;
    }
}

#endif
