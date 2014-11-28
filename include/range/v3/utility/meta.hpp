// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_META_HPP
#define RANGES_V3_UTILITY_META_HPP

#include <type_traits>
#include <range/v3/utility/nullptr_v.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T, T...>
        struct integer_sequence;

        namespace meta
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // General meta-programming utilities
            template<typename...Ts>
            struct list;

            template<typename T>
            using eval = typename T::type;

            template<typename F, typename...Args>
            using apply = typename F::template apply<Args...>;

            template<std::size_t N>
            using size_t = std::integral_constant<std::size_t, N>;

            template<bool B>
            using bool_ = std::integral_constant<bool, B>;

            template<typename T>
            struct id
            {
                using type = T;
            };

            template<template<typename...> class C>
            struct quote
            {
            private:
                // Indirection here needed to avoid Core issue 1430
                // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
                template<typename...Ts>
                struct impl
                {
                    using type = C<Ts...>;
                };
            public:
                template<typename...Ts>
                using apply = eval<impl<Ts...>>;
            };

            template<template<typename...> class C>
            struct quote_trait
            {
                template<typename...Ts>
                using apply = eval<apply<quote<C>, Ts...> >;
            };

            template<typename T, template<T...> class F>
            struct quote_i
            {
            private:
                // Indirection here needed to avoid Core issue 1430
                // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
                template<typename ...Ts>
                struct impl
                {
                    using type = F<Ts::value...>;
                };
            public:
                template<typename...Ts>
                using apply = eval<impl<Ts...>>;
            };

            template<typename T, template<T...> class C>
            struct quote_trait_i
            {
                template<typename...Ts>
                using apply = eval<apply<quote_i<T, C>, Ts...> >;
            };

            template<typename...Fs>
            struct compose
            {};

            template<typename F0>
            struct compose<F0>
            {
                template<typename...Ts>
                using apply = apply<F0, Ts...>;
            };

            template<typename F0, typename...Fs>
            struct compose<F0, Fs...>
            {
                template<typename...Ts>
                using apply = apply<F0, apply<compose<Fs...>, Ts...>>;
            };

            template<typename T>
            struct always
            {
                template<typename...>
                using apply = T;
            };

            template<typename F, typename...Ts>
            struct bind_front
            {
                template<typename...Us>
                using apply = apply<F, Ts..., Us...>;
            };

            template<typename F, typename...Us>
            struct bind_back
            {
                template<typename...Ts>
                using apply = apply<F, Ts..., Us...>;
            };

            template<typename T>
            struct unpack_into
            {};

            template<template<typename...> class T, typename ...Ts>
            struct unpack_into<T<Ts...>>
            {
                template<typename F>
                using apply = apply<F, Ts...>;
            };

            template<typename T, T...Is>
            struct unpack_into<integer_sequence<T, Is...>>
            {
                template<typename F>
                using apply = apply<F, std::integral_constant<T, Is>...>;
            };

            template<typename F, typename Q = quote<list>>
            struct curry
              : compose<F, Q>
            {};

            template<typename F>
            struct uncurry
            {
                template<typename T>
                using apply = apply<unpack_into<T>, F>;
            };

            template<typename F>
            struct flip
            {
            private:
                template<typename ...Ts>
                struct impl
                {};
                template<typename A, typename B, typename ...Ts>
                struct impl<A, B, Ts...>
                {
                    using type = apply<F, B, A, Ts...>;
                };
            public:
                template<typename ...Ts>
                using apply = eval<impl<Ts...>>;
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // if_
            namespace meta_detail
            {
                template<bool If, typename Then, typename Else>
                struct _if_
                {
                    using type = Else;
                };

                template<typename Then, typename Else>
                struct _if_<true, Then, Else>
                {
                    using type = Then;
                };
            }

            template<typename If, typename Then, typename Else>
            using if_ = eval<meta_detail::_if_<If::value, Then, Else>>;

            template<bool If, typename Then, typename Else>
            using if_c = eval<meta_detail::_if_<If, Then, Else>>;

            namespace meta_detail
            {
                // Thanks to  Louis Dionne for this clever hack for a quick-to-compile
                // implementation of and_c and or_c
                std::true_type fast_and_impl_();

                template<typename ...T>
                std::true_type fast_and_impl_(T*...);

                template<typename ...T>
                std::false_type fast_and_impl_(T...);

                std::false_type fast_or_impl_();

                template<typename ...T>
                std::false_type fast_or_impl_(T*...);

                template<typename ...T>
                std::true_type fast_or_impl_(T...);

                template<typename ...Bools>
                struct _and_;

                template<>
                struct _and_<>
                  : std::true_type
                {};

                template<typename Bool, typename...Bools>
                struct _and_<Bool, Bools...>
                  : if_c<!Bool::value, std::false_type, _and_<Bools...>>
                {};

                template<typename ...Bools>
                struct _or_;

                template<>
                struct _or_<>
                  : std::false_type
                {};

                template<typename Bool, typename...Bools>
                struct _or_<Bool, Bools...>
                  : if_c<Bool::value, std::true_type, _or_<Bools...>>
                {};
            }

            template<bool ...Bools>
            using and_c =
                decltype(meta_detail::fast_and_impl_(if_c<Bools, int*, int>{}...));

            template<bool ...Bools>
            using or_c =
                decltype(meta_detail::fast_or_impl_(if_c<Bools, int, int*>{}...));

            template<bool Bool>
            using not_c = bool_<!Bool>;

            template<typename...Bools>
            using fast_and = and_c<Bools::value...>;

            template<typename...Bools>
            using fast_or = or_c<Bools::value...>;

            template<typename Bool>
            using not_ = not_c<Bool::value>;

            template<typename...Bools>
            using and_ = eval<meta_detail::_and_<Bools...>>;

            template<typename...Bools>
            using or_ = eval<meta_detail::_or_<Bools...>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // list
            template<typename...Ts>
            struct list
            {
                using type = list;
                static constexpr std::size_t size() noexcept
                {
                    return sizeof...(Ts);
                }
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // size
            template<typename List>
            using size = meta::size_t<List::size()>;

            ////////////////////////////////////////////////////////////////////////////////////
            // list_cat
            namespace meta_detail
            {
                template<typename ListOfLists>
                struct list_cat_
                {};

                template<>
                struct list_cat_<list<>>
                {
                    using type = list<>;
                };

                template<typename...List1>
                struct list_cat_<list<list<List1...>>>
                {
                    using type = list<List1...>;
                };

                template<typename ...List1, typename ...List2>
                struct list_cat_<list<list<List1...>, list<List2...>>>
                {
                    using type = list<List1..., List2...>;
                };

                template<typename ...List1, typename ...List2, typename...List3>
                struct list_cat_<list<list<List1...>, list<List2...>, list<List3...>>>
                {
                    using type = list<List1..., List2..., List3...>;
                };

                template<typename ...List1, typename ...List2, typename...List3, typename...Rest>
                struct list_cat_<list<list<List1...>, list<List2...>, list<List3...>, Rest...>>
                  : list_cat_<list<list<List1..., List2..., List3...>, Rest...>>
                {};
            }

            template<typename ListOfLists>
            using list_cat = eval<meta_detail::list_cat_<ListOfLists>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // repeat_n
            // Generate lists<_,_,_,..._> with N arguments in O(log N)
            namespace meta_detail
            {
                template<std::size_t N, typename T>
                struct repeat_n_c_
                {
                    using type =
                        list_cat<
                            list<
                                eval<repeat_n_c_<N / 2, T>>,
                                eval<repeat_n_c_<N / 2, T>>,
                                eval<repeat_n_c_<N % 2, T>>>>;
                };

                template<typename T>
                struct repeat_n_c_<0, T>
                {
                    using type = list<>;
                };

                template<typename T>
                struct repeat_n_c_<1, T>
                {
                    using type = list<T>;
                };
            }

            template<typename N, typename T = void>
            using repeat_n = eval<meta_detail::repeat_n_c_<N::value, T>>;

            template<std::size_t N, typename T = void>
            using repeat_n_c = eval<meta_detail::repeat_n_c_<N, T>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // list_element
            namespace meta_detail
            {
                struct empty {};

                template<typename VoidPtrs>
                struct list_element_impl_;

                template<typename ...VoidPtrs>
                struct list_element_impl_<list<VoidPtrs...>>
                {
                    static empty eval(...);

                    template<typename T, typename ...Us>
                    static T eval(VoidPtrs..., T *, Us *...);
                };

                template<typename N, typename List>
                struct list_element_
                {};

                template<typename N, typename...Ts>
                struct list_element_<N, list<Ts...>>
                  : decltype(list_element_impl_<repeat_n<N, void *>>::eval(_nullptr_v<id<Ts>>()...))
                {};
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // list_element
            template<typename N, typename List>
            using list_element = eval<meta_detail::list_element_<N, List>>;

            template<std::size_t N, typename List>
            using list_element_c = list_element<meta::size_t<N>, List>;

            ////////////////////////////////////////////////////////////////////////////////////
            // drop
            namespace meta_detail
            {
                ////////////////////////////////////////////////////////////////////////////////////
                // drop_impl_
                template<typename VoidPtrs>
                struct drop_impl_
                {
                    static empty eval(...);
                };

                template<typename ...VoidPtrs>
                struct drop_impl_<list<VoidPtrs...>>
                {
                    static empty eval(...);

                    template<typename...Ts>
                    static id<list<Ts...>> eval(VoidPtrs..., id<Ts> *...);
                };

                template<typename N, typename List>
                struct drop_
                {};

                template<typename N, typename ...Ts>
                struct drop_<N, list<Ts...>>
                  : decltype(drop_impl_<repeat_n<N, void *>>::eval(_nullptr_v<id<Ts>>()...))
                {};
            }

            template<typename N, typename List>
            using drop = eval<meta_detail::drop_<N, List>>;

            template<std::size_t N, typename List>
            using drop_c = eval<meta_detail::drop_<meta::size_t<N>, List>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // front
            namespace meta_detail
            {
                template<typename List>
                struct front_
                {};

                template<typename Head, typename ...List>
                struct front_<list<Head, List...>>
                {
                    using type = Head;
                };
            }

            template<typename List>
            using front = eval<meta_detail::front_<List>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // back
            namespace meta_detail
            {
                template<typename List>
                struct back_
                {};

                template<typename Head, typename ...List>
                struct back_<list<Head, List...>>
                {
                    using type = list_element_c<sizeof...(List), list<Head, List...>>;
                };
            }

            template<typename List>
            using back = eval<meta_detail::back_<List>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // push_front
            namespace meta_detail
            {
                template<typename List, typename T>
                struct push_front_
                {};

                template<typename ...List, typename T>
                struct push_front_<list<List...>, T>
                {
                    using type = list<T, List...>;
                };
            }

            template<typename List, typename T>
            using push_front = eval<meta_detail::push_front_<List, T>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // pop_front
            namespace meta_detail
            {
                template<typename List>
                struct pop_front_
                {};

                template<typename Head, typename ...List>
                struct pop_front_<list<Head, List...>>
                {
                    using type = list<List...>;
                };
            }

            template<typename List>
            using pop_front = eval<meta_detail::pop_front_<List>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // push_back
            namespace meta_detail
            {
                template<typename List, typename T>
                struct push_back_
                {};

                template<typename ...List, typename T>
                struct push_back_<list<List...>, T>
                {
                    using type = list<List..., T>;
                };
            }

            template<typename List, typename T>
            using push_back = eval<meta_detail::push_back_<List, T>>;

            // pop_back not provided because it cannot be made to meet the complexity
            // guarantees one would expect.

            ////////////////////////////////////////////////////////////////////////////////////
            // empty
            template<typename List>
            using emtpy = bool_<0 == size<List>::value>;

            ////////////////////////////////////////////////////////////////////////////////////
            // find
            namespace meta_detail
            {
                template<typename List, typename T>
                struct find_
                {};

                template<typename T>
                struct find_<list<>, T>
                {
                    using type = list<>;
                };

                template<typename Head, typename ...List, typename T>
                struct find_<list<Head, List...>, T>
                  : find_<list<List...>, T>
                {};

                template<typename ...List, typename T>
                struct find_<list<T, List...>, T>
                {
                    using type = list<T, List...>;
                };
            }

            template<typename List, typename T>
            using find = eval<meta_detail::find_<List, T>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // find_if
            namespace meta_detail
            {
                template<typename List, typename Fun>
                struct find_if_
                {};

                template<typename Fun>
                struct find_if_<list<>, Fun>
                {
                    using type = list<>;
                };

                template<typename Head, typename ...List, typename Fun>
                struct find_if_<list<Head, List...>, Fun>
                  : if_<apply<Fun, Head>, id<list<Head, List...>>, find_if_<list<List...>, Fun>>
                {};
            }

            template<typename List, typename Fun>
            using find_if = eval<meta_detail::find_if_<List, Fun>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // in
            template<typename List, typename T>
            using in = bool_<0 != size<find<List, T>>::value>;

            ////////////////////////////////////////////////////////////////////////////////////
            // unique
            namespace meta_detail
            {
                template<typename List, typename Result>
                struct unique_
                {};

                template<typename Result>
                struct unique_<list<>, Result>
                {
                    using type = Result;
                };

                template<typename Head, typename...List, typename Result>
                struct unique_<list<Head, List...>, Result>
                  : unique_<
                        list<List...>,
                        apply<
                            if_<in<Result, Head>, quote_trait<id>, bind_back<quote<push_back>, Head>>,
                            Result>>
                {};
            }

            template<typename List>
            using unique = eval<meta_detail::unique_<List, list<>>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // replace
            namespace meta_detail
            {
                template<typename List, typename T, typename U>
                struct replace_
                {};

                template<typename...List, typename T, typename U>
                struct replace_<list<List...>, T, U>
                {
                    using type = list<if_<std::is_same<T, List>, U, List>...>;
                };
            }

            template<typename List, typename T, typename U>
            using replace = eval<meta_detail::replace_<List, T, U>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // replace_if
            namespace meta_detail
            {
                template<typename List, typename C, typename U>
                struct replace_if_
                {};

                template<typename...List, typename C, typename U>
                struct replace_if_<list<List...>, C, U>
                {
                    using type = list<if_<apply<C, List>, U, List>...>;
                };
            }

            template<typename List, typename C, typename U>
            using replace_if = eval<meta_detail::replace_if_<List, C, U>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // apply_list
            template<typename C, typename List>
            using apply_list = apply<uncurry<C>, List>;

            ////////////////////////////////////////////////////////////////////////////////////
            // foldl
            namespace meta_detail
            {
                template<typename List, typename State, typename Fun>
                struct foldl_
                {};

                template<typename State, typename Fun>
                struct foldl_<list<>, State, Fun>
                {
                    using type = State;
                };

                template<typename Head, typename ...List, typename State, typename Fun>
                struct foldl_<list<Head, List...>, State, Fun>
                  : foldl_<list<List...>, apply<Fun, State, Head>, Fun>
                {};
            }

            template<typename List, typename State, typename Fun>
            using foldl = eval<meta_detail::foldl_<List, State, Fun>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // foldr
            namespace meta_detail
            {
                template<typename List, typename State, typename Fun>
                struct foldr_
                {};

                template<typename State, typename Fun>
                struct foldr_<list<>, State, Fun>
                {
                    using type = State;
                };

                template<typename Head, typename ...List, typename State, typename Fun>
                struct foldr_<list<Head, List...>, State, Fun>
                {
                    using type = apply<Fun, eval<foldr_<list<List...>, State, Fun>>, Head>;
                };
            }

            template<typename List, typename State, typename Fun>
            using foldr = eval<meta_detail::foldr_<List, State, Fun>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // transform
            namespace meta_detail
            {
                template<typename List, typename Fun, typename = void>
                struct transform_
                {};

                template<typename ...List, typename Fun>
                struct transform_<list<List...>, Fun, void>
                {
                    using type = list<apply<Fun, List>...>;
                };

                template<typename ...List0, typename ...List1, typename Fun>
                struct transform_<list<List0...>, list<List1...>, Fun>
                {
                    using type = list<apply<Fun, List0, List1>...>;
                };
            }

            template<typename List, typename Fun, typename Dummy = void>
            using transform = eval<meta_detail::transform_<List, Fun, Dummy>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // zip_with
            template<typename Fun, typename ListOfLists>
            using zip_with =
                transform<
                    foldl<
                        ListOfLists,
                        repeat_n<size<front<ListOfLists>>, Fun>,
                        bind_back<quote<transform>, quote<bind_front> > >,
                    quote<apply> >;

            ////////////////////////////////////////////////////////////////////////////////////
            // zip
            template<typename ListOfLists>
            using zip = zip_with<quote<list>, ListOfLists>;

            ////////////////////////////////////////////////////////////////////////////////////
            // as_list
            namespace meta_detail
            {
                template<typename T>
                using uncvref_t = eval<std::remove_cv<eval<std::remove_reference<T>>>>;

                // Indirection here needed to avoid Core issue 1430
                // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
                template<typename Sequence>
                struct as_list_
                {
                    using type = apply<uncurry<curry<quote_trait<id>>>, uncvref_t<Sequence>>;
                };
            }

            template<typename Sequence>
            using as_list = eval<meta_detail::as_list_<Sequence>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // reverse
            template<typename List>
            using reverse = foldr<List, list<>, quote<push_back>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // all_of
            template<typename List, typename F>
            using all_of = bool_<0 == find_if<List, compose<quote<not_>, F>>::size()>;

            ////////////////////////////////////////////////////////////////////////////////////
            // any_of
            template<typename List, typename F>
            using any_of = bool_<0 != find_if<List, F>::size()>;

            ////////////////////////////////////////////////////////////////////////////////////
            // none_of
            template<typename List, typename F>
            using none_of = bool_<0 == find_if<List, F>::size()>;

            ////////////////////////////////////////////////////////////////////////////////////
            // add_const_if
            template<typename If>
            using add_const_if = if_<If, quote_trait<std::add_const>, quote_trait<id>>;

            template<bool If>
            using add_const_if_c = if_c<If, quote_trait<std::add_const>, quote_trait<id>>;
        }
    }
}

#endif
