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
            /// \addtogroup group-meta
            /// @{

            ////////////////////////////////////////////////////////////////////////////////////
            // General meta-programming utilities
            template<typename...Ts>
            struct list;

            /// \brief "Evaluate" a metafunction by returning the nested \c T::type alias.
            template<typename T>
            using eval = typename T::type;

            /// \brief Evaluate the Metafunction Class `F` with the arguments \c Args.
            template<typename F, typename...Args>
            using apply = typename F::template apply<Args...>;

            /// \brief A metafunction that evaluates the Metafunction Class `F` with
            /// the arguments \c Args.
            template<typename F, typename...Args>
            struct lazy_apply
            {
                using type = apply<F, Args...>;
            };

            /// \brief An integral constant wrapper for \c std::size_t.
            template<std::size_t N>
            using size_t = std::integral_constant<std::size_t, N>;

            /// \brief An integral constant wrapper for \c bool.
            template<bool B>
            using bool_ = std::integral_constant<bool, B>;

            /// \brief A metafunction that always returns type type \c T.
            template<typename T>
            struct id
            {
                using type = T;
            };

            /// \brief A metafunction that is type \c T.
            template<typename T>
            using id_t = eval<id<T>>;

            /// \brief Turn a class template or alias template into a
            /// Metafunction Class.
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

            /// \brief Turn a metafunction into a Metafunction Class.
            template<template<typename...> class C>
            struct quote_trait
            {
                template<typename...Ts>
                using apply = eval<apply<quote<C>, Ts...> >;
            };

            /// \brief Turn a class template or alias template into a
            /// Metafunction Class.
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

            /// \brief Turn a metafunction into a Metafunction Class.
            template<typename T, template<T...> class C>
            struct quote_trait_i
            {
                template<typename...Ts>
                using apply = eval<apply<quote_i<T, C>, Ts...> >;
            };

            /// \brief Compose the Metafunction Classes in the parameter pack
            /// \c Ts.
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

            /// \brief A Metafunction Class that always returns \c T.
            template<typename T>
            struct always
            {
                template<typename...>
                using apply = T;
            };

            /// \brief A Metafunction Class partially applies the Metafunction
            /// Class `F` by binding the arguments \c Ts to the <i>front</i>
            /// of \c F.
            template<typename F, typename...Ts>
            struct bind_front
            {
                template<typename...Us>
                using apply = apply<F, Ts..., Us...>;
            };

            /// \brief A Metafunction Class partially applies the Metafunction
            /// Class `F` by binding the arguments \c Ts to the <i>back</i>
            /// of \c F.
            template<typename F, typename...Us>
            struct bind_back
            {
                template<typename...Ts>
                using apply = apply<F, Ts..., Us...>;
            };

            /// \brief A metafunction that unpacks the types in the type list
            /// \c List into the Metafunction Class \c F.
            template<typename F, typename List>
            struct lazy_apply_list
            {};

            template<typename F, template<typename...> class T, typename ...Ts>
            struct lazy_apply_list<F, T<Ts...>>
            {
                using type = apply<F, Ts...>;
            };

            template<typename F, typename T, T...Is>
            struct lazy_apply_list<F, integer_sequence<T, Is...>>
            {
                using type = apply<F, std::integral_constant<T, Is>...>;
            };

            /// \brief Applies the Metafunction Class `F` using the types in
            /// the type list \c List as arguments.
            template<typename C, typename List>
            using apply_list = eval<lazy_apply_list<C, List>>;

            /// \brief A Metafunction Class that takes a bunch of arguments,
            /// bundles them into a type list, and then calls the Metafunction
            /// Class `F` with the type list.
            template<typename F, typename Q = quote<list>>
            struct curry
              : compose<F, Q>
            {};

            /// \brief A Metafunction Class that takes a type list,
            /// unpacks the types, and then calls the Metafunction
            /// Class `F` with types.
            template<typename F>
            struct uncurry
            {
                template<typename T>
                using apply = eval<lazy_apply_list<F, T>>;
            };

            /// \brief A Metafunction Class that reverses the order of the first
            /// two arguments.
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
            /// @}

            ////////////////////////////////////////////////////////////////////////////////////
            // if_
            /// \cond
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
            /// \endcond

            /// \brief Select one type or another depending on a compile-time Boolean.
            /// \ingroup group-meta
            template<typename If, typename Then, typename Else>
            using if_ = eval<meta_detail::_if_<If::value, Then, Else>>;

            /// \brief Select one type or another depending on a compile-time Boolean.
            /// \ingroup group-meta
            template<bool If, typename Then, typename Else>
            using if_c = eval<meta_detail::_if_<If, Then, Else>>;

            /// \cond
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
            /// \endcond

            /// \addtogroup group-meta
            /// @{

            /// \brief Logically and together all the Boolean parameters
            template<bool ...Bools>
            using and_c =
                decltype(meta_detail::fast_and_impl_(if_c<Bools, int*, int>{}...));

            /// \brief Logically or together all the Boolean parameters
            template<bool ...Bools>
            using or_c =
                decltype(meta_detail::fast_or_impl_(if_c<Bools, int, int*>{}...));

            /// \brief Logically negate the Boolean parameter
            template<bool Bool>
            using not_c = bool_<!Bool>;

            /// \brief Logically and together all the integral constant-wrapped Boolean
            /// parameters, <i>without</i> doing short-circuiting.
            template<typename...Bools>
            using fast_and = and_c<Bools::value...>;

            /// \brief Logically or together all the integral constant-wrapped Boolean
            /// parameters, <i>without</i> doing short-circuiting.
            template<typename...Bools>
            using fast_or = or_c<Bools::value...>;

            /// \brief Logically negate the integral constant-wrapped Boolean
            /// parameter.
            template<typename Bool>
            using not_ = not_c<Bool::value>;

            /// \brief Logically and together all the integral constant-wrapped Boolean
            /// parameters, with short-circuiting.
            template<typename...Bools>
            using and_ = eval<meta_detail::_and_<Bools...>>;

            /// \brief Logically or together all the integral constant-wrapped Boolean
            /// parameters, with short-circuiting.
            template<typename...Bools>
            using or_ = eval<meta_detail::_or_<Bools...>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // list
            /// \brief A list of types
            template<typename...Ts>
            struct list
            {
                using type = list;
                /// \return `sizeof...(Ts)`
                static constexpr std::size_t size() noexcept
                {
                    return sizeof...(Ts);
                }
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // size
            /// \brief An integral constant wrapper that is the size of the \c meta::list
            /// \c List
            template<typename List>
            using size = meta::size_t<List::size()>;

            /// @}

            ////////////////////////////////////////////////////////////////////////////////////
            // list_cat
            /// \cond
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
            /// \endcond

            /// \brief An integral constant wrapper that is the size of the \c meta::list
            /// \c List
            /// \ingroup group-meta
            template<typename ListOfLists>
            using list_cat = eval<meta_detail::list_cat_<ListOfLists>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // repeat_n
            /// \cond
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
            /// \endcond

            /// Generate `list<T,T,T...T>` of size `N` arguments
            /// in O(log N)
            /// \ingroup group-meta
            template<typename N, typename T = void>
            using repeat_n = eval<meta_detail::repeat_n_c_<N::value, T>>;

            /// Generate `list<T,T,T...T>` of size `N` arguments
            /// in O(log N)
            /// \ingroup group-meta
            template<std::size_t N, typename T = void>
            using repeat_n_c = eval<meta_detail::repeat_n_c_<N, T>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // list_element
            /// \cond
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
            /// \endcond

            ////////////////////////////////////////////////////////////////////////////////////
            // list_element
            /// \brief Return the `N`th element in the \c meta::list \c List in
            /// amortized O(1)
            /// \ingroup group-meta
            template<typename N, typename List>
            using list_element = eval<meta_detail::list_element_<N, List>>;

            /// \brief Return the `N`th element in the \c meta::list \c List in
            /// amortized O(1)
            /// \ingroup group-meta
            template<std::size_t N, typename List>
            using list_element_c = list_element<meta::size_t<N>, List>;

            ////////////////////////////////////////////////////////////////////////////////////
            // drop
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list by removing the first \c N
            /// element from \c List. Complexity: O(1)
            /// \ingroup group-meta
            template<typename N, typename List>
            using drop = eval<meta_detail::drop_<N, List>>;

            /// \brief Return a new \c meta::list by removing the first \c N
            /// element from \c List. Complexity: O(1)
            /// \ingroup group-meta
            template<std::size_t N, typename List>
            using drop_c = eval<meta_detail::drop_<meta::size_t<N>, List>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // front
            /// \cond
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
            /// \endcond

            /// \brief Return the first element in \c meta::list \c List.
            /// Complexity: O(1)
            /// \ingroup group-meta
            template<typename List>
            using front = eval<meta_detail::front_<List>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // back
            /// \cond
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
            /// \endcond

            /// \brief Return the last element in \c meta::list \c List.
            /// Complexity: Amortized O(1)
            /// \ingroup group-meta
            template<typename List>
            using back = eval<meta_detail::back_<List>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // push_front
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list by adding the element \c T
            /// to the front of \c List.
            /// Complexity: O(1)
            /// \ingroup group-meta
            template<typename List, typename T>
            using push_front = eval<meta_detail::push_front_<List, T>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // pop_front
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list by removing the first element
            /// from the front of \c List.
            /// Complexity: O(1)
            /// \ingroup group-meta
            template<typename List>
            using pop_front = eval<meta_detail::pop_front_<List>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // push_back
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list by adding the element \c T
            /// to the back of \c List.
            /// Complexity: O(1)
            /// \ingroup group-meta
            template<typename List, typename T>
            using push_back = eval<meta_detail::push_back_<List, T>>;

            // pop_back not provided because it cannot be made to meet the complexity
            // guarantees one would expect.

            ////////////////////////////////////////////////////////////////////////////////////
            // empty
            /// \brief An Boolean integral constant wrapper around \c true if
            /// \c List is an empty type list; \c false, otherwise.
            /// \ingroup group-meta
            template<typename List>
            using empty = bool_<0 == size<List>::value>;

            ////////////////////////////////////////////////////////////////////////////////////
            // find
            /// \cond
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
            /// \endcond

            /// \brief Return the tail of the list \c List starting at the first occurance
            /// of \c T, if any such element exists; the empty list, otherwise.
            /// \ingroup group-meta
            template<typename List, typename T>
            using find = eval<meta_detail::find_<List, T>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // find_if
            /// \cond
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
            /// \endcond

            /// \brief Return the tail of the list \c List starting at the first element
            /// `A` such that `apply<Fun, A>::%value` is \c true, if any such element
            /// exists; the empty list, otherwise.
            /// \ingroup group-meta
            template<typename List, typename Fun>
            using find_if = eval<meta_detail::find_if_<List, Fun>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // in
            /// \brief A Boolean integral constant wrapper around \c true if
            /// there is at least one occurance of `T` in \c List.
            /// \ingroup group-meta
            template<typename List, typename T>
            using in = not_<empty<find<List, T>>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // unique
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list where all duplicate elements
            /// have been removed. Complexity: O(N^2)
            /// \ingroup group-meta
            template<typename List>
            using unique = eval<meta_detail::unique_<List, list<>>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // replace
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list where all instances of type \c T
            /// have been replaced with \c U. Complexity: O(N)
            /// \ingroup group-meta
            template<typename List, typename T, typename U>
            using replace = eval<meta_detail::replace_<List, T, U>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // replace_if
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list where all elements \c A
            /// such that `apply<C, A>::%value` is \c true have been
            /// replaced with \c U. Complexity: O(N)
            /// \ingroup group-meta
            template<typename List, typename C, typename U>
            using replace_if = eval<meta_detail::replace_if_<List, C, U>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // foldl
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list constructed by doing a left fold
            /// of the list \c List using binary Metafunction Class \c Fun and
            /// initial state \c State. Complexity: O(N)
            /// \ingroup group-meta
            template<typename List, typename State, typename Fun>
            using foldl = eval<meta_detail::foldl_<List, State, Fun>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // foldr
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list constructed by doing a right fold
            /// of the list \c List using binary Metafunction Class \c Fun and
            /// initial state \c State. Complexity: O(N)
            /// \ingroup group-meta
            template<typename List, typename State, typename Fun>
            using foldr = eval<meta_detail::foldr_<List, State, Fun>>;
            /// @}

            ////////////////////////////////////////////////////////////////////////////////////
            // transform
            /// \cond
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
            /// \endcond

            /// \brief Return a new \c meta::list constructed by transforming all
            /// the elements in \c List with the unary Metafuncion Class Fun.
            /// \c transform can also be called with two lists of the same length
            /// and a binary Metafunction Class, in which case it returns a new list
            /// constructed with the results of calling \c Fun with each element in the
            /// lists, pairwise. Complexity: O(N)
            /// \ingroup group-meta
            template<typename List, typename Fun, typename Dummy = void>
            using transform = eval<meta_detail::transform_<List, Fun, Dummy>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // zip_with
            /// \brief Given a list of lists of types and a Metafunction Class \c Fun,
            /// construct a new list by calling \c Fun with the elements from the lists
            /// pairwise. Complexity: O(N*M), where `N` is the size of the outer list, and
            /// M is the size of the inner lists.
            /// \ingroup group-meta
            template<typename Fun, typename ListOfLists>
            using zip_with =
                transform<
                    foldl<
                        ListOfLists,
                        repeat_n<size<front<ListOfLists>>, Fun>,
                        bind_back<quote<transform>, quote<bind_front>>>,
                    quote<apply>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // zip
            /// \brief Given a list of lists of types, construct a new list by
            /// grouping the elements from the lists pairwise into
            /// `meta::list`s. Complexity: O(N*M), where `N` is the size of
            /// the outer list, and `M` is the size of the inner lists.
            /// \ingroup group-meta
            template<typename ListOfLists>
            using zip = zip_with<quote<list>, ListOfLists>;

            ////////////////////////////////////////////////////////////////////////////////////
            // as_list
            /// \cond
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
            /// \endcond

            /// \addtogroup group-meta
            /// @{

            /// \brief Turn a type into an instance of \c meta::list in a way
            /// determined by \c meta::lazy_apply_list.
            template<typename Sequence>
            using as_list = eval<meta_detail::as_list_<Sequence>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // reverse
            /// \brief Return a new \c meta::list by reversing the elements in the
            /// list \c List. Complexity: O(N)
            template<typename List>
            using reverse = foldr<List, list<>, quote<push_back>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // all_of
            /// \brief A Boolean integral constant wrapper around \c true if
            /// `apply<F, A>::%value` is \c true for all elements \c A
            /// in \c meta::list \c List; \c false, otherwise. Complexity: O(N)
            template<typename List, typename F>
            using all_of = empty<find_if<List, compose<quote<not_>, F>>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // any_of
            /// \brief A Boolean integral constant wrapper around \c true if
            /// `apply<F, A>::%value` is \c true for any element \c A
            /// in \c meta::list \c List; \c false, otherwise. Complexity: O(N)
            template<typename List, typename F>
            using any_of = not_<empty<find_if<List, F>>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // none_of
            /// \brief A Boolean integral constant wrapper around \c true if
            /// `apply<F, A>::%value` is \c false for alls elements \c A
            /// in \c meta::list \c List; \c false, otherwise. Complexity: O(N)
            template<typename List, typename F>
            using none_of = empty<find_if<List, F>>;

            /// \cond
            ////////////////////////////////////////////////////////////////////////////////////
            // add_const_if
            template<typename If>
            using add_const_if = if_<If, quote_trait<std::add_const>, quote_trait<id>>;

            template<bool If>
            using add_const_if_c = if_c<If, quote_trait<std::add_const>, quote_trait<id>>;
            /// \endcond

            ////////////////////////////////////////////////////////////////////////////////////
            // Math operations
            /// \brief An integral constant wrapper around the result of adding the
            /// two wrapped integers \c T::value and \c U::value.
            template<typename T, typename U>
            using plus = std::integral_constant<decltype(T::value + U::value), T::value + U::value>;

            /// \brief An integral constant wrapper around the result of subtracting the
            /// two wrapped integers \c T::value and \c U::value.
            template<typename T, typename U>
            using minus = std::integral_constant<decltype(T::value - U::value), T::value - U::value>;

            /// \brief An integral constant wrapper around the result of multiplying the
            /// two wrapped integers \c T::value and \c U::value.
            template<typename T, typename U>
            using multiplies = std::integral_constant<decltype(T::value * U::value), T::value * U::value>;

            /// \brief An integral constant wrapper around the result of dividing the
            /// two wrapped integers \c T::value and \c U::value.
            template<typename T, typename U>
            using divides = std::integral_constant<decltype(T::value / U::value), T::value / U::value>;

            /// \brief An integral constant wrapper around the remainder of dividing the
            /// two wrapped integers \c T::value and \c U::value.
            template<typename T>
            using negate = std::integral_constant<decltype(-T::value), -T::value>;

            /// \brief An integral constant wrapper around the remainder of dividing the
            /// two wrapped integers \c T::value and \c U::value.
            template<typename T, typename U>
            using modulus = std::integral_constant<decltype(T::value % U::value), T::value % U::value>;

            /// \brief A Boolean integral constant wrapper around the result of comparing
            /// \c T::value and \c U::value for equality.
            template<typename T, typename U>
            using equal_to = bool_<T::value == U::value>;

            /// \brief A Boolean integral constant wrapper around the result of comparing
            /// \c T::value and \c U::value for inequality.
            template<typename T, typename U>
            using not_equal_to = bool_<T::value != U::value>;

            /// \brief A Boolean integral constant wrapper around \c true if
            /// \c T::value is greater than \c U::value; \c false, otherwise.
            template<typename T, typename U>
            using greater = bool_<(T::value > U::value)>;

            /// \brief A Boolean integral constant wrapper around \c true if
            /// \c T::value is less than \c U::value; \c false, otherwise.
            template<typename T, typename U>
            using less = bool_<(T::value < U::value)>;

            /// \brief A Boolean integral constant wrapper around \c true if
            /// \c T::value is greater than or equal to \c U::value; \c false,
            /// otherwise.
            template<typename T, typename U>
            using greater_equal = bool_<(T::value >= U::value)>;

            /// \brief A Boolean integral constant wrapper around \c true if
            /// \c T::value is less than or equal to \c U::value; \c false,
            /// otherwise.
            template<typename T, typename U>
            using less_equal = bool_<(T::value <= U::value)>;

            /// \brief An integral constant wrapper around the result of bitwise-and'ing
            /// the two wrapped integers \c T::value and \c U::value.
            template<typename T, typename U>
            using bit_and = std::integral_constant<decltype(T::value & U::value), T::value & U::value>;

            /// \brief An integral constant wrapper around the result of bitwise-or'ing
            /// the two wrapped integers \c T::value and \c U::value.
            template<typename T, typename U>
            using bit_or = std::integral_constant<decltype(T::value | U::value), T::value | U::value>;

            /// \brief An integral constant wrapper around the result of
            /// bitwise-exclusive-or'ing the two wrapped integers \c T::value and
            /// \c U::value.
            template<typename T, typename U>
            using bit_xor = std::integral_constant<decltype(T::value ^ U::value), T::value ^ U::value>;

            /// \brief An integral constant wrapper around the result of
            /// bitwise-complimenting the wrapped integer \c T::value.
            template<typename T>
            using bit_not = std::integral_constant<decltype(~T::value), ~T::value>;
            /// @}
        }
    }
}

#endif
