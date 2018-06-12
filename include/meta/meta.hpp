/// \file meta.hpp Tiny meta-programming library.
//
// Meta library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef META_HPP
#define META_HPP

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <meta/meta_fwd.hpp>

#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wdocumentation-deprecated-sync"
#endif

/// \defgroup meta Meta
///
/// A tiny metaprogramming library

/// \defgroup trait Trait
/// Trait invocation/composition.
/// \ingroup meta

/// \defgroup invocation Invocation
/// Trait invocation
/// \ingroup trait

/// \defgroup composition Composition
/// Trait composition
/// \ingroup trait

/// \defgroup logical Logical
/// Logical operations
/// \ingroup meta

/// \defgroup algorithm Algorithms
/// Algorithms.
/// \ingroup meta

/// \defgroup query Query/Search
/// Query and search algorithms
/// \ingroup algorithm

/// \defgroup transformation Transformation
/// Transformation algorithms
/// \ingroup algorithm

/// \defgroup runtime Runtime
/// Runtime algorithms
/// \ingroup algorithm

/// \defgroup datatype Datatype
/// Datatypes.
/// \ingroup meta

/// \defgroup list List
/// \ingroup datatype

/// \defgroup integral Integer sequence
/// Equivalent to C++14's `std::integer_sequence`
/// \ingroup datatype

/// \defgroup extension Extension
/// Extend meta with your own datatypes.
/// \ingroup datatype

/// \defgroup math Math
/// Integral constant arithmetic.
/// \ingroup meta

/// \defgroup lazy_trait lazy
/// \ingroup trait

/// \defgroup lazy_invocation lazy
/// \ingroup invocation

/// \defgroup lazy_composition lazy
/// \ingroup composition

/// \defgroup lazy_logical lazy
/// \ingroup logical

/// \defgroup lazy_query lazy
/// \ingroup query

/// \defgroup lazy_transformation lazy
/// \ingroup transformation

/// \defgroup lazy_list lazy
/// \ingroup list

/// \defgroup lazy_datatype lazy
/// \ingroup datatype

/// \defgroup lazy_math lazy
/// \ingroup math

