/// \file
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
            template<template<typename...> class C, typename...Ts>
            struct defer;

            /// \addtogroup group-meta
            /// @{

            ////////////////////////////////////////////////////////////////////////////////////
            // General meta-programming utilities
            template<typename...Ts>
            struct list;

            /// An empty type used for various things.
            struct nil_
            {};
            #ifndef nil
            using nil = nil_;
            #endif

            /// "Evaluate" the metafunction \p T by returning the nested \c T::type alias.
            template<typename T>
            using eval = typename T::type;

            /// Evaluate the Metafunction Class \p F with the arguments \p Args.
            template<typename F, typename...Args>
            using apply = typename F::template apply<Args...>;

            /// A Metafunction Class that always returns \p T.
            template<typename T>
            struct always
            {
            private:
                // Redirect through a class template for compilers that have not
                // yet implemented CWG 1558:
                // <http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1558>
                template<typename...>
                struct impl
                {
                    using type = T;
                };
            public:
                template<typename...Ts>
                using apply = eval<impl<Ts...>>;
            };

            /// An alias for `void`.
            template<typename...Ts>
            using void_ = apply<always<void>, Ts...>;

            namespace lazy
            {
                template<typename T>
                using always = defer<always, T>;
            }

            /// \cond
            namespace meta_detail
            {
                template<typename, typename = void>
                struct has_type_
                {
                    using type = std::false_type;
                };

                template<typename T>
                struct has_type_<T, void_<typename T::type>>
                {
                    using type = std::true_type;
                };

                template<typename, typename, typename = void>
                struct lazy_apply_
                {};

                template<typename F, typename...Ts>
                struct lazy_apply_<F, list<Ts...>, void_<apply<F, Ts...>>>
                {
                    using type = apply<F, Ts...>;
                };
            }
            /// \endcond

            /// An alias for `std::true_type` if `T::type` exists and names a type;
            ///        otherwise, it's an alias for `std::false_type`.
            template<typename T>
            using has_type = eval<meta_detail::has_type_<T>>;

            /// A metafunction that evaluates the Metafunction Class `F` with
            /// the arguments \c Args.
            template<typename F, typename...Args>
            struct lazy_apply
              : meta_detail::lazy_apply_<F, list<Args...>>
            {};

            /// An integral constant wrapper for \c std::size_t.
            template<std::size_t N>
            using size_t = std::integral_constant<std::size_t, N>;

            /// An integral constant wrapper for \c bool.
            template<bool B>
            using bool_ = std::integral_constant<bool, B>;

            /// A metafunction that always returns its argument \p T.
            template<typename T>
            struct id
            {
                using type = T;
            };

            /// A metafunction that is type \c T.
            template<typename T>
            using id_t = eval<id<T>>;

            namespace lazy
            {
                template<typename T>
                using id = defer<id, T>;
            }

            /// Turn a class template or alias template \p C into a
            /// Metafunction Class.
            template<template<typename...> class C>
            struct quote
            {
            private:
                template<typename, typename = quote, typename = void>
                struct impl
                {};
                template<typename...Ts, template<typename...> class D>
                struct impl<list<Ts...>, quote<D>, void_<D<Ts...>>>
                {
                    using type = D<Ts...>;
                };
            public:
                // Indirection here needed to avoid Core issue 1430
                // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
                template<typename...Ts>
                using apply = eval<impl<list<Ts...>>>;
            };

            /// Turn a class template or alias template \p F taking literals of
            /// type \p T into a Metafunction Class.
            template<typename T, template<T...> class F>
            struct quote_i
            {
            private:
                template<typename, typename = quote_i, typename = void>
                struct impl
                {};
                template<typename...Ts, typename U, template<U...> class D>
                struct impl<list<Ts...>, quote_i<U, D>, void_<D<Ts::type::value...>>>
                {
                    using type = D<Ts::type::value...>;
                };
            public:
                // Indirection here needed to avoid Core issue 1430
                // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
                template<typename...Ts>
                using apply = eval<impl<list<Ts...>>>;
            };

            /// Turn a metafunction \p C into a Metafunction Class.
            template<template<typename...> class C>
            struct quote_trait
            {
                template<typename...Ts>
                using apply = eval<apply<quote<C>, Ts...>>;
            };

            /// Turn a metafunction \p C taking literals of type \p T
            /// into a Metafunction Class.
            template<typename T, template<T...> class C>
            struct quote_trait_i
            {
                template<typename...Ts>
                using apply = eval<apply<quote_i<T, C>, Ts...>>;
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // defer
            /// A wrapper that defers the instantiation of a template in a \c lambda
            /// expression.
            ///
            /// In the code below, the lambda would ideally be written as
            /// `lambda<_a,_b,push_back<_a,_b>>`, however this fails since `push_back`
            /// expects its first argument to be a list, not a placeholder. Instead,
            /// we express it using \c defer as follows:
            ///
            /// \code
            /// template<typename List>
            /// using reverse = foldr<List, list<>, lambda<_a, _b, defer<push_back, _a, _b> > >;
            /// \endcode
            template<template<typename...> class C, typename...Ts>
            struct defer
              : lazy_apply<quote<C>, Ts...>
            {};

            /// Compose the Metafunction Classes \p Fs in the parameter pack
            /// \p Ts.
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

            namespace lazy
            {
                template<typename...Fns>
                using compose = defer<compose, Fns...>;
            }

            /// A Metafunction Class that partially applies the Metafunction
            /// Class \p F by binding the arguments \p Ts to the \e front
            /// of \p F.
            template<typename F, typename...Ts>
            struct bind_front
            {
                template<typename...Us>
                using apply = apply<F, Ts..., Us...>;
            };

            /// A Metafunction Class partially applies the Metafunction
            /// Class \p F by binding the arguments \p Ts to the \e back
            /// of \p F.
            template<typename F, typename...Us>
            struct bind_back
            {
                template<typename...Ts>
                using apply = apply<F, Ts..., Us...>;
            };

            namespace lazy
            {
                template<typename Fn, typename...Ts>
                using bind_front = defer<bind_front, Fn, Ts...>;

                template<typename Fn, typename...Ts>
                using bind_back = defer<bind_back, Fn, Ts...>;
            }

            /// A metafunction that unpacks the types in the type list
            /// \p List into the Metafunction Class \p F.
            template<typename F, typename List>
            struct lazy_apply_list
            {};

            template<typename F, template<typename...> class T, typename ...Ts>
            struct lazy_apply_list<F, T<Ts...>>
              : lazy_apply<F, Ts...>
            {};

            template<typename F, typename T, T...Is>
            struct lazy_apply_list<F, integer_sequence<T, Is...>>
              : lazy_apply<F, std::integral_constant<T, Is>...>
            {};

            /// Applies the Metafunction Class \p C using the types in
            /// the type list \p List as arguments.
            template<typename C, typename List>
            using apply_list = eval<lazy_apply_list<C, List>>;

            namespace lazy
            {
                template<typename F, typename List>
                using lazy_apply_list = defer<lazy_apply_list, F, List>;

                template<typename F, typename List>
                using apply_list = defer<apply_list, F, List>;
            }

            /// A Metafunction Class that takes a bunch of arguments,
            /// bundles them into a type list, and then calls the Metafunction
            /// Class \p F with the type list \p Q.
            template<typename F, typename Q = quote<list>>
            using curry = compose<F, Q>;

            /// A Metafunction Class that takes a type list,
            /// unpacks the types, and then calls the Metafunction
            /// Class \p F with the types.
            template<typename F>
            using uncurry = bind_front<quote<apply_list>, F>;

            namespace lazy
            {
                template<typename F, typename Q = quote<list>>
                using curry = defer<curry, F, Q>;

                template<typename F>
                using uncurry = defer<uncurry, F>;
            }

            /// A Metafunction Class that reverses the order of the first
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
                  : lazy_apply<F, B, A, Ts...>
                {};
            public:
                template<typename ...Ts>
                using apply = eval<impl<Ts...>>;
            };
            /// @}

            namespace lazy
            {
                template<typename...Ts>
                using flip = defer<flip, Ts...>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // if_
            /// \cond
            namespace meta_detail
            {
                template<typename...>
                struct _if_
                {};

                template<typename If, typename Then>
                struct _if_<If, Then>
                  : std::enable_if<If::type::value, Then>
                {};

                template<typename If, typename Then, typename Else>
                struct _if_<If, Then, Else>
                  : std::conditional<If::type::value, Then, Else>
                {};
            }
            /// \endcond

            /// Select one type or another depending on a compile-time Boolean.
            /// \ingroup group-meta
            template<typename...Args>
            using if_ = eval<meta_detail::_if_<Args...>>;

            /// Select one type or another depending on a compile-time Boolean.
            /// \ingroup group-meta
            template<bool If, typename...Args>
            using if_c = eval<meta_detail::_if_<bool_<If>, Args...>>;

            namespace lazy
            {
                template<typename...Args>
                using if_ = defer<if_, Args...>;
            }

            /// \cond
            namespace meta_detail
            {
                template<typename ...Bools>
                struct _and_;

                template<>
                struct _and_<>
                  : std::true_type
                {};

                template<typename Bool, typename...Bools>
                struct _and_<Bool, Bools...>
                  : if_c<!Bool::type::value, std::false_type, _and_<Bools...>>
                {};

                template<typename ...Bools>
                struct _or_;

                template<>
                struct _or_<>
                  : std::false_type
                {};

                template<typename Bool, typename...Bools>
                struct _or_<Bool, Bools...>
                  : if_c<Bool::type::value, std::true_type, _or_<Bools...>>
                {};
            }
            /// \endcond

            /// \addtogroup group-meta
            /// @{

            /// Logically negate the Boolean parameter
            template<bool Bool>
            using not_c = bool_<!Bool>;

            /// Logically negate the integral constant-wrapped Boolean
            /// parameter.
            template<typename Bool>
            using not_ = not_c<Bool::type::value>;

            /// Logically and together all the Boolean parameters
            template<bool ...Bools>
            using and_c =
                std::is_same<
                    integer_sequence<bool, Bools...>,
                    integer_sequence<bool, (Bools || true)...>>;

            /// Logically and together all the integral constant-wrapped Boolean
            /// parameters, <i>without</i> doing short-circuiting.
            template<typename...Bools>
            using fast_and = and_c<Bools::type::value...>;

            /// Logically and together all the integral constant-wrapped Boolean
            /// parameters, with short-circuiting.
            template<typename...Bools>
            using and_ = eval<meta_detail::_and_<Bools...>>;

            /// Logically or together all the Boolean parameters
            template<bool ...Bools>
            using or_c = not_<
                std::is_same<
                    integer_sequence<bool, Bools...>,
                    integer_sequence<bool, (Bools && false)...>>>;

            /// Logically or together all the integral constant-wrapped Boolean
            /// parameters, <i>without</i> doing short-circuiting.
            template<typename...Bools>
            using fast_or = or_c<Bools::type::value...>;

            /// Logically or together all the integral constant-wrapped Boolean
            /// parameters, with short-circuiting.
            template<typename...Bools>
            using or_ = eval<meta_detail::_or_<Bools...>>;

            namespace lazy
            {
                template<typename...Bools>
                using and_ = defer<and_, Bools...>;

                template<typename...Bools>
                using or_ = defer<or_, Bools...>;

                template<typename Bool>
                using not_ = defer<not_, Bool>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // list
            /// A list of types
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
            /// An integral constant wrapper that is the size of the \c meta::list
            /// \p List.
            template<typename List>
            using size = meta::size_t<List::size()>;

            namespace lazy
            {
                template<typename List>
                using size = defer<size, List>;
            }
            /// @}

            ////////////////////////////////////////////////////////////////////////////////////
            // concat
            /// \cond
            namespace meta_detail
            {
                template<typename...Lists>
                struct concat_
                {};

                template<>
                struct concat_<>
                {
                    using type = list<>;
                };

                template<typename...List1>
                struct concat_<list<List1...>>
                {
                    using type = list<List1...>;
                };

                template<typename ...List1, typename ...List2>
                struct concat_<list<List1...>, list<List2...>>
                {
                    using type = list<List1..., List2...>;
                };

                template<typename ...List1, typename ...List2, typename...List3>
                struct concat_<list<List1...>, list<List2...>, list<List3...>>
                {
                    using type = list<List1..., List2..., List3...>;
                };

                template<typename ...List1, typename ...List2, typename...List3, typename...Rest>
                struct concat_<list<List1...>, list<List2...>, list<List3...>, Rest...>
                  : concat_<list<List1..., List2..., List3...>, Rest...>
                {};
            }
            /// \endcond

            /// Concatenates several lists into a single list.
            /// \pre The parameters must all be instantiations of \c meta::list.
            /// \par Complexity
            /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of
            /// lists.
            /// \ingroup group-meta
            template<typename...Lists>
            using concat = eval<meta_detail::concat_<Lists...>>;

            namespace lazy
            {
                template<typename...Lists>
                using concat = defer<concat, Lists...>;
            }

            /// Joins a list of lists into a single list.
            /// \pre The parameter must be an instantiation of \c meta::list\<T...\>
            ///     where each \c T is itself an instantiation of \c meta::list.
            /// \par Complexity
            /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of
            /// lists.
            /// \ingroup group-meta
            template<typename ListOfLists>
            using join = apply_list<quote<concat>, ListOfLists>;

            namespace lazy
            {
                template<typename ListOfLists>
                using join = defer<join, ListOfLists>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // repeat_n
            /// \cond
            namespace meta_detail
            {
                template<std::size_t N, typename T>
                struct repeat_n_c_
                {
                    using type = concat<
                        eval<repeat_n_c_<N / 2, T>>,
                        eval<repeat_n_c_<N / 2, T>>,
                        eval<repeat_n_c_<N % 2, T>>>;
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
            /// \par Complexity
            /// \f$ O(log N) \f$.
            /// \ingroup group-meta
            template<typename N, typename T = void>
            using repeat_n = eval<meta_detail::repeat_n_c_<N::type::value, T>>;

            /// Generate `list<T,T,T...T>` of size `N` arguments
            /// \par Complexity
            /// \f$ O(log N) \f$.
            /// \ingroup group-meta
            template<std::size_t N, typename T = void>
            using repeat_n_c = eval<meta_detail::repeat_n_c_<N, T>>;

            namespace lazy
            {
                template<typename N, typename T = void>
                using repeat_n = defer<repeat_n, N, T>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // list_element
            /// \cond
            namespace meta_detail
            {
                template<typename VoidPtrs>
                struct list_element_impl_;

                template<typename ...VoidPtrs>
                struct list_element_impl_<list<VoidPtrs...>>
                {
                    static nil_ eval(...);

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
            /// Return the \p N th element in the \c meta::list \p List.
            /// \par Complexity
            /// Amortized \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename N, typename List>
            using list_element = eval<meta_detail::list_element_<N, List>>;

            /// Return the `N`th element in the \c meta::list \c List.
            /// \par Complexity
            /// Amortized \f$ O(1) \f$.
            /// \ingroup group-meta
            template<std::size_t N, typename List>
            using list_element_c = list_element<meta::size_t<N>, List>;

            namespace lazy
            {
                template<typename N, typename List>
                using list_element = defer<list_element, N, List>;
            }

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
                    static nil_ eval(...);
                };

                template<typename ...VoidPtrs>
                struct drop_impl_<list<VoidPtrs...>>
                {
                    static nil_ eval(...);

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

            /// Return a new \c meta::list by removing the first \p N
            /// elements from \p List.
            /// \par Complexity
            /// \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename N, typename List>
            using drop = eval<meta_detail::drop_<N, List>>;

            /// Return a new \c meta::list by removing the first \p N
            /// elements from \p List.
            /// \par Complexity
            /// \f$ O(1) \f$.
            /// \ingroup group-meta
            template<std::size_t N, typename List>
            using drop_c = eval<meta_detail::drop_<meta::size_t<N>, List>>;

            namespace lazy
            {
                template<typename N, typename List>
                using drop = defer<drop, N, List>;
            }

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

            /// Return the first element in \c meta::list \p List.
            /// \par Complexity
            /// \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename List>
            using front = eval<meta_detail::front_<List>>;

            namespace lazy
            {
                template<typename List>
                using front = defer<front, List>;
            }

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

            /// Return the last element in \c meta::list \p List.
            /// \par Complexity
            /// Amortized \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename List>
            using back = eval<meta_detail::back_<List>>;

            namespace lazy
            {
                template<typename List>
                using back = defer<back, List>;
            }

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

            /// Return a new \c meta::list by adding the element \p T
            /// to the front of \p List.
            /// \par Complexity
            /// \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename List, typename T>
            using push_front = eval<meta_detail::push_front_<List, T>>;

            namespace lazy
            {
                template<typename List, typename T>
                using push_front = defer<push_front, List, T>;
            }

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

            /// Return a new \c meta::list by removing the first element
            /// from the front of \p List.
            /// \par Complexity
            /// \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename List>
            using pop_front = eval<meta_detail::pop_front_<List>>;

            namespace lazy
            {
                template<typename List>
                using pop_front = defer<pop_front, List>;
            }

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

            /// Return a new \c meta::list by adding the element \p T
            /// to the back of \p List.
            /// \par Complexity
            /// \f$ O(1) \f$.
            /// \note pop_back not provided because it cannot be made to meet the complexity
            /// guarantees one would expect.
            /// \ingroup group-meta
            template<typename List, typename T>
            using push_back = eval<meta_detail::push_back_<List, T>>;

            namespace lazy
            {
                template<typename List, typename T>
                using push_back = defer<push_back, List, T>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // empty
            /// An Boolean integral constant wrapper around \c true if
            /// \p List is an empty type list; \c false, otherwise.
            /// \ingroup group-meta
            template<typename List>
            using empty = bool_<0 == size<List>::type::value>;

            namespace lazy
            {
                template<typename List>
                using empty = defer<empty, List>;
            }

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

            /// Return the tail of the list \p List starting at the first occurrence
            /// of \p T, if any such element exists; the empty list, otherwise.
            /// \ingroup group-meta
            template<typename List, typename T>
            using find = eval<meta_detail::find_<List, T>>;

            namespace lazy
            {
                template<typename List, typename T>
                using find = defer<find, List, T>;
            }

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

            /// Return the tail of the list \p List starting at the first element
            /// `A` such that `apply<Fun, A>::%value` is \c true, if any such element
            /// exists; the empty list, otherwise.
            /// \ingroup group-meta
            template<typename List, typename Fun>
            using find_if = eval<meta_detail::find_if_<List, Fun>>;

            namespace lazy
            {
                template<typename List, typename Fun>
                using find_if = defer<find_if, List, Fun>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // in
            /// A Boolean integral constant wrapper around \c true if
            /// there is at least one occurrence of \p T in \p List.
            /// \ingroup group-meta
            template<typename List, typename T>
            using in = not_<empty<find<List, T>>>;

            namespace lazy
            {
                template<typename List, typename T>
                using in = defer<in, List, T>;
            }

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

            /// Return a new \c meta::list where all duplicate elements
            /// have been removed.
            /// \par Complexity
            /// \f$ O(N^2) \f$.
            /// \ingroup group-meta
            template<typename List>
            using unique = eval<meta_detail::unique_<List, list<>>>;

            namespace lazy
            {
                template<typename List>
                using unique = defer<unique, List>;
            }

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

            /// Return a new \c meta::list where all instances of type \p T
            /// have been replaced with \p U.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename T, typename U>
            using replace = eval<meta_detail::replace_<List, T, U>>;

            namespace lazy
            {
                template<typename List, typename T, typename U>
                using replace = defer<replace, T, U>;
            }

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

            /// Return a new \c meta::list where all elements \c A of the list \p List
            /// for which `apply<C,A>::%value` is \c true have been replaced with \p U.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename C, typename U>
            using replace_if = eval<meta_detail::replace_if_<List, C, U>>;

            namespace lazy
            {
                template<typename List, typename C, typename U>
                using replace_if = defer<replace_if, C, U>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // foldl
            /// \cond
            namespace meta_detail
            {
                template<typename, typename, typename, typename = void>
                struct foldl_
                {};

                template<typename State, typename Fun>
                struct foldl_<list<>, State, Fun>
                {
                    using type = State;
                };

                template<typename Head, typename ...List, typename State, typename Fun>
                struct foldl_<list<Head, List...>, State, Fun, void_<apply<Fun, State, Head>>>
                  : foldl_<list<List...>, apply<Fun, State, Head>, Fun>
                {};
            }
            /// \endcond

            /// Return a new \c meta::list constructed by doing a left fold
            /// of the list \p List using binary Metafunction Class \p Fun and
            /// initial state \p State.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename State, typename Fun>
            using foldl = eval<meta_detail::foldl_<List, State, Fun>>;

            /// An alias for `meta::foldl`.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename State, typename Fun>
            using accumulate = foldl<List, State, Fun>;

            namespace lazy
            {
                template<typename List, typename State, typename Fun>
                using foldl = defer<foldl, List, State, Fun>;

                template<typename List, typename State, typename Fun>
                using accumulate = defer<accumulate, List, State, Fun>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // foldr
            /// \cond
            namespace meta_detail
            {
                template<typename, typename, typename, typename = void>
                struct foldr_
                {};

                template<typename State, typename Fun>
                struct foldr_<list<>, State, Fun>
                {
                    using type = State;
                };

                template<typename Head, typename ...List, typename State, typename Fun>
                struct foldr_<list<Head, List...>, State, Fun, void_<eval<foldr_<list<List...>, State, Fun>>>>
                  : lazy_apply<Fun, eval<foldr_<list<List...>, State, Fun>>, Head>
                {};
            }
            /// \endcond

            /// Return a new \c meta::list constructed by doing a right fold
            /// of the list \p List using binary Metafunction Class \p Fun and
            /// initial state \p State.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename State, typename Fun>
            using foldr = eval<meta_detail::foldr_<List, State, Fun>>;

            namespace lazy
            {
                template<typename List, typename State, typename Fun>
                using foldr = defer<foldr, List, State, Fun>;
            }
            /// @}

            ////////////////////////////////////////////////////////////////////////////////////
            // transform
            /// \cond
            namespace meta_detail
            {
                template<typename, typename, typename = void>
                struct transform1_
                {};

                template<typename...List, typename Fun>
                struct transform1_<list<List...>, Fun, void_<list<apply<Fun, List>...>>>
                {
                    using type = list<apply<Fun, List>...>;
                };

                template<typename, typename, typename, typename = void>
                struct transform2_
                {};

                template<typename ...List0, typename ...List1, typename Fun>
                struct transform2_<list<List0...>, list<List1...>, Fun, void_<list<apply<Fun, List0, List1>...>>>
                {
                    using type = list<apply<Fun, List0, List1>...>;
                };

                template<typename...Args>
                struct transform_
                {};

                template<typename List, typename Fun>
                struct transform_<List, Fun>
                  : transform1_<List, Fun>
                {};

                template<typename List0, typename List1, typename Fun>
                struct transform_<List0, List1, Fun>
                  : transform2_<List0, List1, Fun>
                {};
            }
            /// \endcond

            /// Return a new \c meta::list constructed by transforming all
            /// the elements in \p List with the unary Metafuncion Class \p Fun.
            /// \c transform can also be called with two lists of the same length
            /// and a binary Metafunction Class, in which case it returns a new list
            /// constructed with the results of calling \p Fun with each element in the
            /// lists, pairwise.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename ...Args>
            using transform = eval<meta_detail::transform_<Args...>>;

            namespace lazy
            {
                template<typename...Args>
                using transform = defer<transform, Args...>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // zip_with
            /// Given a list of lists \p ListOfLists of types and a Metafunction Class \p Fun,
            /// construct a new list by calling \p Fun with the elements from the lists
            /// pairwise.
            /// \par Complexity
            /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
            /// \f$ M \f$ is the size of the inner lists.
            /// \ingroup group-meta
            template<typename Fun, typename ListOfLists>
            using zip_with =
                transform<
                    foldl<
                        ListOfLists,
                        repeat_n<size<front<ListOfLists>>, Fun>,
                        bind_back<quote<transform>, quote<bind_front>>>,
                    quote<apply>>;

            namespace lazy
            {
                template<typename Fun, typename ListOfLists>
                using zip_with = defer<zip_with, Fun, ListOfLists>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // zip
            /// Given a list of lists of types \p ListOfLists, construct a new list by
            /// grouping the elements from the lists pairwise into
            /// `meta::list`s.
            /// \par Complexity
            /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
            /// \f$ M \f$ is the size of the inner lists.
            /// \ingroup group-meta
            template<typename ListOfLists>
            using zip = zip_with<quote<list>, ListOfLists>;

            namespace lazy
            {
                template<typename ListOfLists>
                using zip = defer<zip, ListOfLists>;
            }

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
                  : lazy_apply<uncurry<curry<quote_trait<id>>>, uncvref_t<Sequence>>
                {};
            }
            /// \endcond

            /// \addtogroup group-meta
            /// @{

            /// Turn a type into an instance of \c meta::list in a way
            /// determined by \c meta::lazy_apply_list.
            template<typename Sequence>
            using as_list = eval<meta_detail::as_list_<Sequence>>;

            namespace lazy
            {
                template<typename Sequence>
                using as_list = defer<as_list, Sequence>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // reverse
            /// Return a new \c meta::list by reversing the elements in the
            /// list \p List.
            /// \par Complexity
            /// \f$ O(N) \f$.
            template<typename List>
            using reverse = foldr<List, list<>, quote<push_back>>;

            namespace lazy
            {
                template<typename List>
                using reverse = defer<reverse, List>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // all_of
            /// A Boolean integral constant wrapper around \c true if
            /// `apply<F, A>::%value` is \c true for all elements \c A
            /// in \c meta::list \p List; \c false, otherwise.
            /// \par Complexity
            /// \f$ O(N) \f$.
            template<typename List, typename F>
            using all_of = empty<find_if<List, compose<quote<not_>, F>>>;

            namespace lazy
            {
                template<typename List, typename Fn>
                using all_of = defer<all_of, List, Fn>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // any_of
            /// A Boolean integral constant wrapper around \c true if
            /// `apply<F, A>::%value` is \c true for any element \c A
            /// in \c meta::list \p List; \c false, otherwise.
            /// \par Complexity
            /// \f$ O(N) \f$.
            template<typename List, typename F>
            using any_of = not_<empty<find_if<List, F>>>;

            namespace lazy
            {
                template<typename List, typename Fn>
                using any_of = defer<any_of, List, Fn>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // none_of
            /// A Boolean integral constant wrapper around \c true if
            /// `apply<F, A>::%value` is \c false for alls elements \c A
            /// in \c meta::list \p List; \c false, otherwise.
            /// \par Complexity
            /// \f$ O(N) \f$.
            template<typename List, typename F>
            using none_of = empty<find_if<List, F>>;

            namespace lazy
            {
                template<typename List, typename Fn>
                using none_of = defer<none_of, List, Fn>;
            }

            /// \cond
            namespace meta_detail
            {
                template<int, typename...As>
                struct lambda_
                {
                private:
                    static constexpr std::size_t arity = sizeof...(As) - 1;
                    using Tags = list<As...>; // Includes the lambda body as the last arg!
                    using F = back<Tags>;
                    template<typename T, typename Args, typename Pos = find<Tags, T>>
                    struct impl2
                    {
                        using type = list_element_c<(Tags::size() - Pos::size()), Args>;
                    };
                    template<typename T, typename Args>
                    struct impl2<T, Args, list<>>
                    {
                        using type = T;
                    };
                    template<typename T, typename Args, typename = void>
                    struct impl : impl2<T, Args>
                    {};
                    template<template<typename...> class C, typename...Ts, typename Args>
                    struct impl<defer<C, Ts...>, Args, void_<C<eval<impl<Ts, Args>>...>>>
                    {
                        using type = C<eval<impl<Ts, Args>>...>;
                    };
                    template<int N, typename...Ts, typename Args>
                    struct impl<lambda_<N, Ts...>, Args>
                    {
                        using type = impl;
                        template<typename...Us>
                        using apply =
                            apply_list<lambda_<0, As..., Ts...>, concat<Args, list<Us...>>>;
                    };
                    template<template<typename...> class C, typename...Ts, typename Args>
                    struct impl<C<Ts...>, Args, void_<C<eval<impl<Ts, Args>>...>>>
                    {
                        using type = C<eval<impl<Ts, Args>>...>;
                    };
                public:
                    template<typename...Ts>
                    using apply = eval<if_c<sizeof...(Ts) == arity, impl<F, list<Ts..., void>>>>;
                };
            }
            /// \endcond

            ////////////////////////////////////////////////////////////////////////////////////
            // lambda
            /// For creating anonymous Metafunction Classes.
            /// \code
            /// using L = lambda<_a, _b, std::pair<_b, std::pair<_a, _a>>>;
            /// using P = apply<L, int, short>;
            /// static_assert(std::is_same<P, std::pair<short, std::pair<int, int>>>::value, "");
            /// \endcode
            template<typename...Ts>
            using lambda = meta_detail::lambda_<0, Ts...>;

            // Some argument placeholders for use in \c lambda expressions.
            struct _a; struct _b; struct _c;
            struct _d; struct _e; struct _f;
            struct _g; struct _h; struct _i;

            ////////////////////////////////////////////////////////////////////////////////////
            // cartesian_product
            /// \cond
            namespace meta_detail
            {
                template<typename M2, typename M>
                struct cartesian_product_fn
                {
                    template<typename X>
                    struct lambda0
                    {
                        template<typename Xs>
                        using lambda1 = list<push_front<Xs, X>>;
                        using type = join<transform<M2, quote<lambda1>>>;
                    };
                    using type = join<transform<M, quote_trait<lambda0>>>;
                };
            }
            /// \endcond

            /// Given a list of lists \p ListOfLists, return a new list of lists that is the
            /// Cartesian Product. Like the `sequence` function from the Haskell Prelude.
            /// \par Complexity
            /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
            /// \f$ M \f$ is the size of the inner lists.
            template<typename ListOfLists>
            using cartesian_product =
                foldr<ListOfLists, list<list<>>, quote_trait<meta_detail::cartesian_product_fn>>;

            namespace lazy
            {
                template<typename ListOfLists>
                using cartesian_product = defer<cartesian_product, ListOfLists>;
            }

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
            /// An integral constant wrapper around the result of adding the
            /// two wrapped integers \c T::type::value and \c U::type::value.
            template<typename T, typename U>
            using plus = std::integral_constant<decltype(T::type::value + U::type::value), T::type::value + U::type::value>;

            /// An integral constant wrapper around the result of subtracting the
            /// two wrapped integers \c T::type::value and \c U::type::value.
            template<typename T, typename U>
            using minus = std::integral_constant<decltype(T::type::value - U::type::value), T::type::value - U::type::value>;

            /// An integral constant wrapper around the result of multiplying the
            /// two wrapped integers \c T::type::value and \c U::type::value.
            template<typename T, typename U>
            using multiplies = std::integral_constant<decltype(T::type::value * U::type::value), T::type::value * U::type::value>;

            /// An integral constant wrapper around the result of dividing the
            /// two wrapped integers \c T::type::value and \c U::type::value.
            template<typename T, typename U>
            using divides = std::integral_constant<decltype(T::type::value / U::type::value), T::type::value / U::type::value>;

            /// An integral constant wrapper around the remainder of dividing the
            /// two wrapped integers \c T::type::value and \c U::type::value.
            template<typename T>
            using negate = std::integral_constant<decltype(-T::type::value), -T::type::value>;

            /// An integral constant wrapper around the remainder of dividing the
            /// two wrapped integers \c T::type::value and \c U::type::value.
            template<typename T, typename U>
            using modulus = std::integral_constant<decltype(T::type::value % U::type::value), T::type::value % U::type::value>;

            /// A Boolean integral constant wrapper around the result of comparing
            /// \c T::type::value and \c U::type::value for equality.
            template<typename T, typename U>
            using equal_to = bool_<T::type::value == U::type::value>;

            /// A Boolean integral constant wrapper around the result of comparing
            /// \c T::type::value and \c U::type::value for inequality.
            template<typename T, typename U>
            using not_equal_to = bool_<T::type::value != U::type::value>;

            /// A Boolean integral constant wrapper around \c true if
            /// \c T::type::value is greater than \c U::type::value; \c false, otherwise.
            template<typename T, typename U>
            using greater = bool_<(T::type::value > U::type::value)>;

            /// A Boolean integral constant wrapper around \c true if
            /// \c T::type::value is less than \c U::type::value; \c false, otherwise.
            template<typename T, typename U>
            using less = bool_<(T::type::value < U::type::value)>;

            /// A Boolean integral constant wrapper around \c true if
            /// \c T::type::value is greater than or equal to \c U::type::value; \c false,
            /// otherwise.
            template<typename T, typename U>
            using greater_equal = bool_<(T::type::value >= U::type::value)>;

            /// A Boolean integral constant wrapper around \c true if
            /// \c T::type::value is less than or equal to \c U::type::value; \c false,
            /// otherwise.
            template<typename T, typename U>
            using less_equal = bool_<(T::type::value <= U::type::value)>;

            /// An integral constant wrapper around the result of bitwise-and'ing
            /// the two wrapped integers \c T::type::value and \c U::type::value.
            template<typename T, typename U>
            using bit_and = std::integral_constant<decltype(T::type::value & U::type::value), T::type::value & U::type::value>;

            /// An integral constant wrapper around the result of bitwise-or'ing
            /// the two wrapped integers \c T::type::value and \c U::type::value.
            template<typename T, typename U>
            using bit_or = std::integral_constant<decltype(T::type::value | U::type::value), T::type::value | U::type::value>;

            /// An integral constant wrapper around the result of
            /// bitwise-exclusive-or'ing the two wrapped integers \c T::type::value and
            /// \c U::type::value.
            template<typename T, typename U>
            using bit_xor = std::integral_constant<decltype(T::type::value ^ U::type::value), T::type::value ^ U::type::value>;

            /// An integral constant wrapper around the result of
            /// bitwise-complimenting the wrapped integer \c T::type::value.
            template<typename T>
            using bit_not = std::integral_constant<decltype(~T::type::value), ~T::type::value>;
            /// @}

            namespace lazy
            {
                template<typename T, typename U>
                using plus = defer<plus, T, U>;

                template<typename T, typename U>
                using minus = defer<minus, T, U>;

                template<typename T, typename U>
                using multiplies = defer<multiplies, T, U>;

                template<typename T, typename U>
                using divides = defer<divides, T, U>;

                template<typename T>
                using negate = defer<negate, T>;

                template<typename T, typename U>
                using modulus = defer<modulus, T, U>;

                template<typename T, typename U>
                using equal_to = defer<equal_to, T, U>;

                template<typename T, typename U>
                using not_equal_to = defer<not_equal_to, T, U>;

                template<typename T, typename U>
                using greater = defer<greater, T, U>;

                template<typename T, typename U>
                using less = defer<less, T, U>;

                template<typename T, typename U>
                using greater_equal = defer<greater_equal, T, U>;

                template<typename T, typename U>
                using less_equal = defer<less_equal, T, U>;

                template<typename T, typename U>
                using bit_and = defer<bit_and, T, U>;

                template<typename T, typename U>
                using bit_or = defer<bit_or, T, U>;

                template<typename T, typename U>
                using bit_xor = defer<bit_xor, T, U>;

                template<typename T>
                using bit_not = defer<bit_not, T>;
            }
        }
    }
}

// Make meta::quote work consistently with libc++ containers
// Works around:
//    http://llvm.org/bugs/show_bug.cgi?id=22601 and
//    http://llvm.org/bugs/show_bug.cgi?id=22605
#if defined(__clang__)          && \
    defined(_LIBCPP_VERSION)    && \
    _LIBCPP_VERSION <= 1101     && \
    !defined(RANGES_NO_STD_FORWARD_DECLARACTIONS)

_LIBCPP_BEGIN_NAMESPACE_STD
    template <class, class> struct _LIBCPP_TYPE_VIS_ONLY pair;
    template <class> struct _LIBCPP_TYPE_VIS_ONLY hash;
    template <class> struct _LIBCPP_TYPE_VIS_ONLY less;
    template <class> struct _LIBCPP_TYPE_VIS_ONLY equal_to;
    template <class> struct _LIBCPP_TYPE_VIS_ONLY char_traits;
    template <class, class> class _LIBCPP_TYPE_VIS_ONLY list;
    template <class, class> class _LIBCPP_TYPE_VIS_ONLY forward_list;
    template <class, class> class _LIBCPP_TYPE_VIS_ONLY vector;
    template <class, class> class _LIBCPP_TYPE_VIS_ONLY deque;
    template <class, class, class> class _LIBCPP_TYPE_VIS_ONLY basic_string;
    template <class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY map;
    template <class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY multimap;
    template <class, class, class> class _LIBCPP_TYPE_VIS_ONLY set;
    template <class, class, class> class _LIBCPP_TYPE_VIS_ONLY multiset;
    template <class, class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY unordered_map;
    template <class, class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY unordered_multimap;
    template <class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY unordered_set;
    template <class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY unordered_multiset;
    template <class, class> class _LIBCPP_TYPE_VIS_ONLY queue;
    template <class, class, class> class _LIBCPP_TYPE_VIS_ONLY priority_queue;
    template <class, class> class _LIBCPP_TYPE_VIS_ONLY stack;
_LIBCPP_END_NAMESPACE_STD

namespace ranges
{
    inline namespace v3
    {
        namespace meta
        {
            namespace meta_detail
            {
                template<typename T, typename A = std::allocator<T>>
                using std_list = std::list<T, A>;
                template<typename T, typename A = std::allocator<T>>
                using std_forward_list = std::forward_list<T, A>;
                template<typename T, typename A = std::allocator<T>>
                using std_vector = std::vector<T, A>;
                template<typename T, typename A = std::allocator<T>>
                using std_deque = std::deque<T, A>;
                template<typename T, typename C = std::char_traits<T>, typename A = std::allocator<T>>
                using std_basic_string = std::basic_string<T, C, A>;
                template<typename K, typename V, typename C = std::less<K>, typename A = std::allocator<std::pair<K const, V>>>
                using std_map = std::map<K, V, C, A>;
                template<typename K, typename V, typename C = std::less<K>, typename A = std::allocator<std::pair<K const, V>>>
                using std_multimap = std::multimap<K, V, C, A>;
                template<typename K, typename C = std::less<K>, typename A = std::allocator<K>>
                using std_set = std::set<K, C, A>;
                template<typename K, typename C = std::less<K>, typename A = std::allocator<K>>
                using std_multiset = std::multiset<K, C, A>;
                template<typename K, typename V, typename H = std::hash<K>, typename C = std::equal_to<K>, typename A = std::allocator<std::pair<K const, V>>>
                using std_unordered_map = std::unordered_map<K, V, H, C, A>;
                template<typename K, typename V, typename H = std::hash<K>, typename C = std::equal_to<K>, typename A = std::allocator<std::pair<K const, V>>>
                using std_unordered_multimap = std::unordered_multimap<K, V, H, C, A>;
                template<typename K, typename H = std::hash<K>, typename C = std::equal_to<K>, typename A = std::allocator<K>>
                using std_unordered_set = std::unordered_set<K, H, C, A>;
                template<typename K, typename H = std::hash<K>, typename C = std::equal_to<K>, typename A = std::allocator<K>>
                using std_unordered_multiset = std::unordered_multiset<K, H, C, A>;
                template<typename T, typename C = std_deque<T>>
                using std_queue = std::queue<T, C>;
                template<typename T, typename C = std_vector<T>, class D = std::less<typename C::value_type>>
                using std_priority_queue = std::priority_queue<T, C, D>;
                template<typename T, typename C = std_deque<T>>
                using std_stack = std::stack<T, C>;
            }

            template<>
            struct quote< ::std::list >
              : quote< meta_detail::std_list >
            {};
            template<>
            struct quote< ::std::forward_list >
              : quote< meta_detail::std_forward_list >
            {};
            template<>
            struct quote< ::std::vector >
              : quote< meta_detail::std_vector >
            {};
            template<>
            struct quote< ::std::deque >
              : quote< meta_detail::std_deque >
            {};
            template<>
            struct quote< ::std::basic_string >
              : quote< meta_detail::std_basic_string >
            {};
            template<>
            struct quote< ::std::map >
              : quote< meta_detail::std_map >
            {};
            template<>
            struct quote< ::std::multimap >
              : quote< meta_detail::std_multimap >
            {};
            template<>
            struct quote< ::std::set >
              : quote< meta_detail::std_set >
            {};
            template<>
            struct quote< ::std::multiset >
              : quote< meta_detail::std_multiset >
            {};
            template<>
            struct quote< ::std::unordered_map >
              : quote< meta_detail::std_unordered_map >
            {};
            template<>
            struct quote< ::std::unordered_multimap >
              : quote< meta_detail::std_unordered_multimap >
            {};
            template<>
            struct quote< ::std::unordered_set >
              : quote< meta_detail::std_unordered_set >
            {};
            template<>
            struct quote< ::std::unordered_multiset >
              : quote< meta_detail::std_unordered_multiset >
            {};
            template<>
            struct quote< ::std::queue >
              : quote< meta_detail::std_queue >
            {};
            template<>
            struct quote< ::std::priority_queue >
              : quote< meta_detail::std_priority_queue >
            {};
            template<>
            struct quote< ::std::stack >
              : quote< meta_detail::std_stack >
            {};
        }
    }
}

#endif

#endif
