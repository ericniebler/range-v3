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
#include <initializer_list>
#include <range/v3/utility/nullptr_v.hpp>
#include <range/v3/utility/static_const.hpp>

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

            /// An integral constant wrapper for \c std::size_t.
            template<std::size_t N>
            using size_t = std::integral_constant<std::size_t, N>;

            /// An integral constant wrapper for \c bool.
            template<bool B>
            using bool_ = std::integral_constant<bool, B>;

            /// An integral constant wrapper for \c int.
            template<int I>
            using int_ = std::integral_constant<int, I>;

            /// An integral constant wrapper for \c char.
            template<char Ch>
            using char_ = std::integral_constant<char, Ch>;

            ////////////////////////////////////////////////////////////////////////////////////
            // Math operations
            /// An integral constant wrapper around the result of incrementing the
            /// wrapped integer \c T::type::value.
            template<typename T>
            using inc = std::integral_constant<decltype(T::type::value), T::type::value + 1>;

            /// An integral constant wrapper around the result of decrementing the
            /// wrapped integer \c T::type::value.
            template<typename T>
            using dec = std::integral_constant<decltype(T::type::value), T::type::value - 1>;

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
                template<typename T>
                using inc = defer<inc, T>;

                template<typename T>
                using dec = defer<dec, T>;

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

            /// "Evaluate" the trait \p T by returning the nested \c T::type alias.
            template<typename T>
            using eval = typename T::type;

            /// Evaluate the Alias Class \p F with the arguments \p Args.
            template<typename F, typename...Args>
            using apply = typename F::template apply<Args...>;

            namespace lazy
            {
                template<typename T>
                using eval = defer<eval, T>;

                template<typename F, typename...Args>
                using apply = defer<apply, F, Args...>;
            }

            /// A Alias Class that always returns \p T.
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

                template<template<typename...> class C, typename, typename = void>
                struct defer_
                {};

                template <typename T, template <T...> class C, typename, typename = void>
                struct defer_i_
                {};

                template <typename T, template <T...> class C, T... Is>
                struct defer_i_<T, C, integer_sequence<T, Is...>, void_<C<Is...>>>
                {
                    using type = C<Is...>;
                };
            }
            /// \endcond

            /// An alias for `std::true_type` if `T::type` exists and names a type;
            ///        otherwise, it's an alias for `std::false_type`.
            template<typename T>
            using has_type = eval<meta_detail::has_type_<T>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // defer
            /// A wrapper that defers the instantiation of a template \p C with type parameters
            /// \p Ts in a \c lambda or \c let expression.
            ///
            /// In the code below, the lambda would ideally be written as
            /// `lambda<_a,_b,push_back<_a,_b>>`, however this fails since `push_back`
            /// expects its first argument to be a list, not a placeholder. Instead,
            /// we express it using \c defer as follows:
            ///
            /// \code
            /// template<typename List>
            /// using reverse = reverse_fold<List, list<>, lambda<_a, _b, defer<push_back, _a, _b> > >;
            /// \endcode
            template<template<typename...> class C, typename...Ts>
            struct defer
              : meta_detail::defer_<C, list<Ts...>>
            {};

            ///////////////////////////////////////////////////////////////////////////////////////////
            // defer_i
            /// A wrapper that defers the instantiation of a template \p C with integral constant
            /// parameters \p Is in a \c lambda or \c let expression.
            /// \sa `defer`
            template<typename T, template<T...> class C, T... Is>
            struct defer_i
              : meta_detail::defer_i_<T, C, integer_sequence<T, Is...>>
            {};

            ////////////////////////////////////////////////////////////////////////////////////
            // defer_trait
            /// A wrapper that defers the instantiation of a trait \p C with type parameters \p Ts
            /// in a \c lambda or \c let expression.
            /// \sa `defer`
            template<template<typename...> class C, typename...Ts>
            using defer_trait = lazy::eval<defer<C, Ts...>>;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // defer_trait_i
            /// A wrapper that defers the instantiation of a trait \p C with integral constant
            /// parameters \p Is in a \c lambda or \c let expression.
            /// \sa `defer`
            template <typename T, template <T...> class C, T... Is>
            using defer_trait_i = lazy::eval<defer_i<T, C, Is...>>;

            /// An alias that computes the size of the type \p T.
            /// \par Complexity
            /// \f$ O(1) \f$.
            template<class T>
            using sizeof_ = meta::size_t<sizeof(T)>;

            /// An alias that computes the alignment required for
            /// any instance of the type \p T.
            /// \par Complexity
            /// \f$ O(1) \f$.
            template<class T>
            using alignof_ = meta::size_t<alignof(T)>;

            namespace lazy
            {
                template<typename T>
                using sizeof_ = defer<sizeof_, T>;

                template<typename T>
                using alignof_ = defer<alignof_, T>;
            }

            /// A trait that always returns its argument \p T.
            template<typename T>
            struct id
            {
                using type = T;
            };

            /// An alias that is type \c T. Useful in non-deduced contexts.
            template<typename T>
            using id_t = eval<id<T>>;

            namespace lazy
            {
                template<typename T>
                using id = defer<id, T>;
            }

            /// Turn a class template or alias template \p C into a
            /// Alias Class.
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
            /// type \p T into a Alias Class.
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

            /// Turn a trait \p C into a Alias Class.
            template<template<typename...> class C>
            struct quote_trait
            {
                template<typename...Ts>
                using apply = eval<apply<quote<C>, Ts...>>;
            };

            /// Turn a trait \p C taking literals of type \p T
            /// into a Alias Class.
            template<typename T, template<T...> class C>
            struct quote_trait_i
            {
                template<typename...Ts>
                using apply = eval<apply<quote_i<T, C>, Ts...>>;
            };

            /// Compose the Alias Classes \p Fs in the parameter pack
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

            /// A Alias Class that partially applies the Metafunction
            /// Class \p F by binding the arguments \p Ts to the \e front
            /// of \p F.
            template<typename F, typename...Ts>
            struct bind_front
            {
                template<typename...Us>
                using apply = apply<F, Ts..., Us...>;
            };

            /// A Alias Class partially applies the Metafunction
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

            namespace meta_detail
            {
                template<typename, typename, typename = void>
                struct lazy_apply_
                {};
                template<typename F, typename...Args>
                struct lazy_apply_<F, list<Args...>, void_<apply<F, Args...>>>
                {
                    using type = apply<F, Args...>;
                };
            }

            template<typename F, typename...Args>
            struct lazy_apply_wrap
              : meta_detail::lazy_apply_<F, list<Args...>>
            {};

            /// A trait that unpacks the types in the type list
            /// \p List into the Alias Class \p F.
            namespace extension
            {
                template<typename F, typename List>
                struct apply_list
                {};

                template<typename F, template<typename...> class T, typename ...Ts>
                struct apply_list<F, T<Ts...>>
                  : lazy_apply_wrap<F, Ts...>
                {};

                template<typename F, typename T, T...Is>
                struct apply_list<F, integer_sequence<T, Is...>>
                  : lazy_apply_wrap<F, std::integral_constant<T, Is>...>
                {};
            }

            /// Applies the Alias Class \p C using the types in
            /// the type list \p List as arguments.
            template<typename C, typename List>
            using apply_list = eval<extension::apply_list<C, List>>;

            namespace lazy
            {
                template<typename F, typename List>
                using apply_list = defer<apply_list, F, List>;
            }

            /// A Alias Class that takes a bunch of arguments,
            /// bundles them into a type list, and then calls the Metafunction
            /// Class \p F with the type list \p Q.
            template<typename F, typename Q = quote<list>>
            using curry = compose<F, Q>;

            /// A Alias Class that takes a type list,
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

            /// A Alias Class that reverses the order of the first
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
                  : lazy_apply_wrap<F, B, A, Ts...>
                {};
            public:
                template<typename ...Ts>
                using apply = eval<impl<Ts...>>;
            };
            /// @}

            namespace lazy
            {
                template<typename F>
                using flip = defer<flip, F>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // if_
            /// \cond
            namespace meta_detail
            {
                template<typename...>
                struct _if_
                {};

                template<typename If>
                struct _if_<If>
                  : std::enable_if<If::type::value>
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
                /// \sa 'meta::if_'
                template<typename...Args>
                using if_ = defer<if_, Args...>;

                /// \sa 'meta::if_c'
                template <bool If, typename... Args>
                using if_c = if_<bool_<If>, Args...>;
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

                template<typename...Bools>
                using fast_and = defer<fast_and, Bools...>;

                template<typename...Bools>
                using fast_or = defer<fast_or, Bools...>;
            }

            ///////////////////////////////////////////////////////////////////////////////////////
            // npos
            /// A special value used to indicate no matches. It equals the maximum value
            /// representable by `std::size_t`.
            /// \ingroup group-meta
            using npos = meta::size_t<std::size_t(-1)>;

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
            // at
            /// \cond
            namespace meta_detail
            {
                template<typename VoidPtrs>
                struct at_impl_;

                template<typename ...VoidPtrs>
                struct at_impl_<list<VoidPtrs...>>
                {
                    static nil_ eval(...);

                    template<typename T, typename ...Us>
                    static T eval(VoidPtrs..., T *, Us *...);
                };

                template<typename List, typename N>
                struct at_
                {};

                template<typename...Ts, typename N>
                struct at_<list<Ts...>, N>
                  : decltype(at_impl_<repeat_n<N, void *>>::eval(_nullptr_v<id<Ts>>()...))
                {};
            }
            /// \endcond

            ////////////////////////////////////////////////////////////////////////////////////
            // at
            /// Return the \p N th element in the \c meta::list \p List.
            /// \par Complexity
            /// Amortized \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename List, typename N>
            using at = eval<meta_detail::at_<List, N>>;

            /// Return the `N`th element in the \c meta::list \c List.
            /// \par Complexity
            /// Amortized \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename List, std::size_t N>
            using at_c = at<List, meta::size_t<N>>;

            namespace lazy
            {
                template<typename List, typename N>
                using at = defer<at, List, N>;
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

                template<typename List, typename N>
                struct drop_
                {};

                template<typename ...Ts, typename N>
                struct drop_<list<Ts...>, N>
                  : decltype(drop_impl_<repeat_n<N, void *>>::eval(_nullptr_v<id<Ts>>()...))
                {};
            }
            /// \endcond

            /// Return a new \c meta::list by removing the first \p N
            /// elements from \p List.
            /// \par Complexity
            /// \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename List, typename N>
            using drop = eval<meta_detail::drop_<List, N>>;

            /// Return a new \c meta::list by removing the first \p N
            /// elements from \p List.
            /// \par Complexity
            /// \f$ O(1) \f$.
            /// \ingroup group-meta
            template<typename List, std::size_t N>
            using drop_c = eval<meta_detail::drop_<List, meta::size_t<N>>>;

            namespace lazy
            {
                template<typename List, typename N>
                using drop = defer<drop, List, N>;
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
                    using type = at_c<list<Head, List...>, sizeof...(List)>;
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
            // reverse_find
            /// \cond
            namespace meta_detail
            {
                template<typename List, typename T, typename State = list<>>
                struct reverse_find_
                {};

                template<typename T, typename State>
                struct reverse_find_<list<>, T, State>
                {
                    using type = State;
                };

                template<typename Head, typename ...List, typename T, typename State>
                struct reverse_find_<list<Head, List...>, T, State>
                  : reverse_find_<list<List...>, T, State>
                {};

                template<typename ...List, typename T, typename State>
                struct reverse_find_<list<T, List...>, T, State>
                  : reverse_find_<list<List...>, T, list<T, List...>>
                {};
            }
            /// \endcond

            /// Return the tail of the list \p List starting at the last occurrence
            /// of \p T, if any such element exists; the empty list, otherwise.
            /// \ingroup group-meta
            template<typename List, typename T>
            using reverse_find = eval<meta_detail::reverse_find_<List, T>>;

            namespace lazy
            {
                template<typename List, typename T>
                using reverse_find = defer<reverse_find, List, T>;
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
            // reverse_find_if
            /// \cond
            namespace meta_detail
            {
                template<typename List, typename Fun, typename State = list<>>
                struct reverse_find_if_
                {};

                template<typename Fun, typename State>
                struct reverse_find_if_<list<>, Fun, State>
                {
                    using type = State;
                };

                template<typename Head, typename ...List, typename Fun, typename State>
                struct reverse_find_if_<list<Head, List...>, Fun, State>
                  : reverse_find_if_<list<List...>, Fun, if_<apply<Fun, Head>, list<Head, List...>, State>>
                {};
            }
            /// \endcond

            /// Return the tail of the list \p List starting at the last element
            /// `A` such that `apply<Fun, A>::%value` is \c true, if any such element
            /// exists; the empty list, otherwise.
            /// \ingroup group-meta
            template<typename List, typename Fun>
            using reverse_find_if = eval<meta_detail::reverse_find_if_<List, Fun>>;

            namespace lazy
            {
                template<typename List, typename Fun>
                using reverse_find_if = defer<reverse_find_if, List, Fun>;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////
            // find_index
            /// \cond
            namespace meta_detail
            {
                template<typename List, typename T>
                struct find_index_
                {
                    static constexpr std::size_t i = List::size() - find<List, T>::size();
                    using type = if_c<i == List::size(), npos, size_t<i>>;
                };
            } // namespace detail
            /// \endcond

            /// Finds the index of the first occurrence of the type \p T within the list \p List.
            /// Returns `#meta::npos` if the type \p T was not found.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename T>
            using find_index = eval<meta_detail::find_index_<List, T>>;

            namespace lazy
            {
                /// \sa `meta::index`
                /// \ingroup group-meta
                template<typename List, typename T>
                using find_index = defer<find_index, List, T>;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////
            // reverse_find_index
            /// \cond
            namespace meta_detail
            {
                template<typename List, typename T>
                struct reverse_find_index_
                {
                    static constexpr std::size_t i = List::size() - reverse_find<List, T>::size();
                    using type = if_c<i == List::size(), npos, size_t<i>>;
                };
            } // namespace detail
            /// \endcond

            /// Finds the index of the last occurrence of the type \p T within the list \p List.
            /// Returns `#meta::npos` if the type \p T was not found.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup query
            /// \sa `#meta::npos`
            template<typename List, typename T>
            using reverse_find_index = eval<meta_detail::reverse_find_index_<List, T>>;

            namespace lazy
            {
                /// \sa 'meta::reverse_find_index'
                /// \ingroup lazy_query
                template<typename List, typename T>
                using reverse_find_index = defer<reverse_find_index, List, T>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // fold
            /// \cond
            namespace meta_detail
            {
                template<typename, typename, typename, typename = void>
                struct fold_
                {};

                template<typename State, typename Fun>
                struct fold_<list<>, State, Fun>
                {
                    using type = State;
                };

                template<typename Head, typename ...List, typename State, typename Fun>
                struct fold_<list<Head, List...>, State, Fun, void_<apply<Fun, State, Head>>>
                  : fold_<list<List...>, apply<Fun, State, Head>, Fun>
                {};
            }
            /// \endcond

            /// Return a new \c meta::list constructed by doing a left fold
            /// of the list \p List using binary Alias Class \p Fun and
            /// initial state \p State.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename State, typename Fun>
            using fold = eval<meta_detail::fold_<List, State, Fun>>;

            /// An alias for `meta::fold`.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename State, typename Fun>
            using accumulate = fold<List, State, Fun>;

            namespace lazy
            {
                template<typename List, typename State, typename Fun>
                using fold = defer<fold, List, State, Fun>;

                template<typename List, typename State, typename Fun>
                using accumulate = defer<accumulate, List, State, Fun>;
            }

            ////////////////////////////////////////////////////////////////////////////////////
            // reverse_fold
            /// \cond
            namespace meta_detail
            {
                template<typename, typename, typename, typename = void>
                struct reverse_fold_
                {};

                template<typename State, typename Fun>
                struct reverse_fold_<list<>, State, Fun>
                {
                    using type = State;
                };

                template<typename Head, typename ...List, typename State, typename Fun>
                struct reverse_fold_<list<Head, List...>, State, Fun, void_<eval<reverse_fold_<list<List...>, State, Fun>>>>
                  : lazy_apply_wrap<Fun, eval<reverse_fold_<list<List...>, State, Fun>>, Head>
                {};
            }
            /// \endcond

            /// Return a new \c meta::list constructed by doing a right fold
            /// of the list \p List using binary Alias Class \p Fun and
            /// initial state \p State.
            /// \par Complexity
            /// \f$ O(N) \f$.
            /// \ingroup group-meta
            template<typename List, typename State, typename Fun>
            using reverse_fold = eval<meta_detail::reverse_fold_<List, State, Fun>>;

            namespace lazy
            {
                template<typename List, typename State, typename Fun>
                using reverse_fold = defer<reverse_fold, List, State, Fun>;
            }
            /// @}

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

            ///////////////////////////////////////////////////////////////////////////////////////////
            // set
            // Used to improve the performance of \c meta::unique.
            /// \cond
            namespace meta_detail
            {
                template <typename... Nodes>
                struct root_ : Nodes...
                {
                };

                template <typename... Ts>
                using set_ = root_<id<Ts>...>;

                template <typename Set, typename T>
                struct in_
                {
                };

                template <typename... Set, typename T>
                struct in_<list<Set...>, T> : std::is_base_of<id<T>, set_<Set...>>
                {
                };

                template <typename Set, typename T>
                struct insert_back_
                {
                };

                template <typename... Set, typename T>
                struct insert_back_<list<Set...>, T>
                {
                    using type = if_<in_<list<Set...>, T>, list<Set...>, list<Set..., T>>;
                };
            } // namespace meta_detail
            /// \endcond

            ///////////////////////////////////////////////////////////////////////////////////////////
            // unique
            /// Return a new \c meta::list where all duplicate elements have been removed.
            /// \par Complexity
            /// \f$ O(N^2) \f$.
            /// \ingroup group-meta
            template <typename List>
            using unique = fold<List, list<>, quote_trait<meta_detail::insert_back_>>;

            namespace lazy
            {
                /// \sa 'meta::unique'
                /// \ingroup group-meta
                template <typename List>
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

            ///////////////////////////////////////////////////////////////////////////////////////
            // count
            namespace meta_detail
            {
                template<typename State, typename Val, typename T>
                using count_fn = if_<std::is_same<Val, T>, inc<State>, State>;
            }

            /// Count the number of times a type \p T appears in the list \p List.
            /// \par Complexity
            /// \f$ O(N) \f$.
            template<typename List, typename T>
            using count = fold<List, meta::size_t<0>, bind_back<quote<meta_detail::count_fn>, T>>;

            namespace lazy
            {
                /// \sa `meta::count`
                /// \ingroup group-meta
                template<typename List, typename T>
                using count = defer<count, List, T>;
            }

            ///////////////////////////////////////////////////////////////////////////////////////
            // count_if
            namespace meta_detail
            {
                template<typename State, typename Val, typename Fn>
                using count_if_fn = if_<apply<Fn, Val>, inc<State>, State>;
            }

            /// Count the number of times the predicate \p Fn evaluates to true for all the
            /// elements in the list \p List.
            /// \par Complexity
            /// \f$ O(N) \f$.
            template<typename List, typename Fn>
            using count_if =
                fold<List, meta::size_t<0>, bind_back<quote<meta_detail::count_if_fn>, Fn>>;

            namespace lazy
            {
                /// \sa `meta::count_if`
                /// \ingroup group-meta
                template<typename List, typename Fn>
                using count_if = defer<count_if, List, Fn>;
            }

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
            /// and a binary Alias Class, in which case it returns a new list
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

            ///////////////////////////////////////////////////////////////////////////////////////
            // filter
            /// \cond
            namespace meta_detail
            {
                template<typename Predicate>
                struct filter_
                {
                    template<typename State, typename A>
                    using apply = if_<apply<Predicate, A>, push_back<State, A>, State>;
                };
            } // namespace detail
            /// \endcond

            /// Returns a new meta::list where only those elements of \p List A that satisfy the
            /// Alias Class \p Predicate such that `apply<Pred,A>::%value` is \c true are
            /// present. That is, those elements that don't satisfy the \p Predicate are "removed".
            template<typename List, typename Predicate>
            using filter = fold<List, list<>, meta_detail::filter_<Predicate>>;

            namespace lazy
            {
                template<typename List, typename Predicate>
                using filter = defer<filter, List, Predicate>;
            }

            ///////////////////////////////////////////////////////////////////////////////////////
            // for_each
            /// \cond
            namespace meta_detail
            {
                struct for_each_fn
                {
                    template<class UnaryFunction, class... Args>
                    constexpr auto operator()(list<Args...>, UnaryFunction f) const
                        -> UnaryFunction
                    {
                        return (void)std::initializer_list<int>{(f(Args{}), void(), 0)...}, f;
                    }
                };
            } // namespace meta_detail
            /// \endcond

            namespace
            {
                /// `for_each(List, UnaryFunction)` calls the \p UnaryFunction for each
                /// argument in the \p List.
                constexpr auto &&for_each = static_const<meta_detail::for_each_fn>::value;
            }

            ///////////////////////////////////////////////////////////////////////////////////////
            // zip_with
            /// Given a list of lists \p ListOfLists of types and a Alias Class \p Fun,
            /// construct a new list by calling \p Fun with the elements from the lists
            /// pairwise.
            /// \par Complexity
            /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
            /// \f$ M \f$ is the size of the inner lists.
            /// \ingroup group-meta
            template<typename Fun, typename ListOfLists>
            using zip_with =
                transform<
                    fold<
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
                  : lazy_apply_wrap<uncurry<curry<quote_trait<id>>>, uncvref_t<Sequence>>
                {};
            }
            /// \endcond

            /// \addtogroup group-meta
            /// @{

            /// Turn a type into an instance of \c meta::list in a way
            /// determined by \c meta::apply_list.
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
            using reverse = reverse_fold<List, list<>, quote<push_back>>;

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
                template<typename T, int = 0>
                struct protect;

                template<typename, int = 0>
                struct vararg_;

                template<typename T, int = 0>
                struct is_valid_;

                // Returns which branch to evaluate
                template<typename If, typename ...Ts>
                using lazy_if_ = lazy::eval<defer<_if_, If, protect<Ts>...>>;

                template<typename A, typename T, typename F, typename Ts>
                struct subst1_
                {
                    using type = list<list<T>>;
                };
                template<typename T, typename F, typename Ts>
                struct subst1_<F, T, F, Ts>
                {
                    using type = list<>;
                };
                template<typename A, typename T, typename F, typename Ts>
                struct subst1_<vararg_<A>, T, F, Ts>
                {
                    using type = list<Ts>;
                };

                template<typename As, typename Ts>
                using substitutions_ = push_back<join<transform<
                    concat<As, repeat_n_c<size<Ts>{} + 2 - size<As>{}, back<As>>>,
                    concat<Ts, list<back<As>, back<As>>>,
                    compose<quote<eval>, bind_back<quote<subst1_>, back<As>,
                        drop<Ts, minus<size<As>, meta::size_t<2>>>>>>>, list<back<As>>>;

                template <typename As, typename Ts>
                using substitutions =
                    apply<if_c<(size<Ts>{} + 2 >= size<As>{}), quote<substitutions_>>, As, Ts>;

                template<typename T>
                struct is_vararg_
                  : std::false_type
                {};
                template<typename T>
                struct is_vararg_<vararg_<T>>
                  : std::true_type
                {};

                template<typename Tags>
                using is_variadic_ = is_vararg_<at<push_front<Tags, void>, dec<size<Tags>>>>;

                template<typename Tags, bool IsVariadic = is_variadic_<Tags>::value>
                struct lambda_;

                // Non-variadic lambda implementation
                template<typename...As>
                struct lambda_<list<As...>, false>
                {
                private:
                    static constexpr std::size_t arity = sizeof...(As) - 1;
                    using Tags = list<As...>; // Includes the lambda body as the last arg!
                    using F = back<Tags>;
                    template<typename T, typename Args> struct impl;
                    template<typename T, typename Args>
                    using lazy_impl_ = lazy::eval<defer<impl, T, protect<Args>>>;
                    template<typename, typename, typename = void>
                    struct subst_
                    {};
                    template<template<typename...> class C, typename...Ts, typename Args>
                    struct subst_<defer<C, Ts...>, Args, void_<C<eval<impl<Ts, Args>>...>>>
                    {
                        using type = C<eval<impl<Ts, Args>>...>;
                    };
                    template <typename T, template <T...> class C, T... Is, typename Args>
                    struct subst_<defer_i<T, C, Is...>, Args, void_<C<Is...>>>
                    {
                        using type = C<Is...>;
                    };
                    template<typename T, typename Args>
                    struct lookup
                    {
                        using type = at<Args, reverse_find_index<Tags, T>>;
                    };
                    template<typename T, typename Args>
                    struct impl
                      : if_<in<Tags, T>, lookup<T, Args>, id<T>>
                    {};
                    template<typename T, typename Args>
                    struct impl<protect<T>, Args>
                    {
                        using type = T;
                    };
                    template<typename T, typename Args>
                    struct impl<is_valid_<T>, Args>
                    {
                        using type = has_type<impl<T, Args>>;
                    };
                    template<typename If, typename ...Ts, typename Args>
                    struct impl<defer<if_, If, Ts...>, Args> // Short-circuit if_
                      : impl<lazy_impl_<lazy_if_<If, Ts...>, Args>, Args>
                    {};
                    template<typename Bool, typename ...Ts, typename Args>
                    struct impl<defer<and_, Bool, Ts...>, Args> // Short-circuit and_
                      : impl<lazy_impl_<lazy_if_<Bool, lazy::and_<Ts...>, protect<std::false_type>>,
                            Args>, Args>
                    {};
                    template<typename Bool, typename ...Ts, typename Args>
                    struct impl<defer<or_, Bool, Ts...>, Args> // Short-circuit or_
                      : impl<lazy_impl_<lazy_if_<Bool, protect<std::true_type>, lazy::or_<Ts...>>,
                            Args>, Args>
                    {};
                    template<template<typename...> class C, typename...Ts, typename Args>
                    struct impl<defer<C, Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                    {};
                    template <typename T, template <T...> class C, T... Is, typename Args>
                    struct impl<defer_i<T, C, Is...>, Args> : subst_<defer_i<T, C, Is...>, Args>
                    {};
                    template<template<typename...> class C, typename...Ts, typename Args>
                    struct impl<C<Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                    {};
                    template<typename...Ts, typename Args>
                    struct impl<lambda_<list<Ts...>, false>, Args>
                    {
                        using type = compose<uncurry<lambda_<list<As..., Ts...>, false>>,
                            curry<bind_front<quote<concat>, Args>>>;
                    };
                    template<typename...Bs, typename Args>
                    struct impl<lambda_<list<Bs...>, true>, Args>
                    {
                        using type = compose<
                            typename lambda_<list<As..., Bs...>, true>::thunk,
                            bind_front<quote<concat>, transform<Args, quote<list>>>,
                            curry<bind_front<quote<substitutions>, list<Bs...>>>>;
                    };
                public:
                // Work around GCC #64970
                // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64970
                #if defined(__GNUC__) && !defined(__clang__)
                    template<typename...Ts>
                    using can_apply_ = and_<bool_<sizeof...(Ts) == arity>,
                        has_type<impl<F, list<Ts..., F>>>>;
                #endif
                    template<typename...Ts>
                    using apply = eval<if_c<sizeof...(Ts) == arity, impl<F, list<Ts..., F>>>>;
                };

                // Lambda with variadic placeholder (broken out due to less efficient compile-time
                // resource usage)
                template<typename...As>
                struct lambda_<list<As...>, true>
                {
                private:
                    template<typename T, bool IsVar> friend struct lambda_;
                    using Tags = list<As...>; // Includes the lambda body as the last arg!
                    template<typename T, typename Args> struct impl;
                    template<typename Args>
                    using eval_impl_ = compose<quote<eval>, bind_back<quote<impl>, Args>>;
                    template<typename T, typename Args>
                    using lazy_impl_ = lazy::eval<defer<impl, T, protect<Args>>>;
                    template<template<typename...> class C, typename Args, typename Ts>
                    using try_subst_ = apply_list<quote<C>, join<transform<Ts, eval_impl_<Args>>>>;
                    template<typename, typename, typename = void>
                    struct subst_
                    {};
                    template<template<typename...> class C, typename...Ts, typename Args>
                    struct subst_<defer<C, Ts...>, Args, void_<try_subst_<C, Args, list<Ts...>>>>
                    {
                        using type = list<try_subst_<C, Args, list<Ts...>>>;
                    };
                    template <typename T, template <T...> class C, T... Is, typename Args>
                    struct subst_<defer_i<T, C, Is...>, Args, void_<C<Is...>>>
                    {
                        using type = list<C<Is...>>;
                    };
                    template<typename T, typename Args>
                    struct lookup
                    {
                        using type = at<Args, reverse_find_index<Tags, T>>;
                    };
                    template<typename T, typename Args>
                    struct impl
                      : if_<in<Tags, T>, lookup<T, Args>, id<list<T>>>
                    {};
                    template<typename T, typename Args>
                    struct impl<protect<T>, Args>
                    {
                        using type = list<T>;
                    };
                    template<typename T, typename Args>
                    struct impl<is_valid_<T>, Args>
                    {
                        using type = list<has_type<impl<T, Args>>>;
                    };
                    template<typename If, typename ...Ts, typename Args>
                    struct impl<defer<if_, If, Ts...>, Args> // Short-circuit if_
                      : impl<lazy_impl_<lazy_if_<If, Ts...>, Args>, Args>
                    {};
                    template<typename Bool, typename ...Ts, typename Args>
                    struct impl<defer<and_, Bool, Ts...>, Args> // Short-circuit and_
                      : impl<lazy_impl_<lazy_if_<Bool, lazy::and_<Ts...>, protect<std::false_type>>,
                            Args>, Args>
                    {};
                    template<typename Bool, typename ...Ts, typename Args>
                    struct impl<defer<or_, Bool, Ts...>, Args> // Short-circuit or_
                      : impl<lazy_impl_<lazy_if_<Bool, protect<std::true_type>, lazy::or_<Ts...>>,
                            Args>, Args>
                    {};
                    template<template<typename...> class C, typename...Ts, typename Args>
                    struct impl<defer<C, Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                    {};
                    template <typename T, template <T...> class C, T... Is, typename Args>
                    struct impl<defer_i<T, C, Is...>, Args> : subst_<defer_i<T, C, Is...>, Args>
                    {};
                    template<template<typename...> class C, typename...Ts, typename Args>
                    struct impl<C<Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                    {};
                    template<typename...Bs, bool IsVar, typename Args>
                    struct impl<lambda_<list<Bs...>, IsVar>, Args>
                    {
                        using type = list<compose<
                            typename lambda_<list<As..., Bs...>, true>::thunk,
                            bind_front<quote<concat>, Args>,
                            curry<bind_front<quote<substitutions>, list<Bs...>>>>>;
                    };
                    struct thunk
                    {
                        template<typename S, typename R = eval<impl<back<Tags>, S>>>
                        using apply = if_c<size<R>{} == 1, front<R>>;
                    };
                public:
                    template<typename...Ts>
                    using apply = meta::apply<thunk, substitutions<Tags, list<Ts...>>>;
                };
            }
            /// \endcond

            ////////////////////////////////////////////////////////////////////////////////////
            // lambda
            /// For creating anonymous Alias Classes.
            /// \code
            /// using L = lambda<_a, _b, std::pair<_b, std::pair<_a, _a>>>;
            /// using P = apply<L, int, short>;
            /// static_assert(std::is_same<P, std::pair<short, std::pair<int, int>>>::value, "");
            /// \endcode
            template<typename...Ts>
            using lambda = if_c<(sizeof...(Ts) > 0), meta_detail::lambda_<list<Ts...>>>;

            ////////////////////////////////////////////////////////////////////////////////////
            // is_valid
            /// For testing whether a deferred computation will succeed in a \c let or a \c lambda.
            template<typename T>
            using is_valid = meta_detail::is_valid_<T>;

            ////////////////////////////////////////////////////////////////////////////////////
            // vararg
            /// For defining variadic placeholders.
            template<typename T>
            using vararg = meta_detail::vararg_<T>;

            ///////////////////////////////////////////////////////////////////////////////////////
            // let
            /// For use when defining local variables in \c meta::let expressions
            /// \sa `meta::let`
            template<typename Tag, typename Value>
            struct var;

            /// \cond
            namespace meta_detail
            {
                template<typename...As>
                struct let_
                {};
                template<typename Fn>
                struct let_<Fn>
                {
                    using type = lazy::apply<lambda<Fn>>;
                };
                template<typename Tag, typename Value, typename...Rest>
                struct let_<var<Tag, Value>, Rest...>
                {
                    using type = lazy::apply<lambda<Tag, eval<let_<Rest...>>>, Value>;
                };
                template<typename T, typename = void>
                struct eval_let_
                {};
                template<typename Lambda, typename ...Ts>
                struct eval_let_<lazy::apply<Lambda, Ts...>, void_<apply<Lambda, Ts...>>>
                {
                    using type = apply<Lambda, Ts...>;
                };
            }
            /// \endcond

            /// A lexically scoped expression with local variables.
            ///
            /// \code
            /// template<typename T, typename List>
            /// using find_index_ = let<
            ///     var<_a, List>,
            ///     var<_b, lazy::find<_a, T>>,
            ///     lazy::if_<
            ///         std::is_same<_b, list<>>,
            ///         meta::npos,
            ///         lazy::minus<lazy::size<_a>, lazy::size<_b>>>>;
            /// static_assert(find_index_<int, list<short, int, float>>{} == 1, "");
            /// static_assert(find_index_<double, list<short, int, float>>{} == meta::npos{}, "");
            /// \endcode
            /// \ingroup group-meta
            template<typename...As>
            using let = eval<meta_detail::eval_let_<eval<meta_detail::let_<As...>>>>;

            namespace lazy
            {
                /// \sa `meta::let`
                /// \ingroup group-meta
                template<typename...As>
                using let = defer<let, As...>;
            }

            inline namespace placeholders
            {
                // Some argument placeholders for use in \c lambda and \c let expressions.
                struct _a; struct _b; struct _c;
                struct _d; struct _e; struct _f;
                struct _g; struct _h; struct _i;

                using _args = vararg<void>;
                using _args_a = vararg<_a>;
                using _args_b = vararg<_b>;
                using _args_c = vararg<_c>;
            }

            namespace meta_detail
            {
                // Accessing the nested ::type of any lazy:: computation is equivalent to
                // an evaluation with \c meta::let. This way, nested lazy computations get
                // evaluated recursively.
                #if !defined(__GNUC__) || defined(__clang__)
                template<template<typename...> class C, typename...Ts>
                struct defer_<C, list<Ts...>, void_<apply<lambda<defer<C, Ts...>>>>>
                {
                    using type = apply<lambda<defer<C, Ts...>>>;
                };
                #else
                // Work around GCC #64970
                // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=64970
                template<template<typename...> class C, typename...Ts>
                struct defer_<C, list<Ts...>,
                    if_c<lambda<defer<C, Ts...>>::template can_apply_<>::value>>
                {
                    using type = apply<lambda<defer<C, Ts...>>>;
                };
                #endif
            }

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
                reverse_fold<ListOfLists, list<list<>>, quote_trait<meta_detail::cartesian_product_fn>>;

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

            /// An integral constant wrapper around the minimum of \c T::type::value
            /// and \c U::type::value
            template<typename T, typename U>
            using min = if_<less<U, T>, U, T>;

            /// An integral constant wrapper around the maximum of \c T::type::value
            /// and \c U::type::value
            template<typename T, typename U>
            using max = if_<less<U, T>, T, U>;

            namespace lazy
            {
                template<typename T, typename U>
                using max = defer<max, T, U>;

                template<typename T, typename U>
                using min = defer<min, T, U>;
            }

            /// \cond
            namespace meta_detail
            {
                template <typename State, typename Ch>
                using atoi_ = if_c<(Ch::value >= '0' && Ch::value <= '9'),
                                   std::integral_constant<typename State::value_type,
                                                          State::value * 10 + (Ch::value - '0')>>;
            }
            /// \endcond

            /// A user-defined literal that generates objects of type \c meta::size_t.
            /// \ingroup integral
            template<char...Chs>
            constexpr fold<list<char_<Chs>...>, meta::size_t<0>, quote<meta_detail::atoi_>>
            operator "" _z() { return {}; }
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
    template<class, class> struct _LIBCPP_TYPE_VIS_ONLY pair;
    template<class> struct _LIBCPP_TYPE_VIS_ONLY hash;
    template<class> struct _LIBCPP_TYPE_VIS_ONLY less;
    template<class> struct _LIBCPP_TYPE_VIS_ONLY equal_to;
    template<class> struct _LIBCPP_TYPE_VIS_ONLY char_traits;
    template<class, class> class _LIBCPP_TYPE_VIS_ONLY list;
    template<class, class> class _LIBCPP_TYPE_VIS_ONLY forward_list;
    template<class, class> class _LIBCPP_TYPE_VIS_ONLY vector;
    template<class, class> class _LIBCPP_TYPE_VIS_ONLY deque;
    template<class, class, class> class _LIBCPP_TYPE_VIS_ONLY basic_string;
    template<class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY map;
    template<class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY multimap;
    template<class, class, class> class _LIBCPP_TYPE_VIS_ONLY set;
    template<class, class, class> class _LIBCPP_TYPE_VIS_ONLY multiset;
    template<class, class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY unordered_map;
    template<class, class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY unordered_multimap;
    template<class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY unordered_set;
    template<class, class, class, class> class _LIBCPP_TYPE_VIS_ONLY unordered_multiset;
    template<class, class> class _LIBCPP_TYPE_VIS_ONLY queue;
    template<class, class, class> class _LIBCPP_TYPE_VIS_ONLY priority_queue;
    template<class, class> class _LIBCPP_TYPE_VIS_ONLY stack;
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