/// Tiny metaprogramming library
namespace meta
{
    /// \cond
    inline namespace v1
    {
        /// \endcond

        namespace detail
        {
            /// Returns a \p T nullptr
            template <typename T>
            constexpr T *_nullptr_v()
            {
                return nullptr;
            }
        } // namespace detail

        /// An empty type.
        /// \ingroup datatype
        struct nil_
        {
        };

        /// Type alias for \p T::type.
        /// \ingroup invocation
        template <typename T>
        using _t = typename T::type;

#if META_CXX_VARIABLE_TEMPLATES || defined(META_DOXYGEN_INVOKED)
        /// Variable alias for \c T::type::value
        /// \note Requires C++14 or greater.
        /// \ingroup invocation
        template <typename T>
        constexpr typename _t<T>::value_type _v = _t<T>::value;
#endif

        /// Lazy versions of meta actions
        namespace lazy
        {
            /// \sa `meta::_t`
            /// \ingroup lazy_invocation
            template <typename T>
            using _t = defer<_t, T>;
        }

        /// An integral constant wrapper for \c std::size_t.
        /// \ingroup integral
        template <std::size_t N>
        using size_t = std::integral_constant<std::size_t, N>;

        /// An integral constant wrapper for \c bool.
        /// \ingroup integral
        template <bool B>
        using bool_ = std::integral_constant<bool, B>;

        /// An integral constant wrapper for \c int.
        /// \ingroup integral
        template <int I>
        using int_ = std::integral_constant<int, I>;

        /// An integral constant wrapper for \c char.
        /// \ingroup integral
        template <char Ch>
        using char_ = std::integral_constant<char, Ch>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Math operations
        /// An integral constant wrapper around the result of incrementing the wrapped
        /// integer \c
        /// T::type::value.
        template <typename T>
        using inc = std::integral_constant<decltype(T::type::value + 1), T::type::value + 1>;

        /// An integral constant wrapper around the result of decrementing the wrapped
        /// integer \c
        /// T::type::value.
        template <typename T>
        using dec = std::integral_constant<decltype(T::type::value - 1), T::type::value - 1>;

        /// An integral constant wrapper around the result of adding the two wrapped
        /// integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using plus = std::integral_constant<decltype(T::type::value + U::type::value),
                                            T::type::value + U::type::value>;

        /// An integral constant wrapper around the result of subtracting the two
        /// wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using minus = std::integral_constant<decltype(T::type::value - U::type::value),
                                             T::type::value - U::type::value>;

        /// An integral constant wrapper around the result of multiplying the two
        /// wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using multiplies = std::integral_constant<decltype(T::type::value * U::type::value),
                                                  T::type::value * U::type::value>;

        /// An integral constant wrapper around the result of dividing the two wrapped
        /// integers \c
        /// T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using divides = std::integral_constant<decltype(T::type::value / U::type::value),
                                               T::type::value / U::type::value>;

        /// An integral constant wrapper around the result of negating the wrapped
        /// integer
        /// \c T::type::value.
        /// \ingroup math
        template <typename T>
        using negate = std::integral_constant<decltype(-T::type::value), -T::type::value>;

        /// An integral constant wrapper around the remainder of dividing the two
        /// wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using modulus = std::integral_constant<decltype(T::type::value % U::type::value),
                                               T::type::value % U::type::value>;

        /// A Boolean integral constant wrapper around the result of comparing \c
        /// T::type::value and
        /// \c U::type::value for equality.
        /// \ingroup math
        template <typename T, typename U>
        using equal_to = bool_<T::type::value == U::type::value>;

        /// A Boolean integral constant wrapper around the result of comparing \c
        /// T::type::value and
        /// \c U::type::value for inequality.
        /// \ingroup math
        template <typename T, typename U>
        using not_equal_to = bool_<T::type::value != U::type::value>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is
        /// greater than
        /// \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <typename T, typename U>
        using greater = bool_<(T::type::value > U::type::value)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is
        /// less than \c
        /// U::type::value; \c false, otherwise.
        /// \ingroup math
        template <typename T, typename U>
        using less = bool_<(T::type::value < U::type::value)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is
        /// greater than
        /// or equal to \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <typename T, typename U>
        using greater_equal = bool_<(T::type::value >= U::type::value)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is
        /// less than or
        /// equal to \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <typename T, typename U>
        using less_equal = bool_<(T::type::value <= U::type::value)>;

        /// An integral constant wrapper around the result of bitwise-and'ing the two
        /// wrapped
        /// integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using bit_and = std::integral_constant<decltype(T::type::value & U::type::value),
                                               T::type::value & U::type::value>;

        /// An integral constant wrapper around the result of bitwise-or'ing the two
        /// wrapped
        /// integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using bit_or = std::integral_constant<decltype(T::type::value | U::type::value),
                                              T::type::value | U::type::value>;

        /// An integral constant wrapper around the result of bitwise-exclusive-or'ing
        /// the two
        /// wrapped integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <typename T, typename U>
        using bit_xor = std::integral_constant<decltype(T::type::value ^ U::type::value),
                                               T::type::value ^ U::type::value>;

        /// An integral constant wrapper around the result of bitwise-complementing the
        /// wrapped
        /// integer \c T::type::value.
        /// \ingroup math
        template <typename T>
        using bit_not = std::integral_constant<decltype(~T::type::value), ~T::type::value>;

        namespace lazy
        {
            /// \sa 'meta::int'
            /// \ingroup lazy_math
            template <typename T>
            using inc = defer<inc, T>;

            /// \sa 'meta::dec'
            /// \ingroup lazy_math
            template <typename T>
            using dec = defer<dec, T>;

            /// \sa 'meta::plus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using plus = defer<plus, T, U>;

            /// \sa 'meta::minus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using minus = defer<minus, T, U>;

            /// \sa 'meta::multiplies'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using multiplies = defer<multiplies, T, U>;

            /// \sa 'meta::divides'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using divides = defer<divides, T, U>;

            /// \sa 'meta::negate'
            /// \ingroup lazy_math
            template <typename T>
            using negate = defer<negate, T>;

            /// \sa 'meta::modulus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using modulus = defer<modulus, T, U>;

            /// \sa 'meta::equal_to'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using equal_to = defer<equal_to, T, U>;

            /// \sa 'meta::not_equal_t'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using not_equal_to = defer<not_equal_to, T, U>;

            /// \sa 'meta::greater'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using greater = defer<greater, T, U>;

            /// \sa 'meta::less'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using less = defer<less, T, U>;

            /// \sa 'meta::greater_equal'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using greater_equal = defer<greater_equal, T, U>;

            /// \sa 'meta::less_equal'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using less_equal = defer<less_equal, T, U>;

            /// \sa 'meta::bit_and'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_and = defer<bit_and, T, U>;

            /// \sa 'meta::bit_or'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_or = defer<bit_or, T, U>;

            /// \sa 'meta::bit_xor'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_xor = defer<bit_xor, T, U>;

            /// \sa 'meta::bit_not'
            /// \ingroup lazy_math
            template <typename T>
            using bit_not = defer<bit_not, T>;
        }

        /// \cond
        namespace detail
        {
            enum class indices_strategy_
            {
                done,
                repeat,
                recurse
            };

            constexpr indices_strategy_ strategy_(std::size_t cur, std::size_t end)
            {
                return cur >= end ? indices_strategy_::done
                                  : cur * 2 <= end ? indices_strategy_::repeat
                                                   : indices_strategy_::recurse;
            }

            template <typename T>
            constexpr std::size_t range_distance_(T begin, T end)
            {
                return begin <= end ? static_cast<std::size_t>(end - begin)
                                    : throw "The start of the integer_sequence must not be "
                                            "greater than the end";
            }

            template <std::size_t End, typename State, indices_strategy_ Status_>
            struct make_indices_
            {
                using type = State;
            };

            template <typename T, T, typename>
            struct coerce_indices_
            {
            };
        }
/// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // integer_sequence
#if !META_CXX_INTEGER_SEQUENCE
        /// A container for a sequence of compile-time integer constants.
        /// \ingroup integral
        template <typename T, T... Is>
        struct integer_sequence
        {
            using value_type = T;
            /// \return `sizeof...(Is)`
            static constexpr std::size_t size() noexcept { return sizeof...(Is); }
        };
#endif

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // index_sequence
        /// A container for a sequence of compile-time integer constants of type
        /// \c std::size_t
        /// \ingroup integral
        template <std::size_t... Is>
        using index_sequence = integer_sequence<std::size_t, Is...>;

#if META_HAS_MAKE_INTEGER_SEQ && !defined(META_DOXYGEN_INVOKED)
        // Implement make_integer_sequence and make_index_sequence with the
        // __make_integer_seq builtin on compilers that provide it. (Redirect
        // through decltype to workaround suspected clang bug.)
        /// \cond
        namespace detail
        {
            template <class T, T N>
            __make_integer_seq<integer_sequence, T, N> make_integer_sequence_();
        }
        /// \endcond

        template <typename T, T N>
        using make_integer_sequence = decltype(detail::make_integer_sequence_<T, N>());

        template <std::size_t N>
        using make_index_sequence = make_integer_sequence<std::size_t, N>;
#else
        /// Generate \c index_sequence containing integer constants [0,1,2,...,N-1].
        /// \par Complexity
        /// \f$ O(log(N)) \f$.
        /// \ingroup integral
        template <std::size_t N>
        using make_index_sequence =
            _t<detail::make_indices_<N, index_sequence<0>, detail::strategy_(1, N)>>;

        /// Generate \c integer_sequence containing integer constants [0,1,2,...,N-1].
        /// \par Complexity
        /// \f$ O(log(N)) \f$.
        /// \ingroup integral
        template <typename T, T N>
        using make_integer_sequence =
            _t<detail::coerce_indices_<T, 0, make_index_sequence<static_cast<std::size_t>(N)>>>;
#endif

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // integer_range
        /// Makes the integer sequence <tt>[From, To)</tt>.
        /// \par Complexity
        /// \f$ O(log(To - From)) \f$.
        /// \ingroup integral
        template <class T, T From, T To>
        using integer_range =
            _t<detail::coerce_indices_<T, From,
                                       make_index_sequence<detail::range_distance_(From, To)>>>;

        /// \cond
        namespace detail
        {
            template <typename, typename>
            struct concat_indices_
            {
            };

            template <std::size_t... Is, std::size_t... Js>
            struct concat_indices_<index_sequence<Is...>, index_sequence<Js...>>
            {
                using type = index_sequence<Is..., (Js + sizeof...(Is))...>;
            };

            template <>
            struct make_indices_<0u, index_sequence<0>, indices_strategy_::done>
            {
                using type = index_sequence<>;
            };

            template <std::size_t End, std::size_t... Values>
            struct make_indices_<End, index_sequence<Values...>, indices_strategy_::repeat>
                : make_indices_<End, index_sequence<Values..., (Values + sizeof...(Values))...>,
                                detail::strategy_(sizeof...(Values)*2, End)>
            {
            };

            template <std::size_t End, std::size_t... Values>
            struct make_indices_<End, index_sequence<Values...>, indices_strategy_::recurse>
                : concat_indices_<index_sequence<Values...>,
                                  make_index_sequence<End - sizeof...(Values)>>
            {
            };

            template <typename T, T Offset, std::size_t... Values>
            struct coerce_indices_<T, Offset, index_sequence<Values...>>
            {
                using type =
                    integer_sequence<T, static_cast<T>(static_cast<T>(Values) + Offset)...>;
            };
        } // namespace detail
        /// \endcond

        /// Evaluate the Callable \p F with the arguments \p Args.
        /// \ingroup invocation
        template <typename F, typename... Args>
        using invoke = typename F::template invoke<Args...>;

        /// Lazy versions of meta actions
        namespace lazy
        {
            /// \sa `meta::invoke`
            /// \ingroup lazy_invocation
            template <typename F, typename... Args>
            using invoke = defer<invoke, F, Args...>;
        }

        /// A trait that always returns its argument \p T. Also, a Callable that always
        /// returns
        /// \p T.
        /// \ingroup trait
        /// \ingroup invocation
        template <typename T>
        struct id
        {
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5 && !defined(META_DOXYGEN_INVOKED)
            // Redirect through decltype for compilers that have not
            // yet implemented CWG 1558:
            // <http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1558>
            static id impl(void *);

            template <typename... Ts>
            using invoke = _t<decltype(id::impl(static_cast<list<Ts...> *>(nullptr)))>;
#else
            template <typename...>
            using invoke = T;
#endif

            using type = T;
        };

        /// An alias for type \p T. Useful in non-deduced contexts.
        /// \ingroup trait
        template <typename T>
        using id_t = _t<id<T>>;

        namespace lazy
        {
            /// \sa `meta::id`
            /// \ingroup lazy_trait
            /// \ingroup lazy_invocation
            template <typename T>
            using id = defer<id, T>;
        }

        /// An alias for `void`.
        /// \ingroup trait
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 5 && !defined(META_DOXYGEN_INVOKED)
        // Redirect through decltype for compilers that have not
        // yet implemented CWG 1558:
        // <http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1558>
        template <typename... Ts>
        using void_ = invoke<id<void>, Ts...>;
#else
        template <typename...>
        using void_ = void;
#endif

        /// \cond
        namespace detail
        {
            template <typename, typename = void>
            struct is_trait_
            {
                using type = std::false_type;
            };

            template <typename T>
            struct is_trait_<T, void_<typename T::type>>
            {
                using type = std::true_type;
            };

            template <typename, typename = void>
            struct is_callable_
            {
                using type = std::false_type;
            };

            template <typename T>
            struct is_callable_<T, void_<quote<T::template invoke>>>
            {
                using type = std::true_type;
            };

            template <template <typename...> class C, typename... Ts,
                template <typename...> class D = C>
            id<D<Ts...>> try_defer_(int);
            template <template <typename...> class C, typename... Ts>
            nil_ try_defer_(long);

            template <template <typename...> class C, typename... Ts>
            using defer_ = decltype(detail::try_defer_<C, Ts...>(0));

            template <typename T, template <T...> class C, T... Is,
                template <T...> class D = C>
            id<D<Is...>> try_defer_i_(int);
            template <typename T, template <T...> class C, T... Is>
            nil_ try_defer_i_(long);

            template <typename T, template <T...> class C, T... Is>
            using defer_i_ = decltype(detail::try_defer_i_<T, C, Is...>(0));

            template <typename T>
            using _t_t = _t<_t<T>>;
        } // namespace detail
        /// \endcond

        /// An alias for `std::true_type` if `T::type` exists and names a type;
        /// otherwise, it's an alias for `std::false_type`.
        /// \ingroup trait
        template <typename T>
        using is_trait = _t<detail::is_trait_<T>>;

        /// An alias for `std::true_type` if `T::invoke` exists and names a class
        /// template or alias template; otherwise, it's an alias for
        /// `std::false_type`.
        /// \ingroup trait
        template <typename T>
        using is_callable = _t<detail::is_callable_<T>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer
        /// A wrapper that defers the instantiation of a template \p C with type
        /// parameters \p Ts in a \c lambda or \c let expression.
        ///
        /// In the code below, the lambda would ideally be written as
        /// `lambda<_a,_b,push_back<_a,_b>>`, however this fails since `push_back`
        /// expects its first argument to be a list, not a placeholder. Instead,
        /// we express it using \c defer as follows:
        ///
        /// \code
        /// template <typename List>
        /// using reverse = reverse_fold<List, list<>, lambda<_a, _b, defer<push_back,
        /// _a, _b>>>;
        /// \endcode
        ///
        /// \ingroup invocation
        template <template <typename...> class C, typename... Ts>
        struct defer : detail::defer_<C, Ts...>
        {
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_i
        /// A wrapper that defers the instantiation of a template \p C with integral
        /// constant parameters \p Is in a \c lambda or \c let expression.
        /// \sa `defer`
        /// \ingroup invocation
        template <typename T, template <T...> class C, T... Is>
        struct defer_i : detail::defer_i_<T, C, Is...>
        {
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_trait
        /// A wrapper that defers the instantiation of a trait \p C with type parameters
        /// \p Ts in a
        /// \c lambda or \c let expression.
        /// \sa `defer`
        /// \ingroup invocation
        template <template <typename...> class C, typename... Ts>
        using defer_trait = defer<detail::_t_t, detail::defer_<C, Ts...>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_trait_i
        /// A wrapper that defers the instantiation of a trait \p C with integral
        /// constant
        /// parameters \p Is in a \c lambda or \c let expression.
        /// \sa `defer_i`
        /// \ingroup invocation
        template <typename T, template <T...> class C, T... Is>
        using defer_trait_i =
            defer<detail::_t_t, detail::defer_i_<T, C, Is...>>;

        /// An alias that computes the size of the type \p T.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup trait
        template <class T>
        using sizeof_ = meta::size_t<sizeof(T)>;

        /// An alias that computes the alignment required for any instance of the type
        /// \p T.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup trait
        template <class T>
        using alignof_ = meta::size_t<alignof(T)>;

        namespace lazy
        {
            /// \sa `meta::sizeof_`
            /// \ingroup lazy_trait
            template <typename T>
            using sizeof_ = defer<sizeof_, T>;

            /// \sa `meta::alignof_`
            /// \ingroup lazy_trait
            template <typename T>
            using alignof_ = defer<alignof_, T>;
        }

        /// is
        /// \cond
        namespace detail
        {
            template <typename, template <typename...> class>
            struct is_ : std::false_type
            {
            };

            template <typename... Ts, template <typename...> class C>
            struct is_<C<Ts...>, C> : std::true_type
            {
            };
        }
        /// \endcond

        /// Test whether a type \c T is an instantiation of class
        /// template \c C.
        /// \ingroup trait
        template <typename T, template <typename...> class C>
        using is = _t<detail::is_<T, C>>;

        /// Compose the Callables \p Fs in the parameter pack \p Ts.
        /// \ingroup composition
        template <typename... Fs>
        struct compose
        {
        };

        template <typename F0>
        struct compose<F0>
        {
            template <typename... Ts>
            using invoke = invoke<F0, Ts...>;
        };

        template <typename F0, typename... Fs>
        struct compose<F0, Fs...>
        {
            template <typename... Ts>
            using invoke = invoke<F0, invoke<compose<Fs...>, Ts...>>;
        };

        namespace lazy
        {
            /// \sa 'meta::compose'
            /// \ingroup lazy_composition
            template <typename... Fns>
            using compose = defer<compose, Fns...>;
        }

        /// Turn a class template or alias template \p C into a Callable.
        /// \ingroup composition
        template <template <typename...> class C>
        struct quote
        {
            // Indirection through defer here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <typename... Ts>
            using invoke = _t<detail::defer_<C, Ts...>>;
        };

        /// Turn a class template or alias template \p C taking literals of type \p T
        /// into a Callable.
        /// \ingroup composition
        template <typename T, template <T...> class C>
        struct quote_i
        {
            // Indirection through defer_i here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <typename... Ts>
            using invoke = _t<detail::defer_i_<T, C, Ts::type::value...>>;
        };

#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 8 && !defined(META_DOXYGEN_INVOKED)
        template <template <typename...> class C>
        struct quote_trait
        {
            template <typename... Ts>
            using invoke = _t<invoke<quote<C>, Ts...>>;
        };

        template <typename T, template <T...> class C>
        struct quote_trait_i
        {
            template <typename... Ts>
            using invoke = _t<invoke<quote_i<T, C>, Ts...>>;
        };
#else
        // clang-format off
        /// Turn a trait template \p C into a Callable.
        /// \code
        /// static_assert(std::is_same<invoke<quote_trait<std::add_const>, int>, int const>::value, "");
        /// \endcode
        /// \ingroup composition
        template <template <typename...> class C>
        using quote_trait = compose<quote<_t>, quote<C>>;

        /// Turn a trait \p C taking literals of type \p T into a Callable.
        /// \sa 'meta::quote_trait'
        /// \ingroup composition
        template <typename T, template <T...> class C>
        using quote_trait_i = compose<quote<_t>, quote_i<T, C>>;
// clang-format on
#endif

        /// A Callable that partially applies the Callable
        /// \p F by binding the arguments \p Ts to the \e front of \p F.
        /// \ingroup composition
        template <typename F, typename... Ts>
        struct bind_front
        {
            template <typename... Us>
            using invoke = invoke<F, Ts..., Us...>;
        };

        /// A Callable that partially applies the Callable \p F by binding the
        /// arguments \p Us to the \e back of \p F.
        /// \ingroup composition
        template <typename F, typename... Us>
        struct bind_back
        {
            template <typename... Ts>
            using invoke = invoke<F, Ts..., Us...>;
        };

        namespace lazy
        {
            /// \sa 'meta::bind_front'
            /// \ingroup lazy_composition
            template <typename Fn, typename... Ts>
            using bind_front = defer<bind_front, Fn, Ts...>;

            /// \sa 'meta::bind_back'
            /// \ingroup lazy_composition
            template <typename Fn, typename... Ts>
            using bind_back = defer<bind_back, Fn, Ts...>;
        }

        /// Extend meta with your own datatypes.
        namespace extension
        {
            /// A trait that unpacks the types in the type list \p List into the Callable
            /// \p F.
            /// \ingroup extension
            template <typename F, typename List>
            struct apply
            {
            };

            template <typename F, typename Ret, typename... Args>
            struct apply<F, Ret(Args...)> : lazy::invoke<F, Ret, Args...>
            {
            };

            template <typename F, template <typename...> class T, typename... Ts>
            struct apply<F, T<Ts...>> : lazy::invoke<F, Ts...>
            {
            };

            template <typename F, typename T, T... Is>
            struct apply<F, integer_sequence<T, Is...>>
                : lazy::invoke<F, std::integral_constant<T, Is>...>
            {
            };
        }

        /// Applies the Callable \p C using the types in the type list \p List as
        /// arguments.
        /// \ingroup invocation
        template <typename C, typename List>
        using apply = _t<extension::apply<C, List>>;

        namespace lazy
        {
            template <typename F, typename List>
            using apply = defer<apply, F, List>;
        }

        /// A Callable that takes a bunch of arguments, bundles them into a type list,
        /// and
        /// then calls the Callable \p F with the type list \p Q.
        /// \ingroup composition
        template <typename F, typename Q = quote<list>>
        using curry = compose<F, Q>;

        /// A Callable that takes a type list, unpacks the types, and then calls the
        /// Callable \p F with the types.
        /// \ingroup composition
        template <typename F>
        using uncurry = bind_front<quote<apply>, F>;

        namespace lazy
        {
            /// \sa 'meta::curry'
            /// \ingroup lazy_composition
            template <typename F, typename Q = quote<list>>
            using curry = defer<curry, F, Q>;

            /// \sa 'meta::uncurry'
            /// \ingroup lazy_composition
            template <typename F>
            using uncurry = defer<uncurry, F>;
        }

        /// A Callable that reverses the order of the first two arguments.
        /// \ingroup composition
        template <typename F>
        struct flip
        {
        private:
            template <typename... Ts>
            struct impl
            {
            };
            template <typename A, typename B, typename... Ts>
            struct impl<A, B, Ts...> : lazy::invoke<F, B, A, Ts...>
            {
            };

        public:
            template <typename... Ts>
            using invoke = _t<impl<Ts...>>;
        };

        namespace lazy
        {
            /// \sa 'meta::flip'
            /// \ingroup lazy_composition
            template <typename F>
            using flip = defer<flip, F>;
        }

        /// \cond
        namespace detail
        {
            template <typename...>
            struct on_
            {
            };
            template <typename F, typename... Gs>
            struct on_<F, Gs...>
            {
                template <typename... Ts>
                using invoke = invoke<F, invoke<compose<Gs...>, Ts>...>;
            };
        }
        /// \endcond

        /// Use as `on<F, Gs...>`. Creates an Callable that applies Callable \c F to the
        /// result of applying Callable `compose<Gs...>` to all the arguments.
        /// \ingroup composition
        template <typename... Fs>
        using on = detail::on_<Fs...>;

        namespace lazy
        {
            /// \sa 'meta::on'
            /// \ingroup lazy_composition
            template <typename F, typename G>
            using on = defer<on, F, G>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // if_
        /// \cond
        namespace detail
        {
        #ifdef __clang__
            // Clang is faster with this implementation
            template <typename, typename = bool>
            struct _if_
            {
            };

            template <typename If>
            struct _if_<list<If>, decltype(bool(If::type::value))> : std::enable_if<If::type::value>
            {
            };

            template <typename If, typename Then>
            struct _if_<list<If, Then>, decltype(bool(If::type::value))>
                : std::enable_if<If::type::value, Then>
            {
            };

            template <typename If, typename Then, typename Else>
            struct _if_<list<If, Then, Else>, decltype(bool(If::type::value))>
                : std::conditional<If::type::value, Then, Else>
            {
            };
        #else
            // GCC seems to prefer this implementation
            template <typename, typename = std::true_type>
            struct _if_
            {
            };

            template <typename If>
            struct _if_<list<If>, bool_<If::type::value>>
            {
                using type = void;
            };

            template <typename If, typename Then>
            struct _if_<list<If, Then>, bool_<If::type::value>>
            {
                using type = Then;
            };

            template <typename If, typename Then, typename Else>
            struct _if_<list<If, Then, Else>, bool_<If::type::value>>
            {
                using type = Then;
            };

            template <typename If, typename Then, typename Else>
            struct _if_<list<If, Then, Else>, bool_<!If::type::value>>
            {
                using type = Else;
            };
        #endif
        } // namespace detail
        /// \endcond

        /// Select one type or another depending on a compile-time Boolean.
        /// \ingroup logical
        template <typename... Args>
        using if_ = _t<detail::_if_<list<Args...>>>;

        /// Select one type or another depending on a compile-time Boolean.
        /// \ingroup logical
        template <bool If, typename... Args>
        using if_c = _t<detail::_if_<list<bool_<If>, Args...>>>;

        namespace lazy
        {
            /// \sa 'meta::if_'
            /// \ingroup lazy_logical
            template <typename... Args>
            using if_ = defer<if_, Args...>;

            /// \sa 'meta::if_c'
            /// \ingroup lazy_logical
            template <bool If, typename... Args>
            using if_c = if_<bool_<If>, Args...>;
        }

        /// \cond
        namespace detail
        {
            template <bool>
            struct _and_
            {
                template <class...>
                using invoke = std::true_type;
            };

            template <>
            struct _and_<false>
            {
                template <typename Bool, typename... Bools>
                using invoke =
                    invoke<
                        if_c<
                            !Bool::type::value,
                            id<std::false_type>,
                            _and_<0 == sizeof...(Bools)>>,
                        Bools...>;
            };

            template <bool>
            struct _or_
            {
                template <class = void>
                using invoke = std::false_type;
            };

            template <>
            struct _or_<false>
            {
                template <typename Bool_, typename... Bools>
                using invoke =
                    invoke<
                        if_c<
                            Bool_::type::value,
                            id<std::true_type>,
                            _or_<0 == sizeof...(Bools)>>,
                        Bools...>;
            };
        } // namespace detail
        /// \endcond

        /// Logically negate the Boolean parameter
        /// \ingroup logical
        template <bool Bool_>
        using not_c = bool_<!Bool_>;

        /// Logically negate the integral constant-wrapped Boolean parameter.
        /// \ingroup logical
        template <typename Bool_>
        using not_ = not_c<Bool_::type::value>;

/// Logically and together all the Boolean parameters
/// \ingroup logical
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 5 && __GNUC_MINOR__ == 1
        // Alternative formulation of and_c to workaround
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66405
        template <bool... Bools>
        using and_c = std::is_same<integer_sequence<bool, true, Bools...>,
                                   integer_sequence<bool, Bools..., true>>;
#else
        template <bool... Bools>
        using and_c = std::is_same<integer_sequence<bool, Bools...>,
                                   integer_sequence<bool, (Bools || true)...>>;
#endif

        /// Logically and together all the integral constant-wrapped Boolean parameters,
        /// \e without
        /// doing short-circuiting.
        /// \ingroup logical
        template <typename... Bools>
        using strict_and = and_c<Bools::type::value...>;

        /// Logically and together all the integral constant-wrapped Boolean parameters,
        /// \e with
        /// short-circuiting.
        /// \ingroup logical
        // Make a trip through defer<> to avoid CWG1430
        template <typename... Bools>
        using and_ = _t<defer<detail::_and_<0 == sizeof...(Bools)>::template invoke, Bools...>>;

        /// Logically or together all the Boolean parameters
        /// \ingroup logical
        template <bool... Bools>
        using or_c = not_<std::is_same<integer_sequence<bool, Bools...>,
                                       integer_sequence<bool, (Bools && false)...>>>;

        /// Logically or together all the integral constant-wrapped Boolean parameters,
        /// \e without
        /// doing short-circuiting.
        /// \ingroup logical
        template <typename... Bools>
        using strict_or = or_c<Bools::type::value...>;

        /// Logically or together all the integral constant-wrapped Boolean parameters,
        /// \e with
        /// short-circuiting.
        /// \ingroup logical
        // Make a trip through defer<> to avoid CWG1430
        template <typename... Bools>
        using or_ = _t<defer<detail::_or_<0 == sizeof...(Bools)>:: template invoke, Bools...>>;

        namespace lazy
        {
            /// \sa 'meta::and_'
            /// \ingroup lazy_logical
            template <typename... Bools>
            using and_ = defer<and_, Bools...>;

            /// \sa 'meta::or_'
            /// \ingroup lazy_logical
            template <typename... Bools>
            using or_ = defer<or_, Bools...>;

            /// \sa 'meta::not_'
            /// \ingroup lazy_logical
            template <typename Bool_>
            using not_ = defer<not_, Bool_>;

            /// \sa 'meta::strict_and'
            /// \ingroup lazy_logical
            template <typename... Bools>
            using strict_and = defer<strict_and, Bools...>;

            /// \sa 'meta::strict_or'
            /// \ingroup lazy_logical
            template <typename... Bools>
            using strict_or = defer<strict_or, Bools...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // fold
        /// \cond
        namespace detail
        {
            template <typename Fun, typename T0>
            struct compose1_
            {
                template <typename X>
                using invoke = invoke<Fun, _t<X>, T0>;
            };

            template <typename Fun, typename T0, typename T1, typename T2, typename T3, typename T4,
                      typename T5, typename T6, typename T7, typename T8, typename T9>
            struct compose10_
            {
                template <typename X, typename Y>
                using F = invoke<Fun, X, Y>;

                template <typename S>
                using invoke =
                    F<F<F<F<F<F<F<F<F<F<_t<S>, T0>, T1>, T2>, T3>, T4>, T5>, T6>, T7>, T8>, T9>;
            };

            template <typename, typename, typename>
            struct fold_
            {
            };

            template <typename State, typename Fun>
            struct fold_<list<>, State, Fun> : State
            {
            };

            template <typename Head, typename... List, typename State, typename Fun>
            struct fold_<list<Head, List...>, State, Fun>
                : fold_<list<List...>, lazy::invoke<compose1_<Fun, Head>, State>, Fun>
            {
            };

            template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
                      typename T6, typename T7, typename T8, typename T9, typename... List,
                      typename State, typename Fun>
            struct fold_<list<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, List...>, State, Fun>
                : fold_<
                      list<List...>,
                      lazy::invoke<compose10_<Fun, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>, State>,
                      Fun>
            {
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list constructed by doing a left fold of the list \p List using
        /// binary Callable \p Fun and initial state \p State. That is, the \c State_N for the list
        /// element \c A_N is computed by `Fun(State_N-1, A_N) -> State_N`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename State, typename Fun>
        using fold = _t<detail::fold_<List, id<State>, Fun>>;

        /// An alias for `meta::fold`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename State, typename Fun>
        using accumulate = fold<List, State, Fun>;

        namespace lazy
        {
            /// \sa 'meta::foldl'
            /// \ingroup lazy_transformation
            template <typename List, typename State, typename Fun>
            using fold = defer<fold, List, State, Fun>;

            /// \sa 'meta::accumulate'
            /// \ingroup lazy_transformation
            template <typename List, typename State, typename Fun>
            using accumulate = defer<accumulate, List, State, Fun>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse_fold
        /// \cond
        namespace detail
        {
            template <typename List, typename State, typename Fun>
            struct reverse_fold_
            {
            };

            template <typename State, typename Fun>
            struct reverse_fold_<list<>, State, Fun>
            {
                using type = State;
            };

            template <typename Head, typename... List, typename State, typename Fun>
            struct reverse_fold_<list<Head, List...>, State, Fun>
                : lazy::invoke<compose1_<Fun, Head>, reverse_fold_<list<List...>, State, Fun>>
            {
            };

            template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
                      typename T6, typename T7, typename T8, typename T9, typename... List,
                      typename State, typename Fun>
            struct reverse_fold_<list<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, List...>, State, Fun>
                : lazy::invoke<compose10_<Fun, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0>,
                               reverse_fold_<list<List...>, State, Fun>>
            {
            };
        }
        /// \endcond

        /// Return a new \c meta::list constructed by doing a right fold of the list \p List using
        /// binary Callable \p Fun and initial state \p State. That is, the \c State_N
        /// for the list element \c A_N is computed by `Fun(A_N, State_N+1) -> State_N`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename State, typename Fun>
        using reverse_fold = _t<detail::reverse_fold_<List, State, Fun>>;

        namespace lazy
        {
            /// \sa 'meta::foldr'
            /// \ingroup lazy_transformation
            template <typename List, typename State, typename Fun>
            using reverse_fold = defer<reverse_fold, List, State, Fun>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // npos
        /// A special value used to indicate no matches. It equals the maximum value
        /// representable
        /// by std::size_t.
        /// \ingroup list
        using npos = meta::size_t<std::size_t(-1)>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // list
        /// A list of types.
        /// \ingroup list
        template <typename... Ts>
        struct list
        {
            using type = list;
            /// \return `sizeof...(Ts)`
            static constexpr std::size_t size() noexcept { return sizeof...(Ts); }
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // size
        /// An integral constant wrapper that is the size of the \c meta::list
        /// \p List.
        /// \ingroup list
        template <typename List>
        using size = meta::size_t<List::size()>;

        namespace lazy
        {
            /// \sa 'meta::size'
            /// \ingroup lazy_list
            template <typename List>
            using size = defer<size, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // concat
        /// \cond
        namespace detail
        {
            template <typename... Lists>
            struct concat_
            {
            };

            template <>
            struct concat_<>
            {
                using type = list<>;
            };

            template <typename... List1>
            struct concat_<list<List1...>>
            {
                using type = list<List1...>;
            };

            template <typename... List1, typename... List2, typename... Rest>
            struct concat_<list<List1...>, list<List2...>, Rest...>
                : concat_<list<List1..., List2...>, Rest...>
            {
            };

            template <typename... List1, typename... List2, typename... List3, typename... List4,
                      typename... List5, typename... List6, typename... List7, typename... List8,
                      typename... List9, typename... List10, typename... Rest>
            struct concat_<list<List1...>, list<List2...>, list<List3...>, list<List4...>,
                           list<List5...>, list<List6...>, list<List7...>, list<List8...>,
                           list<List9...>, list<List10...>, Rest...>
                : concat_<list<List1..., List2..., List3..., List4..., List5..., List6..., List7...,
                               List8..., List9..., List10...>,
                          Rest...>
            {
            };
        } // namespace detail
        /// \endcond

        /// Concatenates several lists into a single list.
        /// \pre The parameters must all be instantiations of \c meta::list.
        /// \par Complexity
        /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of lists.
        /// \ingroup transformation
        template <typename... Lists>
        using concat = _t<detail::concat_<Lists...>>;

        namespace lazy
        {
            /// \sa 'meta::concat'
            /// \ingroup lazy_transformation
            template <typename... Lists>
            using concat = defer<concat, Lists...>;
        }

        /// Joins a list of lists into a single list.
        /// \pre The parameter must be an instantiation of \c meta::list\<T...\>
        /// where each \c T is itself an instantiation of \c meta::list.
        /// \par Complexity
        /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of
        /// lists.
        /// \ingroup transformation
        template <typename ListOfLists>
        using join = apply<quote<concat>, ListOfLists>;

        namespace lazy
        {
            /// \sa 'meta::join'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using join = defer<join, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // transform
        /// \cond
        namespace detail
        {
            template <typename, typename = void>
            struct transform_
            {
            };

            template <typename... Ts, typename Fun>
            struct transform_<list<list<Ts...>, Fun>, void_<invoke<Fun, Ts>...>>
            {
                using type = list<invoke<Fun, Ts>...>;
            };

            template <typename... Ts0, typename... Ts1, typename Fun>
            struct transform_<list<list<Ts0...>, list<Ts1...>, Fun>,
                              void_<invoke<Fun, Ts0, Ts1>...>>
            {
                using type = list<invoke<Fun, Ts0, Ts1>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list constructed by transforming all the elements in
        /// \p List with
        /// the unary Callable \p Fun. \c transform can also be called with two lists of
        /// the same length and a binary Callable, in which case it returns a new list
        /// constructed with the results of calling \c Fun with each element in the
        /// lists,
        /// pairwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename... Args>
        using transform = _t<detail::transform_<list<Args...>>>;

        namespace lazy
        {
            /// \sa 'meta::transform'
            /// \ingroup lazy_transformation
            template <typename... Args>
            using transform = defer<transform, Args...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // repeat_n
        /// \cond
        namespace detail
        {
            template <typename T, std::size_t>
            using first_ = T;

            template <typename T, typename Ints>
            struct repeat_n_c_
            {
            };

            template <typename T, std::size_t... Is>
            struct repeat_n_c_<T, index_sequence<Is...>>
            {
                using type = list<first_<T, Is>...>;
            };
        }
        /// \endcond

        /// Generate `list<T,T,T...T>` of size \p N arguments.
        /// \par Complexity
        /// \f$ O(log N) \f$.
        /// \ingroup list
        template <std::size_t N, typename T = void>
        using repeat_n_c = _t<detail::repeat_n_c_<T, make_index_sequence<N>>>;

        /// Generate `list<T,T,T...T>` of size \p N arguments.
        /// \par Complexity
        /// \f$ O(log N) \f$.
        /// \ingroup list
        template <typename N, typename T = void>
        using repeat_n = repeat_n_c<N::type::value, T>;

        namespace lazy
        {
            /// \sa 'meta::repeat_n'
            /// \ingroup lazy_list
            template <typename N, typename T = void>
            using repeat_n = defer<repeat_n, N, T>;

            /// \sa 'meta::repeat_n_c'
            /// \ingroup lazy_list
            template <std::size_t N, typename T = void>
            using repeat_n_c = defer<repeat_n, meta::size_t<N>, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // at
        /// \cond
        namespace detail
        {
#if META_HAS_TYPE_PACK_ELEMENT && !defined(META_DOXYGEN_INVOKED)
            template <typename List, std::size_t N, typename = void>
            struct at_
            {
            };

            template <typename... Ts, std::size_t N>
            struct at_<list<Ts...>, N, void_<__type_pack_element<N, Ts...>>>
            {
                using type = __type_pack_element<N, Ts...>;
            };
#else
            template <typename VoidPtrs>
            struct at_impl_;

            template <typename... VoidPtrs>
            struct at_impl_<list<VoidPtrs...>>
            {
                static nil_ eval(...);

                template <typename T, typename... Us>
                static T eval(VoidPtrs..., T *, Us *...);
            };

            template <typename List, std::size_t N>
            struct at_
            {
            };

            template <typename... Ts, std::size_t N>
            struct at_<list<Ts...>, N>
                : decltype(at_impl_<repeat_n_c<N, void *>>::eval(static_cast<id<Ts> *>(nullptr)...))
            {
            };
#endif // META_HAS_TYPE_PACK_ELEMENT
        } // namespace detail
        /// \endcond

        /// Return the \p N th element in the \c meta::list \p List.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <typename List, std::size_t N>
        using at_c = _t<detail::at_<List, N>>;

        /// Return the \p N th element in the \c meta::list \p List.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <typename List, typename N>
        using at = at_c<List, N::type::value>;

        namespace lazy
        {
            /// \sa 'meta::at'
            /// \ingroup lazy_list
            template <typename List, typename N>
            using at = defer<at, List, N>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // drop
        /// \cond
        namespace detail
        {
            ///////////////////////////////////////////////////////////////////////////////////////
            /// drop_impl_
            template <typename VoidPtrs>
            struct drop_impl_
            {
                static nil_ eval(...);
            };

            template <typename... VoidPtrs>
            struct drop_impl_<list<VoidPtrs...>>
            {
                static nil_ eval(...);

                template <typename... Ts>
                static id<list<Ts...>> eval(VoidPtrs..., id<Ts> *...);
            };

            template <>
            struct drop_impl_<list<>>
            {
                template <typename... Ts>
                static id<list<Ts...>> eval(id<Ts> *...);
            };

            template <typename List, typename N>
            struct drop_
            {
            };

            template <typename... Ts, typename N>
            struct drop_<list<Ts...>, N>
                : decltype(drop_impl_<repeat_n<N, void *>>::eval(detail::_nullptr_v<id<Ts>>()...))
            {
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by removing the first \p N elements from \p List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <typename List, typename N>
        using drop = _t<detail::drop_<List, N>>;

        /// Return a new \c meta::list by removing the first \p N elements from \p List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <typename List, std::size_t N>
        using drop_c = _t<detail::drop_<List, meta::size_t<N>>>;

        namespace lazy
        {
            /// \sa 'meta::drop'
            /// \ingroup lazy_transformation
            template <typename List, typename N>
            using drop = defer<drop, List, N>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // front
        /// \cond
        namespace detail
        {
            template <typename List>
            struct front_
            {
            };

            template <typename Head, typename... List>
            struct front_<list<Head, List...>>
            {
                using type = Head;
            };
        } // namespace detail
        /// \endcond

        /// Return the first element in \c meta::list \p List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup list
        template <typename List>
        using front = _t<detail::front_<List>>;

        namespace lazy
        {
            /// \sa 'meta::front'
            /// \ingroup lazy_list
            template <typename List>
            using front = defer<front, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // back
        /// \cond
        namespace detail
        {
            template <typename List>
            struct back_
            {
            };

            template <typename Head, typename... List>
            struct back_<list<Head, List...>>
            {
                using type = at_c<list<Head, List...>, sizeof...(List)>;
            };
        } // namespace detail
        /// \endcond

        /// Return the last element in \c meta::list \p List.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <typename List>
        using back = _t<detail::back_<List>>;

        namespace lazy
        {
            /// \sa 'meta::back'
            /// \ingroup lazy_list
            template <typename List>
            using back = defer<back, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // push_front
        /// \cond
        namespace detail
        {
            template <typename List, typename T>
            struct push_front_
            {
            };

            template <typename... List, typename T>
            struct push_front_<list<List...>, T>
            {
                using type = list<T, List...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by adding the element \c T to the front of \p
        /// List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <typename List, typename T>
        using push_front = _t<detail::push_front_<List, T>>;

        namespace lazy
        {
            /// \sa 'meta::push_front'
            /// \ingroup lazy_transformation
            template <typename List, typename T>
            using push_front = defer<push_front, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // pop_front
        /// \cond
        namespace detail
        {
            template <typename List>
            struct pop_front_
            {
            };

            template <typename Head, typename... List>
            struct pop_front_<list<Head, List...>>
            {
                using type = list<List...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by removing the first element from the front of
        /// \p List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <typename List>
        using pop_front = _t<detail::pop_front_<List>>;

        namespace lazy
        {
            /// \sa 'meta::pop_front'
            /// \ingroup lazy_transformation
            template <typename List>
            using pop_front = defer<pop_front, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // push_back
        /// \cond
        namespace detail
        {
            template <typename List, typename T>
            struct push_back_
            {
            };

            template <typename... List, typename T>
            struct push_back_<list<List...>, T>
            {
                using type = list<List..., T>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by adding the element \c T to the back of \p
        /// List.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \note \c pop_back not provided because it cannot be made to meet the
        /// complexity guarantees one would expect.
        /// \ingroup transformation
        template <typename List, typename T>
        using push_back = _t<detail::push_back_<List, T>>;

        namespace lazy
        {
            /// \sa 'meta::push_back'
            /// \ingroup lazy_transformation
            template <typename List, typename T>
            using push_back = defer<push_back, List, T>;
        }

        /// \cond
        namespace detail
        {
            template <typename T, typename U>
            using min_ = if_<less<U, T>, U, T>;

            template <typename T, typename U>
            using max_ = if_<less<U, T>, T, U>;
        }
        /// \endcond

        /// An integral constant wrapper around the minimum of `Ts::type::value...`
        /// \ingroup math
        template <typename... Ts>
        using min = fold<pop_front<list<Ts...>>, front<list<Ts...>>, quote<detail::min_>>;

        /// An integral constant wrapper around the maximum of `Ts::type::value...`
        /// \ingroup math
        template <typename... Ts>
        using max = fold<pop_front<list<Ts...>>, front<list<Ts...>>, quote<detail::max_>>;

        namespace lazy
        {
            /// \sa 'meta::min'
            /// \ingroup lazy_math
            template <typename... Ts>
            using min = defer<min, Ts...>;

            /// \sa 'meta::max'
            /// \ingroup lazy_math
            template <typename... Ts>
            using max = defer<max, Ts...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // empty
        /// An Boolean integral constant wrapper around \c true if \p List is an empty
        /// type list; \c
        /// false, otherwise.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup list
        template <typename List>
        using empty = bool_<0 == size<List>::type::value>;

        namespace lazy
        {
            /// \sa 'meta::empty'
            /// \ingroup lazy_list
            template <typename List>
            using empty = defer<empty, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // pair
        /// A list with exactly two elements
        /// \ingroup list
        template <typename F, typename S>
        using pair = list<F, S>;

        /// Retrieve the first element of the \c pair \p Pair
        /// \ingroup list
        template <typename Pair>
        using first = front<Pair>;

        /// Retrieve the first element of the \c pair \p Pair
        /// \ingroup list
        template <typename Pair>
        using second = front<pop_front<Pair>>;

        namespace lazy
        {
            /// \sa 'meta::first'
            /// \ingroup lazy_list
            template <typename Pair>
            using first = defer<first, Pair>;

            /// \sa 'meta::second'
            /// \ingroup lazy_list
            template <typename Pair>
            using second = defer<second, Pair>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // find_index
        /// \cond
        namespace detail
        {
            // With thanks to Peter Dimov:
            constexpr std::size_t find_index_i_(bool const *const first, bool const *const last,
                                                std::size_t N = 0)
            {
                return first == last ? npos::value
                                     : *first ? N : find_index_i_(first + 1, last, N + 1);
            }

            template <typename List, typename T>
            struct find_index_
            {
            };

            template <typename V>
            struct find_index_<list<>, V>
            {
                using type = npos;
            };

            template <typename... T, typename V>
            struct find_index_<list<T...>, V>
            {
#if (defined(__clang__) && __clang_major__ < 6) || defined(__apple_build_version__)
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(T)] = {std::is_same<T, V>::value...};
#else
                static constexpr bool s_v[] = {std::is_same<T, V>::value...};
#endif
                using type = size_t<find_index_i_(s_v, s_v + sizeof...(T))>;
            };
        } // namespace detail
        /// \endcond

        /// Finds the index of the first occurrence of the type \p T within the list \p
        /// List.
        /// Returns `#meta::npos` if the type \p T was not found.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        /// \sa `meta::npos`
        template <typename List, typename T>
        using find_index = _t<detail::find_index_<List, T>>;

        namespace lazy
        {
            /// \sa 'meta::find_index'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using find_index = defer<find_index, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse_find_index
        /// \cond
        namespace detail
        {
            // With thanks to Peter Dimov:
            constexpr std::size_t reverse_find_index_i_(bool const *const first,
                                                        bool const *const last, std::size_t N)
            {
                return first == last
                           ? npos::value
                           : *(last - 1) ? N - 1 : reverse_find_index_i_(first, last - 1, N - 1);
            }

            template <typename List, typename T>
            struct reverse_find_index_
            {
            };

            template <typename V>
            struct reverse_find_index_<list<>, V>
            {
                using type = npos;
            };

            template <typename... T, typename V>
            struct reverse_find_index_<list<T...>, V>
            {
#if (defined(__clang__) && __clang_major__ < 6) || defined(__apple_build_version__)
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(T)] = {std::is_same<T, V>::value...};
#else
                static constexpr bool s_v[] = {std::is_same<T, V>::value...};
#endif
                using type = size_t<reverse_find_index_i_(s_v, s_v + sizeof...(T), sizeof...(T))>;
            };
        } // namespace detail
        /// \endcond

        /// Finds the index of the last occurrence of the type \p T within the list \p
        /// List. Returns
        /// `#meta::npos` if the type \p T was not found.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        /// \sa `#meta::npos`
        template <typename List, typename T>
        using reverse_find_index = _t<detail::reverse_find_index_<List, T>>;

        namespace lazy
        {
            /// \sa 'meta::reverse_find_index'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using reverse_find_index = defer<reverse_find_index, List, T>;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // find
        /// Return the tail of the list \p List starting at the first occurrence of \p
        /// T, if any
        /// such element exists; the empty list, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename T>
        using find = drop<List, min<find_index<List, T>, size<List>>>;

        namespace lazy
        {
            /// \sa 'meta::find'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using find = defer<find, List, T>;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // reverse_find
        /// Return the tail of the list \p List starting at the last occurrence of \p T,
        /// if any such
        /// element exists; the empty list, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename T>
        using reverse_find = drop<List, min<reverse_find_index<List, T>, size<List>>>;

        namespace lazy
        {
            /// \sa 'meta::rfind'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using reverse_find = defer<reverse_find, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // find_if
        /// \cond
        namespace detail
        {
            constexpr bool const *find_if_i_(bool const *const begin, bool const *const end)
            {
                return begin == end || *begin ? begin : find_if_i_(begin + 1, end);
            }

            template <typename List, typename Fun, typename = void>
            struct find_if_
            {
            };

            template <typename Fun>
            struct find_if_<list<>, Fun>
            {
                using type = list<>;
            };

            template <typename... List, typename Fun>
            struct find_if_<list<List...>, Fun,
                            void_<integer_sequence<bool, bool(invoke<Fun, List>::type::value)...>>>
            {
#if (defined(__clang__) && __clang_major__ < 6) || defined(__apple_build_version__)
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(List)] = {invoke<Fun, List>::type::value...};
#else
                static constexpr bool s_v[] = {invoke<Fun, List>::type::value...};
#endif
                using type =
                    drop_c<list<List...>, detail::find_if_i_(s_v, s_v + sizeof...(List)) - s_v>;
            };
        } // namespace detail
        /// \endcond

        /// Return the tail of the list \p List starting at the first element `A` such
        /// that
        /// `invoke<Fun, A>::%value` is \c true, if any such element exists; the empty
        /// list,
        /// otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename Fun>
        using find_if = _t<detail::find_if_<List, Fun>>;

        namespace lazy
        {
            /// \sa 'meta::find_if'
            /// \ingroup lazy_query
            template <typename List, typename Fun>
            using find_if = defer<find_if, List, Fun>;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // reverse_find_if
        /// \cond
        namespace detail
        {
            constexpr bool const *reverse_find_if_i_(bool const *const begin, bool const *const pos,
                                                     bool const *const end)
            {
                return begin == pos
                           ? end
                           : *(pos - 1) ? pos - 1 : reverse_find_if_i_(begin, pos - 1, end);
            }

            template <typename List, typename Fun, typename = void>
            struct reverse_find_if_
            {
            };

            template <typename Fun>
            struct reverse_find_if_<list<>, Fun>
            {
                using type = list<>;
            };

            template <typename... List, typename Fun>
            struct reverse_find_if_<
                list<List...>, Fun,
                void_<integer_sequence<bool, bool(invoke<Fun, List>::type::value)...>>>
            {
#if (defined(__clang__) && __clang_major__ < 6) || defined(__apple_build_version__)
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(List)] = {invoke<Fun, List>::type::value...};
#else
                static constexpr bool s_v[] = {invoke<Fun, List>::type::value...};
#endif
                using type =
                  drop_c<list<List...>, detail::reverse_find_if_i_(s_v, s_v + sizeof...(List),
                                                                   s_v + sizeof...(List)) - s_v>;
            };
        }
        /// \endcond

        /// Return the tail of the list \p List starting at the last element `A` such
        /// that
        /// `invoke<Fun, A>::%value` is \c true, if any such element exists; the empty
        /// list,
        /// otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename Fun>
        using reverse_find_if = _t<detail::reverse_find_if_<List, Fun>>;

        namespace lazy
        {
            /// \sa 'meta::rfind_if'
            /// \ingroup lazy_query
            template <typename List, typename Fun>
            using reverse_find_if = defer<reverse_find_if, List, Fun>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // replace
        /// \cond
        namespace detail
        {
            template <typename List, typename T, typename U>
            struct replace_
            {
            };

            template <typename... List, typename T, typename U>
            struct replace_<list<List...>, T, U>
            {
                using type = list<if_<std::is_same<T, List>, U, List>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list where all instances of type \p T have been
        /// replaced with
        /// \p U.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename T, typename U>
        using replace = _t<detail::replace_<List, T, U>>;

        namespace lazy
        {
            /// \sa 'meta::replace'
            /// \ingroup lazy_transformation
            template <typename List, typename T, typename U>
            using replace = defer<replace, T, U>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // replace_if
        /// \cond
        namespace detail
        {
            template <typename List, typename C, typename U, typename = void>
            struct replace_if_
            {
            };

            template <typename... List, typename C, typename U>
            struct replace_if_<list<List...>, C, U,
                               void_<integer_sequence<bool, bool(invoke<C, List>::type::value)...>>>
            {
                using type = list<if_<invoke<C, List>, U, List>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list where all elements \c A of the list \p List for
        /// which
        /// `invoke<C,A>::%value` is \c true have been replaced with \p U.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename C, typename U>
        using replace_if = _t<detail::replace_if_<List, C, U>>;

        namespace lazy
        {
            /// \sa 'meta::replace_if'
            /// \ingroup lazy_transformation
            template <typename List, typename C, typename U>
            using replace_if = defer<replace_if, C, U>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        // count
        namespace detail
        {
            constexpr std::size_t count_i_(bool const *const begin, bool const *const end,
                                           std::size_t n)
            {
                return begin == end ? n : detail::count_i_(begin + 1, end, n + *begin);
            }

            template <typename List, typename T, typename = void>
            struct count_
            {
            };

            template <typename T>
            struct count_<list<>, T>
            {
                using type = meta::size_t<0>;
            };

            template <typename... List, typename T>
            struct count_<list<List...>, T>
            {
#if (defined(__clang__) && __clang_major__ < 6) || defined(__apple_build_version__)
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(List)] = {std::is_same<T, List>::value...};
#else
                static constexpr bool s_v[] = {std::is_same<T, List>::value...};
#endif
                using type = meta::size_t<detail::count_i_(s_v, s_v + sizeof...(List), 0u)>;
            };
        }

        /// Count the number of times a type \p T appears in the list \p List.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename T>
        using count = _t<detail::count_<List, T>>;

        namespace lazy
        {
            /// \sa `meta::count`
            /// \ingroup lazy_query
            template <typename List, typename T>
            using count = defer<count, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        // count_if
        namespace detail
        {
            template <typename List, typename Fn, typename = void>
            struct count_if_
            {
            };

            template <typename Fn>
            struct count_if_<list<>, Fn>
            {
                using type = meta::size_t<0>;
            };

            template <typename... List, typename Fn>
            struct count_if_<list<List...>, Fn,
                             void_<integer_sequence<bool, bool(invoke<Fn, List>::type::value)...>>>
            {
#if (defined(__clang__) && __clang_major__ < 6) || defined(__apple_build_version__)
                // Explicitly specify extent to avoid https://llvm.org/bugs/show_bug.cgi?id=28385
                static constexpr bool s_v[sizeof...(List)] = {invoke<Fn, List>::type::value...};
#else
                static constexpr bool s_v[] = {invoke<Fn, List>::type::value...};
#endif
                using type = meta::size_t<detail::count_i_(s_v, s_v + sizeof...(List), 0u)>;
            };
        }

        /// Count the number of times the predicate \p Fn evaluates to true for all the
        /// elements in
        /// the list \p List.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename Fn>
        using count_if = _t<detail::count_if_<List, Fn>>;

        namespace lazy
        {
            /// \sa `meta::count_if`
            /// \ingroup lazy_query
            template <typename List, typename Fn>
            using count_if = defer<count_if, List, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // filter
        /// \cond
        namespace detail
        {
            template <typename Pred>
            struct filter_
            {
                template <typename A>
                using invoke = if_c<invoke<Pred, A>::type::value, list<A>, list<>>;
            };
        } // namespace detail
        /// \endcond

        /// Returns a new meta::list where only those elements of \p List that satisfy
        /// the
        /// Callable \p Pred such that `invoke<Pred,A>::%value` is \c true are present.
        /// That is, those elements that don't satisfy the \p Pred are "removed".
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename Pred>
        using filter = join<transform<List, detail::filter_<Pred>>>;

        namespace lazy
        {
            /// \sa 'meta::filter'
            /// \ingroup lazy_transformation
            template <typename List, typename Pred>
            using filter = defer<filter, List, Pred>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // static_const
        ///\cond
        namespace detail
        {
            template <typename T>
            struct static_const
            {
                static constexpr T value{};
            };

            // Avoid potential ODR violations with global objects:
            template <typename T>
            constexpr T static_const<T>::value;
        } // namespace detail

        ///\endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // for_each
        /// \cond
        namespace detail
        {
            struct for_each_fn
            {
                template <class UnaryFunction, class... Args>
                constexpr auto operator()(list<Args...>, UnaryFunction f) const -> UnaryFunction
                {
                    return (void)std::initializer_list<int>{((void)f(Args{}), 0)...}, f;
                }
            };
        } // namespace detail
        /// \endcond

        ///\cond
        namespace
        {
            /// \endcond

            /// `for_each(List, UnaryFunction)` calls the \p UnaryFunction for each
            /// argument in the \p List.
            /// \ingroup runtime
            constexpr auto &&for_each = detail::static_const<detail::for_each_fn>::value;

            /// \cond
        }
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // transpose
        /// Given a list of lists of types \p ListOfLists, transpose the elements from
        /// the lists.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <typename ListOfLists>
        using transpose = fold<ListOfLists, repeat_n<size<front<ListOfLists>>, list<>>,
                               bind_back<quote<transform>, quote<push_back>>>;

        namespace lazy
        {
            /// \sa 'meta::transpose'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using transpose = defer<transpose, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // zip_with
        /// Given a list of lists of types \p ListOfLists and a Callable \p Fun,
        /// construct
        /// a new list by calling \p Fun with the elements from the lists pairwise.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <typename Fun, typename ListOfLists>
        using zip_with = transform<transpose<ListOfLists>, uncurry<Fun>>;

        namespace lazy
        {
            /// \sa 'meta::zip_with'
            /// \ingroup lazy_transformation
            template <typename Fun, typename ListOfLists>
            using zip_with = defer<zip_with, Fun, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // zip
        /// Given a list of lists of types \p ListOfLists, construct a new list by
        /// grouping the
        /// elements from the lists pairwise into `meta::list`s.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$
        /// is the size of the inner lists.
        /// \ingroup transformation
        template <typename ListOfLists>
        using zip = transpose<ListOfLists>;

        namespace lazy
        {
            /// \sa 'meta::zip'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using zip = defer<zip, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // as_list
        /// \cond
        namespace detail
        {
            template <typename T>
            using uncvref_t = _t<std::remove_cv<_t<std::remove_reference<T>>>>;

            // Indirection here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <typename Sequence>
            struct as_list_ : lazy::invoke<uncurry<quote<list>>, Sequence>
            {
            };
        } // namespace detail
        /// \endcond

        /// Turn a type into an instance of \c meta::list in a way determined by
        /// \c meta::apply.
        /// \ingroup list
        template <typename Sequence>
        using as_list = _t<detail::as_list_<detail::uncvref_t<Sequence>>>;

        namespace lazy
        {
            /// \sa 'meta::as_list'
            /// \ingroup lazy_list
            template <typename Sequence>
            using as_list = defer<as_list, Sequence>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse
        /// \cond
        namespace detail
        {
            template <typename List, typename State = list<>>
            struct reverse_ : lazy::fold<List, State, quote<push_front>>
            {
            };

            template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5,
                      typename T6, typename T7, typename T8, typename T9, typename... Ts,
                      typename... Us>
            struct reverse_<list<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, Ts...>, list<Us...>>
                : reverse_<list<Ts...>, list<T9, T8, T7, T6, T5, T4, T3, T2, T1, T0, Us...>>
            {
            };
        }
        /// \endcond

        /// Return a new \c meta::list by reversing the elements in the list \p List.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List>
        using reverse = _t<detail::reverse_<List>>;

        namespace lazy
        {
            /// \sa 'meta::reverse'
            /// \ingroup lazy_transformation
            template <typename List>
            using reverse = defer<reverse, List>;
        }

        /// Logically negate the result of Callable \p F.
        /// \ingroup trait
        template <typename F>
        using not_fn = compose<quote<not_>, F>;

        namespace lazy
        {
            /// \sa 'meta::not_fn'
            /// \ingroup lazy_trait
            template <typename F>
            using not_fn = defer<not_fn, F>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // all_of
        /// A Boolean integral constant wrapper around \c true if `invoke<F, A>::%value`
        /// is \c true
        /// for all elements \c A in \c meta::list \p List; \c false, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename F>
        using all_of = empty<find_if<List, not_fn<F>>>;

        namespace lazy
        {
            /// \sa 'meta::all_of'
            /// \ingroup lazy_query
            template <typename List, typename Fn>
            using all_of = defer<all_of, List, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // any_of
        /// A Boolean integral constant wrapper around \c true if `invoke<F, A>::%value`
        /// is
        /// \c true for any element \c A in \c meta::list \p List; \c false, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename F>
        using any_of = not_<empty<find_if<List, F>>>;

        namespace lazy
        {
            /// \sa 'meta::any_of'
            /// \ingroup lazy_query
            template <typename List, typename Fn>
            using any_of = defer<any_of, List, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // none_of
        /// A Boolean integral constant wrapper around \c true if `invoke<F, A>::%value`
        /// is
        /// \c false for all elements \c A in \c meta::list \p List; \c false,
        /// otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename F>
        using none_of = empty<find_if<List, F>>;

        namespace lazy
        {
            /// \sa 'meta::none_of'
            /// \ingroup lazy_query
            template <typename List, typename Fn>
            using none_of = defer<none_of, List, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // in
        /// A Boolean integral constant wrapper around \c true if there is at least one
        /// occurrence
        /// of \p T in \p List.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <typename List, typename T>
        using in = not_<empty<find<List, T>>>;

        namespace lazy
        {
            /// \sa 'meta::in'
            /// \ingroup lazy_query
            template <typename List, typename T>
            using in = defer<in, List, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // inherit
        /// \cond
        namespace detail
        {
            template <typename List>
            struct inherit_
            {
            };

            template <typename... List>
            struct inherit_<list<List...>> : List...
            {
                using type = inherit_;
            };
        }
        /// \endcond

        /// A type that inherits from all the types in the list
        /// \pre The types in the list must be unique
        /// \pre All the types in the list must be non-final class types
        /// \ingroup datatype
        template <typename List>
        using inherit = meta::_t<detail::inherit_<List>>;

        namespace lazy
        {
            /// \sa 'meta::inherit'
            /// \ingroup lazy_datatype
            template <typename List>
            using inherit = defer<inherit, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // set
        // Used to improve the performance of \c meta::unique.
        /// \cond
        namespace detail
        {
            template <typename Set, typename T>
            struct in_
            {
            };

            template <typename... Set, typename T>
            struct in_<list<Set...>, T> : std::is_base_of<id<T>, inherit<list<id<Set>...>>>
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
        } // namespace detail
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // unique
        /// Return a new \c meta::list where all duplicate elements have been removed.
        /// \par Complexity
        /// \f$ O(N^2) \f$.
        /// \ingroup transformation
        template <typename List>
        using unique = fold<List, list<>, quote_trait<detail::insert_back_>>;

        namespace lazy
        {
            /// \sa 'meta::unique'
            /// \ingroup lazy_transformation
            template <typename List>
            using unique = defer<unique, List>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // partition
        /// \cond
        namespace detail
        {
            template <typename Pred>
            struct partition_
            {
                template <typename, typename, typename = void>
                struct impl
                {
                };
                template <typename... Yes, typename... No, typename A>
                struct impl<pair<list<Yes...>, list<No...>>, A,
                            void_<bool_<invoke<Pred, A>::type::value>>>
                {
                    using type = if_<invoke<Pred, A>, pair<list<Yes..., A>, list<No...>>,
                                     pair<list<Yes...>, list<No..., A>>>;
                };
                template <typename State, typename A>
                using invoke = _t<impl<State, A>>;
            };
        } // namespace detail
        /// \endcond

        /// Returns a pair of lists, where the elements of \p List that satisfy the
        /// Callable \p Pred such that `invoke<Pred,A>::%value` is \c true are present
        /// in the
        /// first list and the rest are in the second.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename List, typename Pred>
        using partition = fold<List, pair<list<>, list<>>, detail::partition_<Pred>>;

        namespace lazy
        {
            /// \sa 'meta::partition'
            /// \ingroup lazy_transformation
            template <typename List, typename Pred>
            using partition = defer<partition, List, Pred>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // sort
        /// \cond
        namespace detail
        {
            template <typename, typename, typename = void>
            struct sort_
            {
            };

            template <typename Pred>
            struct sort_<list<>, Pred>
            {
                using type = list<>;
            };

            template <typename A, typename Pred>
            struct sort_<list<A>, Pred>
            {
                using type = list<A>;
            };

            template <typename A, typename B, typename... List, typename Pred>
            struct sort_<
                list<A, B, List...>, Pred,
                void_<_t<sort_<first<partition<list<B, List...>, bind_back<Pred, A>>>, Pred>>>>
            {
                using P = partition<list<B, List...>, bind_back<Pred, A>>;
                using type = concat<_t<sort_<first<P>, Pred>>, list<A>, _t<sort_<second<P>, Pred>>>;
            };
        }
        /// \endcond

        // clang-format off
        /// Return a new \c meta::list that is sorted according to Callable predicate \p Pred.
        /// \par Complexity
        /// Expected: \f$ O(N log N) \f$
        /// Worst case: \f$ O(N^2) \f$.
        /// \code
        /// using L0 = list<char[5], char[3], char[2], char[6], char[1], char[5], char[10]>;
        /// using L1 = meta::sort<L0, lambda<_a, _b, lazy::less<lazy::sizeof_<_a>, lazy::sizeof_<_b>>>>;
        /// static_assert(std::is_same<L1, list<char[1], char[2], char[3], char[5], char[5], char[6], char[10]>>::value, "");
        /// \endcode
        /// \ingroup transformation
        // clang-format on
        template <typename List, typename Pred>
        using sort = _t<detail::sort_<List, Pred>>;

        namespace lazy
        {
            /// \sa 'meta::sort'
            /// \ingroup lazy_transformation
            template <typename List, typename Pred>
            using sort = defer<sort, List, Pred>;
        }

        ////////////////////////////////////////////////////////////////////////////
        // lambda_
        /// \cond
        namespace detail
        {
            template <typename T, int = 0>
            struct protect_;

            template <typename, int = 0>
            struct vararg_;

            template <typename T, int = 0>
            struct is_valid_;

            // Returns which branch to evaluate
            template <typename If, typename... Ts>
            using lazy_if_ = lazy::_t<defer<_if_, list<If, protect_<Ts>...>>>;

            template <typename A, typename T, typename F, typename Ts>
            struct subst1_
            {
                using type = list<list<T>>;
            };
            template <typename T, typename F, typename Ts>
            struct subst1_<F, T, F, Ts>
            {
                using type = list<>;
            };
            template <typename A, typename T, typename F, typename Ts>
            struct subst1_<vararg_<A>, T, F, Ts>
            {
                using type = list<Ts>;
            };

            template <typename As, typename Ts>
            using substitutions_ = push_back<
                join<transform<
                    concat<As, repeat_n_c<size<Ts>{} + 2 - size<As>{}, back<As>>>,
                    concat<Ts, repeat_n_c<2, back<As>>>,
                    bind_back<quote_trait<subst1_>, back<As>, drop_c<Ts, size<As>{} - 2>>>>,
                list<back<As>>>;

            template <typename As, typename Ts>
            using substitutions =
                invoke<if_c<(size<Ts>{} + 2 >= size<As>{}), quote<substitutions_>>, As, Ts>;

            template <typename T>
            struct is_vararg_ : std::false_type
            {
            };
            template <typename T>
            struct is_vararg_<vararg_<T>> : std::true_type
            {
            };

            template <typename Tags>
            using is_variadic_ = is_vararg_<at<push_front<Tags, void>, dec<size<Tags>>>>;

            template <typename Tags, bool IsVariadic = is_variadic_<Tags>::value>
            struct lambda_;

            // Non-variadic lambda implementation
            template <typename... As>
            struct lambda_<list<As...>, false>
            {
            private:
                static constexpr std::size_t arity = sizeof...(As)-1;
                using Tags = list<As...>; // Includes the lambda body as the last arg!
                using F = back<Tags>;
                template <typename T, typename Args>
                struct impl;
                template <typename T, typename Args>
                using lazy_impl_ = lazy::_t<defer<impl, T, protect_<Args>>>;
                template <typename, typename, typename = void>
                struct subst_
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct subst_<defer<C, Ts...>, Args, void_<C<_t<impl<Ts, Args>>...>>>
                {
                    using type = C<_t<impl<Ts, Args>>...>;
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct subst_<defer_i<T, C, Is...>, Args, void_<C<Is...>>>
                {
                    using type = C<Is...>;
                };
                template <typename T, typename Args>
                struct impl : if_c<(reverse_find_index<Tags, T>() != npos()),
                                   lazy::at<Args, reverse_find_index<Tags, T>>, id<T>>
                {
                };
                template <typename T, typename Args>
                struct impl<protect_<T>, Args>
                {
                    using type = T;
                };
                template <typename T, typename Args>
                struct impl<is_valid_<T>, Args>
                {
                    using type = is_trait<impl<T, Args>>;
                };
                template <typename If, typename... Ts, typename Args>
                struct impl<defer<if_, If, Ts...>, Args> // Short-circuit if_
                    : impl<lazy_impl_<lazy_if_<If, Ts...>, Args>, Args>
                {
                };
                template <typename Bool_, typename... Ts, typename Args>
                struct impl<defer<and_, Bool_, Ts...>, Args> // Short-circuit and_
                    : impl<lazy_impl_<lazy_if_<Bool_, lazy::and_<Ts...>, protect_<std::false_type>>,
                                      Args>,
                           Args>
                {
                };
                template <typename Bool_, typename... Ts, typename Args>
                struct impl<defer<or_, Bool_, Ts...>, Args> // Short-circuit or_
                    : impl<lazy_impl_<lazy_if_<Bool_, protect_<std::true_type>, lazy::or_<Ts...>>,
                                      Args>,
                           Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<defer<C, Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct impl<defer_i<T, C, Is...>, Args> : subst_<defer_i<T, C, Is...>, Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<C<Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename... Ts, typename Args>
                struct impl<lambda_<list<Ts...>, false>, Args>
                {
                    using type = compose<uncurry<lambda_<list<As..., Ts...>, false>>,
                                         curry<bind_front<quote<concat>, Args>>>;
                };
                template <typename... Bs, typename Args>
                struct impl<lambda_<list<Bs...>, true>, Args>
                {
                    using type = compose<typename lambda_<list<As..., Bs...>, true>::thunk,
                                         bind_front<quote<concat>, transform<Args, quote<list>>>,
                                         curry<bind_front<quote<substitutions>, list<Bs...>>>>;
                };

            public:
                template <typename... Ts>
                using invoke = _t<if_c<sizeof...(Ts) == arity, impl<F, list<Ts..., F>>>>;
            };

            // Lambda with variadic placeholder (broken out due to less efficient
            // compile-time
            // resource usage)
            template <typename... As>
            struct lambda_<list<As...>, true>
            {
            private:
                template <typename T, bool IsVar>
                friend struct lambda_;
                using Tags = list<As...>; // Includes the lambda body as the last arg!
                template <typename T, typename Args>
                struct impl;
                template <typename Args>
                using eval_impl_ = bind_back<quote_trait<impl>, Args>;
                template <typename T, typename Args>
                using lazy_impl_ = lazy::_t<defer<impl, T, protect_<Args>>>;
                template <template <typename...> class C, typename Args, typename Ts>
                using try_subst_ = apply<quote<C>, join<transform<Ts, eval_impl_<Args>>>>;
                template <typename, typename, typename = void>
                struct subst_
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct subst_<defer<C, Ts...>, Args, void_<try_subst_<C, Args, list<Ts...>>>>
                {
                    using type = list<try_subst_<C, Args, list<Ts...>>>;
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct subst_<defer_i<T, C, Is...>, Args, void_<C<Is...>>>
                {
                    using type = list<C<Is...>>;
                };
                template <typename T, typename Args>
                struct impl : if_c<(reverse_find_index<Tags, T>() != npos()),
                                   lazy::at<Args, reverse_find_index<Tags, T>>, id<list<T>>>
                {
                };
                template <typename T, typename Args>
                struct impl<protect_<T>, Args>
                {
                    using type = list<T>;
                };
                template <typename T, typename Args>
                struct impl<is_valid_<T>, Args>
                {
                    using type = list<is_trait<impl<T, Args>>>;
                };
                template <typename If, typename... Ts, typename Args>
                struct impl<defer<if_, If, Ts...>, Args> // Short-circuit if_
                    : impl<lazy_impl_<lazy_if_<If, Ts...>, Args>, Args>
                {
                };
                template <typename Bool_, typename... Ts, typename Args>
                struct impl<defer<and_, Bool_, Ts...>, Args> // Short-circuit and_
                    : impl<lazy_impl_<lazy_if_<Bool_, lazy::and_<Ts...>, protect_<std::false_type>>,
                                      Args>,
                           Args>
                {
                };
                template <typename Bool_, typename... Ts, typename Args>
                struct impl<defer<or_, Bool_, Ts...>, Args> // Short-circuit or_
                    : impl<lazy_impl_<lazy_if_<Bool_, protect_<std::true_type>, lazy::or_<Ts...>>,
                                      Args>,
                           Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<defer<C, Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct impl<defer_i<T, C, Is...>, Args> : subst_<defer_i<T, C, Is...>, Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<C<Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename... Bs, bool IsVar, typename Args>
                struct impl<lambda_<list<Bs...>, IsVar>, Args>
                {
                    using type =
                        list<compose<typename lambda_<list<As..., Bs...>, true>::thunk,
                                     bind_front<quote<concat>, Args>,
                                     curry<bind_front<quote<substitutions>, list<Bs...>>>>>;
                };
                struct thunk
                {
                    template <typename S, typename R = _t<impl<back<Tags>, S>>>
                    using invoke = if_c<size<R>{} == 1, front<R>>;
                };

            public:
                template <typename... Ts>
                using invoke = invoke<thunk, substitutions<Tags, list<Ts...>>>;
            };
        }
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // lambda
        /// For creating anonymous Callables.
        /// \code
        /// using L = lambda<_a, _b, std::pair<_b, std::pair<_a, _a>>>;
        /// using P = invoke<L, int, short>;
        /// static_assert(std::is_same<P, std::pair<short, std::pair<int, int>>>::value,
        /// "");
        /// \endcode
        /// \ingroup trait
        template <typename... Ts>
        using lambda = if_c<(sizeof...(Ts) > 0), detail::lambda_<list<Ts...>>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // is_valid
        /// For testing whether a deferred computation will succeed in a \c let or a \c
        /// lambda.
        /// \ingroup trait
        template <typename T>
        using is_valid = detail::is_valid_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // vararg
        /// For defining variadic placeholders.
        template <typename T>
        using vararg = detail::vararg_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // protect
        /// For preventing the evaluation of a nested `defer`ed computation in a \c let
        /// or
        /// \c lambda expression.
        template <typename T>
        using protect = detail::protect_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // var
        /// For use when defining local variables in \c meta::let expressions
        /// \sa `meta::let`
        template <typename Tag, typename Value>
        struct var;

        /// \cond
        namespace detail
        {
            template <typename... As>
            struct let_
            {
            };
            template <typename Fn>
            struct let_<Fn>
            {
                using type = lazy::invoke<lambda<Fn>>;
            };
            template <typename Tag, typename Value, typename... Rest>
            struct let_<var<Tag, Value>, Rest...>
            {
                using type = lazy::invoke<lambda<Tag, _t<let_<Rest...>>>, Value>;
            };
        }
        /// \endcond

        /// A lexically scoped expression with local variables.
        ///
        /// \code
        /// template <typename T, typename List>
        /// using find_index_ = let<
        ///     var<_a, List>,
        ///     var<_b, lazy::find<_a, T>>,
        ///     lazy::if_<
        ///         std::is_same<_b, list<>>,
        ///         meta::npos,
        ///         lazy::minus<lazy::size<_a>, lazy::size<_b>>>>;
        /// static_assert(find_index_<int, list<short, int, float>>{} == 1, "");
        /// static_assert(find_index_<double, list<short, int, float>>{} ==
        /// meta::npos{}, "");
        /// \endcode
        /// \ingroup trait
        template <typename... As>
        using let = _t<_t<detail::let_<As...>>>;

        namespace lazy
        {
            /// \sa `meta::let`
            /// \ingroup lazy_trait
            template <typename... As>
            using let = defer<let, As...>;
        }

        // Some argument placeholders for use in \c lambda expressions.
        /// \ingroup trait
        inline namespace placeholders
        {
            // regular placeholders:
            struct _a;
            struct _b;
            struct _c;
            struct _d;
            struct _e;
            struct _f;
            struct _g;
            struct _h;
            struct _i;

            // variadic placeholders:
            using _args = vararg<void>;
            using _args_a = vararg<_a>;
            using _args_b = vararg<_b>;
            using _args_c = vararg<_c>;
        } // namespace placeholders

        ///////////////////////////////////////////////////////////////////////////////////////////
        // cartesian_product
        /// \cond
        namespace detail
        {
            template <typename M2, typename M>
            struct cartesian_product_fn
            {
                template <typename X>
                struct lambda0
                {
                    template <typename Xs>
                    using lambda1 = list<push_front<Xs, X>>;
                    using type = join<transform<M2, quote<lambda1>>>;
                };
                using type = join<transform<M, quote_trait<lambda0>>>;
            };
        } // namespace detail
        /// \endcond

        /// Given a list of lists \p ListOfLists, return a new list of lists that is the
        /// Cartesian
        /// Product. Like the `sequence` function from the Haskell Prelude.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <typename ListOfLists>
        using cartesian_product =
            reverse_fold<ListOfLists, list<list<>>, quote_trait<detail::cartesian_product_fn>>;

        namespace lazy
        {
            /// \sa 'meta::cartesian_product'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using cartesian_product = defer<cartesian_product, ListOfLists>;
        }

        /// \cond
        ///////////////////////////////////////////////////////////////////////////////////////////
        // add_const_if
        namespace detail
        {
            template <bool>
            struct add_const_if
            {
                template <typename T>
                using invoke = T const;
            };
            template <>
            struct add_const_if<false>
            {
                template <typename T>
                using invoke = T;
            };
        } // namespace detail
        template <bool If>
        using add_const_if_c = detail::add_const_if<If>;
        template <typename If>
        using add_const_if = add_const_if_c<If::type::value>;
        /// \endcond

        /// \cond
        ///////////////////////////////////////////////////////////////////////////////////////////
        // const_if
        template <bool If, typename T>
        using const_if_c = invoke<add_const_if_c<If>, T>;
        template <typename If, typename T>
        using const_if = invoke<add_const_if<If>, T>;
        /// \endcond

        /// \cond
        namespace detail
        {
            template <typename State, typename Ch>
            using atoi_ = if_c<(Ch::value >= '0' && Ch::value <= '9'),
                               std::integral_constant<typename State::value_type,
                                                      State::value * 10 + (Ch::value - '0')>>;
        }
        /// \endcond

        inline namespace literals
        {
            /// A user-defined literal that generates objects of type \c meta::size_t.
            /// \ingroup integral
            template <char... Chs>
            constexpr fold<list<char_<Chs>...>, meta::size_t<0>, quote<detail::atoi_>>
                operator"" _z()
            {
                return {};
            }
        }
        /// \cond
    } // namespace v1
      /// \endcond
} // namespace meta

/// \cond
#if defined(__clang__) && defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 1101

_LIBCPP_BEGIN_NAMESPACE_STD
template <class>
class _LIBCPP_TYPE_VIS_ONLY allocator;
template <class, class>
struct _LIBCPP_TYPE_VIS_ONLY pair;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY hash;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY less;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY equal_to;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY char_traits;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY list;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY forward_list;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY vector;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY deque;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY basic_string;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY map;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY multimap;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY set;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY multiset;
template <class, class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_map;
template <class, class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_multimap;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_set;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_multiset;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY queue;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY priority_queue;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY stack;
_LIBCPP_END_NAMESPACE_STD

namespace meta
{
    inline namespace v1
    {
        namespace detail
        {
            template <typename T, typename A = std::allocator<T>>
            using std_list = std::list<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_forward_list = std::forward_list<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_vector = std::vector<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_deque = std::deque<T, A>;
            template <typename T, typename C = std::char_traits<T>, typename A = std::allocator<T>>
            using std_basic_string = std::basic_string<T, C, A>;
            template <typename K, typename V, typename C = std::less<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_map = std::map<K, V, C, A>;
            template <typename K, typename V, typename C = std::less<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_multimap = std::multimap<K, V, C, A>;
            template <typename K, typename C = std::less<K>, typename A = std::allocator<K>>
            using std_set = std::set<K, C, A>;
            template <typename K, typename C = std::less<K>, typename A = std::allocator<K>>
            using std_multiset = std::multiset<K, C, A>;
            template <typename K, typename V, typename H = std::hash<K>,
                      typename C = std::equal_to<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_unordered_map = std::unordered_map<K, V, H, C, A>;
            template <typename K, typename V, typename H = std::hash<K>,
                      typename C = std::equal_to<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_unordered_multimap = std::unordered_multimap<K, V, H, C, A>;
            template <typename K, typename H = std::hash<K>, typename C = std::equal_to<K>,
                      typename A = std::allocator<K>>
            using std_unordered_set = std::unordered_set<K, H, C, A>;
            template <typename K, typename H = std::hash<K>, typename C = std::equal_to<K>,
                      typename A = std::allocator<K>>
            using std_unordered_multiset = std::unordered_multiset<K, H, C, A>;
            template <typename T, typename C = std_deque<T>>
            using std_queue = std::queue<T, C>;
            template <typename T, typename C = std_vector<T>,
                      class D = std::less<typename C::value_type>>
            using std_priority_queue = std::priority_queue<T, C, D>;
            template <typename T, typename C = std_deque<T>>
            using std_stack = std::stack<T, C>;
        }

        template <>
        struct quote<::std::list> : quote<detail::std_list>
        {
        };
        template <>
        struct quote<::std::deque> : quote<detail::std_deque>
        {
        };
        template <>
        struct quote<::std::forward_list> : quote<detail::std_forward_list>
        {
        };
        template <>
        struct quote<::std::vector> : quote<detail::std_vector>
        {
        };
        template <>
        struct quote<::std::basic_string> : quote<detail::std_basic_string>
        {
        };
        template <>
        struct quote<::std::map> : quote<detail::std_map>
        {
        };
        template <>
        struct quote<::std::multimap> : quote<detail::std_multimap>
        {
        };
        template <>
        struct quote<::std::set> : quote<detail::std_set>
        {
        };
        template <>
        struct quote<::std::multiset> : quote<detail::std_multiset>
        {
        };
        template <>
        struct quote<::std::unordered_map> : quote<detail::std_unordered_map>
        {
        };
        template <>
        struct quote<::std::unordered_multimap> : quote<detail::std_unordered_multimap>
        {
        };
        template <>
        struct quote<::std::unordered_set> : quote<detail::std_unordered_set>
        {
        };
        template <>
        struct quote<::std::unordered_multiset> : quote<detail::std_unordered_multiset>
        {
        };
        template <>
        struct quote<::std::queue> : quote<detail::std_queue>
        {
        };
        template <>
        struct quote<::std::priority_queue> : quote<detail::std_priority_queue>
        {
        };
        template <>
        struct quote<::std::stack> : quote<detail::std_stack>
        {
        };
    }
}

#endif
/// \endcond

#ifdef __clang__
#pragma GCC diagnostic pop
#endif
#endif
