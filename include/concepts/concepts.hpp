/// \file
//  Concepts emulation library
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

#ifndef CONCEPT_CONCEPTS_HPP
#define CONCEPT_CONCEPTS_HPP

#include <initializer_list>
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <concepts/swap.hpp>
#include <concepts/type_traits.hpp>

#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
// disable buggy compatibility warning about "requires" and "concept" being
// C++20 keywords.
#pragma GCC diagnostic ignored "-Wc++2a-compat"
#endif

#ifdef __clang__
#define CONCEPT_PP_IS_SAME(A, B) __is_same(A, B)
#elif defined(__GNUC__)
#define CONCEPT_PP_IS_SAME(A, B) __is_same_as(A, B)
#else
#define CONCEPT_PP_IS_SAME(A, B) std::is_same<A, B>::value
#endif

#if __COUNTER__ != __COUNTER__
#define CONCEPT_COUNTER __COUNTER__
#else
#define CONCEPT_COUNTER __LINE__
#endif

#define CONCEPT_PP_CHECK(...) CONCEPT_PP_CHECK_N(__VA_ARGS__, 0,)
#define CONCEPT_PP_CHECK_N(x, n, ...) n
#define CONCEPT_PP_PROBE(x) x, 1,

// CONCEPT_CXX_VA_OPT
#ifndef CONCEPT_CXX_VA_OPT
#define CONCEPT_CXX_VA_OPT_(...) CONCEPT_PP_CHECK(__VA_OPT__(,) 1)
#define CONCEPT_CXX_VA_OPT CONCEPT_CXX_VA_OPT_(~)
#endif // CONCEPT_CXX_VA_OPT

#define CONCEPT_PP_CAT_(X, ...)  X ## __VA_ARGS__
#define CONCEPT_PP_CAT(X, ...)   CONCEPT_PP_CAT_(X, __VA_ARGS__)
#define CONCEPT_PP_CAT2_(X, ...) X ## __VA_ARGS__
#define CONCEPT_PP_CAT2(X, ...)  CONCEPT_PP_CAT2_(X, __VA_ARGS__)

#define CONCEPT_PP_EVAL(X, ...) X(__VA_ARGS__)
#define CONCEPT_PP_EVAL2(X, ...) X(__VA_ARGS__)
#define CONCEPT_PP_EVAL3(X, ...) X(__VA_ARGS__)
#define CONCEPT_PP_EVAL4(X, ...) X(__VA_ARGS__)

#define CONCEPT_PP_EXPAND(...) __VA_ARGS__
#define CONCEPT_PP_EAT(...)

#define CONCEPT_PP_IS_PAREN(x) CONCEPT_PP_CHECK(CONCEPT_PP_IS_PAREN_PROBE x)
#define CONCEPT_PP_IS_PAREN_PROBE(...) CONCEPT_PP_PROBE(~)

#define CONCEPT_PP_COUNT(...)                                                   \
    CONCEPT_PP_COUNT_(__VA_ARGS__,                                              \
        50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,            \
        30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,            \
        10,9,8,7,6,5,4,3,2,1,)                                                  \
        /**/
#define CONCEPT_PP_COUNT_(                                                      \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,                                    \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,                           \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,                           \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,                           \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, N, ...)                   \
    N                                                                           \
    /**/

#define CONCEPT_PP_IS_EQUAL(X,Y) \
    CONCEPT_PP_CHECK(CONCEPT_PP_CAT(CONCEPT_PP_CAT(CONCEPT_PP_IS_EQUAL_, X), Y))
#define CONCEPT_PP_IS_EQUAL_00 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_11 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_22 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_33 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_44 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_55 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_66 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_77 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_88 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_99 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_1010 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_1111 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_1212 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_1313 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_1414 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_1515 CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_EQUAL_1616 CONCEPT_PP_PROBE(~)

#define CONCEPT_PP_NOT(X) CONCEPT_PP_CAT(CONCEPT_PP_NOT_, X)
#define CONCEPT_PP_NOT_0 1
#define CONCEPT_PP_NOT_1 0

#define CONCEPT_PP_IIF(BIT) CONCEPT_PP_CAT_(CONCEPT_PP_IIF_, BIT)
#define CONCEPT_PP_IIF_0(TRUE, ...) __VA_ARGS__
#define CONCEPT_PP_IIF_1(TRUE, ...) TRUE
#define CONCEPT_PP_FIRST(X, ...) X
#define CONCEPT_PP_SECOND(X, ...) __VA_ARGS__

#define CONCEPT_PP_OR(X,Y) \
    CONCEPT_PP_NOT(CONCEPT_PP_CHECK(CONCEPT_PP_CAT(CONCEPT_PP_CAT(CONCEPT_PP_NOR_, X), Y)))
#define CONCEPT_PP_NOR_00 CONCEPT_PP_PROBE(~)

#if CONCEPT_CXX_VA_OPT

#define CONCEPT_PP_IS_EMPTY_NON_FUNCTION(...) \
    CONCEPT_PP_CHECK(__VA_OPT__(, 0), 1)

#else // RANGES_VA_OPT

#define CONCEPT_PP_SPLIT(i, ...)                                                \
    CONCEPT_PP_CAT_(CONCEPT_PP_SPLIT_, i)(__VA_ARGS__)                          \
    /**/
#define CONCEPT_PP_SPLIT_0(a, ...) a
#define CONCEPT_PP_SPLIT_1(a, ...) __VA_ARGS__

#define CONCEPT_PP_IS_VARIADIC(...)                                             \
    CONCEPT_PP_SPLIT(                                                           \
        0,                                                                      \
        CONCEPT_PP_CAT(                                                         \
            CONCEPT_PP_IS_VARIADIC_R_,                                          \
            CONCEPT_PP_IS_VARIADIC_C __VA_ARGS__))                              \
    /**/
#define CONCEPT_PP_IS_VARIADIC_C(...) 1
#define CONCEPT_PP_IS_VARIADIC_R_1 1,
#define CONCEPT_PP_IS_VARIADIC_R_CONCEPT_PP_IS_VARIADIC_C 0,

// emptiness detection macro...

#define CONCEPT_PP_IS_EMPTY_NON_FUNCTION(...)                                   \
    CONCEPT_PP_IIF(CONCEPT_PP_IS_VARIADIC(__VA_ARGS__))                         \
    (0, CONCEPT_PP_IS_VARIADIC(CONCEPT_PP_IS_EMPTY_NON_FUNCTION_C __VA_ARGS__()))\
    /**/
#define CONCEPT_PP_IS_EMPTY_NON_FUNCTION_C() ()

#endif // CONCEPT_PP_VA_OPT

#define CONCEPT_PP_EMPTY()
#define CONCEPT_PP_COMMA() ,
#define CONCEPT_PP_COMMA_IIF(X)                                                 \
    CONCEPT_PP_IIF(X)(CONCEPT_PP_EMPTY, CONCEPT_PP_COMMA)()

////////////////////////////////////////////////////////////////////////////////
// CONCEPT_template

#define CONCEPT_PP_EVAL_OR_(X, ...)\
    CONCEPT_PP_IIF(CONCEPT_PP_IS_PAREN(X))(CONCEPT_PP_EXPAND, CONCEPT_PP_EVAL)(X, __VA_ARGS__)
#define CONCEPT_PP_EVAL_OR(...)\
    CONCEPT_PP_EVAL_OR_(__VA_ARGS__)

#define CONCEPT_PP_IF_PREFIX_ADDS_ARG_(PREFIX, ...)\
    CONCEPT_PP_NOT(CONCEPT_PP_IS_EQUAL(CONCEPT_PP_COUNT(CONCEPT_PP_CAT(PREFIX, __VA_ARGS__)), CONCEPT_PP_COUNT(__VA_ARGS__)))
#define CONCEPT_PP_IF_PREFIX_ADDS_ARG(...)\
    CONCEPT_PP_IF_PREFIX_ADDS_ARG_(__VA_ARGS__)

#define CONCEPT_PP_IS_KEYWORD(...) \
    CONCEPT_PP_IF_PREFIX_ADDS_ARG(CONCEPT_PP_TOLIST_, __VA_ARGS__)

#define CONCEPT_PP_FINALIZE_(PAIR, ...) \
    CONCEPT_PP_CAT(CONCEPT_PP_FINALIZE_, CONCEPT_PP_FIRST PAIR)((CONCEPT_PP_SECOND PAIR), __VA_ARGS__)
#define CONCEPT_PP_FINALIZE(...)\
    CONCEPT_PP_FINALIZE_(__VA_ARGS__)

#define CONCEPT_PP_IS_AUTO_auto CONCEPT_PP_PROBE(~)
#define CONCEPT_PP_IS_DECLTYPE_AUTO_0(...) ~
#define CONCEPT_PP_IS_DECLTYPE_AUTO_1(...) CONCEPT_PP_CAT(CONCEPT_PP_IS_AUTO_, __VA_ARGS__)
#define CONCEPT_PP_IS_DECLTYPE_AUTO_decltype(...) \
    CONCEPT_PP_CAT(CONCEPT_PP_IS_DECLTYPE_AUTO_, CONCEPT_PP_IS_EQUAL(1, CONCEPT_PP_COUNT(__VA_ARGS__)))(__VA_ARGS__)
#define CONCEPT_PP_IS_DECLTYPE_AUTO(...) \
    CONCEPT_PP_IIF(CONCEPT_PP_IS_EQUAL(1, CONCEPT_PP_COUNT(__VA_ARGS__)))(CONCEPT_PP_CHECK(CONCEPT_PP_CAT2(CONCEPT_PP_IS_DECLTYPE_AUTO_, __VA_ARGS__)),0)

#define CONCEPT_PP_USE_SFINAE_TPARAM(...) \
    CONCEPT_PP_OR(CONCEPT_PP_IF_PREFIX_ADDS_ARG(CONCEPT_PP_TESTSFINAETPARAM_, __VA_ARGS__), CONCEPT_PP_IS_DECLTYPE_AUTO(__VA_ARGS__))

#define CONCEPT_PP_TESTSFINAETPARAM_auto ~,
#define CONCEPT_PP_TESTSFINAETPARAM_implicit ~,

#define CONCEPT_PP_ENABLE_IF_0(DECL, ...) \
    > CONCEPT_PP_EXPAND DECL typename std::enable_if<_concept_requires_, __VA_ARGS__>::type
#define CONCEPT_PP_ENABLE_IF_1(DECL, ...) \
    , typename std::enable_if<_concept_requires_, int>::type = 0> CONCEPT_PP_EXPAND DECL

#define CONCEPT_PP_FINALIZE_IMPL(DECL, ...)\
    CONCEPT_PP_CAT(CONCEPT_PP_ENABLE_IF_, CONCEPT_PP_OR(CONCEPT_PP_IS_EMPTY_NON_FUNCTION(__VA_ARGS__), CONCEPT_PP_USE_SFINAE_TPARAM(__VA_ARGS__)))(DECL, __VA_ARGS__) \
    CONCEPT_PP_IIF(CONCEPT_PP_USE_SFINAE_TPARAM(__VA_ARGS__))(CONCEPT_PP_EXPAND, CONCEPT_PP_EAT)(__VA_ARGS__)

#define CONCEPT_PP_FINALIZE_1(TYPE, ...) \
    CONCEPT_PP_FINALIZE_IMPL((), CONCEPT_PP_EXPAND TYPE)
#define CONCEPT_PP_FINALIZE_2(TYPE, _0, ...) \
    CONCEPT_PP_FINALIZE_IMPL((CONCEPT_PP_EXPAND _0), CONCEPT_PP_EXPAND TYPE)
#define CONCEPT_PP_FINALIZE_3(TYPE, _0, _1, ...) \
    CONCEPT_PP_FINALIZE_IMPL((CONCEPT_PP_EXPAND _0 CONCEPT_PP_EXPAND _1), CONCEPT_PP_EXPAND TYPE)
#define CONCEPT_PP_FINALIZE_4(TYPE, _0, _1, _2, ...) \
    CONCEPT_PP_FINALIZE_IMPL((CONCEPT_PP_EXPAND _0 CONCEPT_PP_EXPAND _1 CONCEPT_PP_EXPAND _2), CONCEPT_PP_EXPAND TYPE)
#define CONCEPT_PP_FINALIZE_5(TYPE, _0, _1, _2, _3, ...) \
    CONCEPT_PP_FINALIZE_IMPL((CONCEPT_PP_EXPAND _0 CONCEPT_PP_EXPAND _1 CONCEPT_PP_EXPAND _2 CONCEPT_PP_EXPAND _3), CONCEPT_PP_EXPAND TYPE)

#define CONCEPT_PP_TOLIST_1(...) \
    CONCEPT_PP_IIF(CONCEPT_PP_IS_KEYWORD(__VA_ARGS__))(\
        CONCEPT_PP_TOLIST_2, (1, __VA_ARGS__)), CONCEPT_PP_CAT(CONCEPT_PP_TOLIST_, __VA_ARGS__)
#define CONCEPT_PP_TOLIST_2(_0, ...) \
    CONCEPT_PP_IIF(CONCEPT_PP_IS_KEYWORD(__VA_ARGS__))(\
        CONCEPT_PP_TOLIST_3, (2, __VA_ARGS__)), _0, CONCEPT_PP_CAT(CONCEPT_PP_TOLIST_, __VA_ARGS__)
#define CONCEPT_PP_TOLIST_3(_0, _1, ...) \
    CONCEPT_PP_IIF(CONCEPT_PP_IS_KEYWORD(__VA_ARGS__))(\
        CONCEPT_PP_TOLIST_4, (3, __VA_ARGS__)), _0, _1, CONCEPT_PP_CAT(CONCEPT_PP_TOLIST_, __VA_ARGS__)
#define CONCEPT_PP_TOLIST_4(_0, _1, _2, ...) \
    CONCEPT_PP_IIF(CONCEPT_PP_IS_KEYWORD(__VA_ARGS__))(\
        CONCEPT_PP_TOLIST_5, (4, __VA_ARGS__)), _0, _1, _2, CONCEPT_PP_CAT(CONCEPT_PP_TOLIST_, __VA_ARGS__)
#define CONCEPT_PP_TOLIST_5(_0, _1, _2, _3, ...) \
    (5, __VA_ARGS__), _0, _1, _2, _3,

#define CONCEPT_PP_TOLIST_attribute(...) (__VA_ARGS__),
#define CONCEPT_PP_TOLIST_attributes(...) (__VA_ARGS__),
#define CONCEPT_PP_TOLIST_static (static),
#define CONCEPT_PP_TOLIST_inline (inline),
#define CONCEPT_PP_TOLIST_constexpr (constexpr),
#define CONCEPT_PP_TOLIST_explicit (explicit),
#define CONCEPT_PP_TOLIST_friend (friend),
#define CONCEPT_PP_TOLIST_auto (auto)
#define CONCEPT_PP_TOLIST_implicit ()
#define CONCEPT_PP_TOLIST_decltype(...) (decltype(__VA_ARGS__))

#define CONCEPT_PP_REQUIRES_(...) \
    CONCEPT_PP_CAT(CONCEPT_PP_REQUIRES_, __VA_ARGS__) ) CONCEPT_PP_RETURNS_
#define CONCEPT_PP_REQUIRES_requires
#define CONCEPT_PP_RETURNS_(...) \
    CONCEPT_PP_FINALIZE(CONCEPT_PP_EVAL_OR(CONCEPT_PP_EVAL_OR(\
        CONCEPT_PP_EVAL_OR(CONCEPT_PP_EVAL_OR(\
            CONCEPT_PP_TOLIST_1(__VA_ARGS__))))))

#define CONCEPT_template(...)                                                   \
    template<                                                                   \
        __VA_ARGS__                                                             \
        CONCEPT_PP_COMMA_IIF(CONCEPT_PP_IS_EMPTY_NON_FUNCTION(__VA_ARGS__))     \
        int (*_concept_unique_)[CONCEPT_COUNTER] = nullptr,                     \
        bool _concept_requires_ = (_concept_unique_ == nullptr) &&              \
            static_cast<bool>(CONCEPT_PP_REQUIRES_                              \
    /**/

////////////////////////////////////////////////////////////////////////////////
// CONCEPT_requires
#define CONCEPT_requires(...)                                                   \
    CONCEPT_template()(requires __VA_ARGS__)                                    \
    /**/

#define CONCEPT_assert(...) static_assert((bool) (__VA_ARGS__), "Concept assertion failed : " #__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// CONCEPT_def
//   For defining concepts with a syntax symilar to C++20. For example:
//
//     CONCEPT_def(
//         // The Assignable concept from the Ranges TS
//         template(class T, class U)
//         concept Assignable,
//             requires (T t, U &&u) {
//                 (t = (U &&) u) ->* Same<_&&, T>()
//             } &&
//             True<std::is_lvalue_reference<T>>()
//     );
#define CONCEPT_def(DECL, ...)                                                  \
    CONCEPT_PP_EVAL(                                                            \
        CONCEPT_PP_DECL_DEF,                                                    \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_DECL_, DECL),                             \
        __VA_ARGS__,                                                            \
        CONCEPT_PP_EAT)                                                         \
    /**/
#define CONCEPT_PP_DECL_DEF_NAME(...) __VA_ARGS__,
#define CONCEPT_PP_DECL_DEF(TPARAM, NAME, REQUIRES, ...)                        \
    CONCEPT_PP_CAT(CONCEPT_PP_DECL_DEF_, CONCEPT_PP_IS_PAREN(NAME))(            \
        TPARAM,                                                                 \
        NAME,                                                                   \
        REQUIRES,                                                               \
        __VA_ARGS__)                                                            \
    /**/
// The defn is of the form:
//   template(class A, class B = void, class... Rest)
//   (concept Name)(A, B, Rest...),
//      // requirements...
#define CONCEPT_PP_DECL_DEF_1(TPARAM, NAME, REQUIRES, ...)                      \
    CONCEPT_PP_EVAL4(                                                           \
        CONCEPT_PP_DECL_DEF_IMPL,                                               \
        TPARAM,                                                                 \
        CONCEPT_PP_DECL_DEF_NAME NAME,                                          \
        REQUIRES,                                                               \
        __VA_ARGS__)                                                            \
    /**/
// The defn is of the form:
//   template(class A, class B)
//   concept Name,
//      // requirements...
// Compute the template arguments (A, B) from the template introducer.
#define CONCEPT_PP_DECL_DEF_0(TPARAM, NAME, REQUIRES, ...)                      \
    CONCEPT_PP_DECL_DEF_IMPL(                                                   \
        TPARAM,                                                                 \
        NAME,                                                                   \
        (CONCEPT_PP_CAT(CONCEPT_PP_AUX_, TPARAM)),                              \
        REQUIRES,                                                               \
        __VA_ARGS__)                                                            \
    /**/
// Expand the template definition into a struct and template alias like:
//    struct NameConcept {
//      template<class A, class B>
//      auto requires_(/*requirement expression args (optional)*/) -> decltype(
//          dummy{} && /*requirements...*/)
//    };
//    template<class A, class B>
//    using Name = is_satisfied_by<NameConcept, A, B>;
#define CONCEPT_PP_DECL_DEF_IMPL(TPARAM, NAME, ARGS, REQUIRES, ...)             \
    struct CONCEPT_PP_CAT(CONCEPT_PP_CAT(CONCEPT_PP_DEF_, NAME), Concept) {     \
        using _ = ::concepts::_;                                                \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                 \
        auto requires_ CONCEPT_PP_EVAL2(                                        \
            CONCEPT_PP_DEF_WRAP,                                                \
            CONCEPT_PP_CAT(CONCEPT_PP_DEF_, REQUIRES),                          \
            REQUIRES,                                                           \
            (__VA_ARGS__))(~) void());                                          \
    };                                                                          \
    CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                     \
    using CONCEPT_PP_CAT(CONCEPT_PP_DEF_, NAME) =                               \
        ::concepts::is_satisfied_by<                                            \
            CONCEPT_PP_CAT(CONCEPT_PP_CAT(CONCEPT_PP_DEF_, NAME), Concept),     \
            CONCEPT_PP_EXPAND ARGS>                                             \
    /**/

#define CONCEPT_PP_DEF_DECL_template(...)                                       \
    template(__VA_ARGS__),                                                      \
    /**/
#define CONCEPT_PP_DEF_template(...)                                            \
    template<__VA_ARGS__>                                                       \
    /**/
#define CONCEPT_PP_DEF_concept
#define CONCEPT_PP_DEF_class
#define CONCEPT_PP_DEF_typename
#define CONCEPT_PP_DEF_int
#define CONCEPT_PP_DEF_size_t
#define CONCEPT_PP_DEF_unsigned
#define CONCEPT_PP_DEF_requires ~,
#define CONCEPT_PP_AUX_template(...)                                            \
    CONCEPT_PP_CAT2(                                                            \
        CONCEPT_PP_TPARAM_,                                                     \
        CONCEPT_PP_COUNT(__VA_ARGS__))(__VA_ARGS__)                             \
    /**/
#define CONCEPT_PP_TPARAM_1(_1)                                                 \
    CONCEPT_PP_CAT2(CONCEPT_PP_DEF_, _1)
#define CONCEPT_PP_TPARAM_2(_1, ...)                                            \
    CONCEPT_PP_CAT2(CONCEPT_PP_DEF_, _1), CONCEPT_PP_TPARAM_1(__VA_ARGS__)
#define CONCEPT_PP_TPARAM_3(_1, ...)                                            \
    CONCEPT_PP_CAT2(CONCEPT_PP_DEF_, _1), CONCEPT_PP_TPARAM_2(__VA_ARGS__)
#define CONCEPT_PP_TPARAM_4(_1, ...)                                            \
    CONCEPT_PP_CAT2(CONCEPT_PP_DEF_, _1), CONCEPT_PP_TPARAM_3(__VA_ARGS__)
#define CONCEPT_PP_TPARAM_5(_1, ...)                                            \
    CONCEPT_PP_CAT2(CONCEPT_PP_DEF_, _1), CONCEPT_PP_TPARAM_4(__VA_ARGS__)

#define CONCEPT_PP_DEF_WRAP(X, Y, ...)                                          \
    CONCEPT_PP_EVAL3(                                                           \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_WRAP_, CONCEPT_PP_COUNT(__VA_ARGS__)),    \
        X,                                                                      \
        Y,                                                                      \
        __VA_ARGS__)                                                            \
    /**/

// No requires expression:
#define CONCEPT_PP_DEF_WRAP_1(_, HEAD, TAIL)                                    \
    () -> decltype(::concepts::detail::dummy{} && HEAD, CONCEPT_PP_EXPAND TAIL  \
    /**/
// Requires expression:
#define CONCEPT_PP_DEF_WRAP_2(_a, HEAD, _b, TAIL)                               \
    CONCEPT_PP_CAT(CONCEPT_PP_DEF_REQUIRES_, CONCEPT_PP_IS_PAREN(HEAD)) HEAD,   \
    CONCEPT_PP_EXPAND TAIL                                                      \
    /**/
// Requires expression without a requirement parameter list:
#define CONCEPT_PP_DEF_REQUIRES_0                                               \
    () -> decltype(::concepts::detail::dummy                                    \
    /**/
// Requires expression with a requirement parameter list:
#define CONCEPT_PP_DEF_REQUIRES_1(...)                                          \
    (__VA_ARGS__) -> decltype(::concepts::detail::dummy                         \
    /**/

namespace concepts
{
    inline namespace v1
    {
        /// \cond
        template<typename T>
        struct bool_;

        namespace detail
        {
            template<typename T>
            T gcc_bugs_bugs_bugs(T);

            template<bool If>
            struct eval_if
            {
                template<typename Else>
                constexpr bool operator()(bool then_, Else) const noexcept
                {
                    return then_;
                }
            };

            template<>
            struct eval_if<false>
            {
                template<typename Else>
                constexpr bool operator()(bool, Else else_) const noexcept
                {
                    return static_cast<bool>(else_);
                }
            };

            template<typename T>
            struct id
            {
                int operator()(T);
            };

            template<typename...>
            struct undef;

            struct dummy
            {
                template<typename... Ts>
                constexpr dummy(Ts &&...) noexcept
                {}
                template<typename That>
                constexpr friend typename std::enable_if<(bool)That(), dummy>::type
                operator&&(dummy, bool_<That>) noexcept
                {
                    return {};
                }
            };
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{
        ///

        // For short-cirtuit evaluation of requirements:
        template<class T>
        struct bool_
        {
            using type = bool_;
            using value_type = bool;
            constexpr /*implicit*/ operator bool() const noexcept
            {
                return static_cast<bool>(T());
            }
            constexpr bool operator()() const noexcept
            {
                return static_cast<bool>(*this);
            }
            template<bool B, typename T_ = T, typename std::enable_if<B == (bool)T_(), int>::type = 0>
            constexpr /*implicit*/ operator meta::bool_<B>() const noexcept
            {
                return {};
            }
            static constexpr struct value_t
            {
                constexpr /*implicit*/ operator bool() const noexcept
                {
                    return static_cast<bool>(T());
                }
            } value{};
        };

        template<typename T, typename U>
        struct and_
        {
            constexpr explicit operator bool() const noexcept
            {
                return detail::eval_if<!static_cast<bool>(T())>()(false, U());
            }
            constexpr bool operator()() const noexcept
            {
                return static_cast<bool>(*this);
            }
        };

        template<typename T, typename U>
        struct or_
        {
            constexpr explicit operator bool() const noexcept
            {
                return detail::eval_if<static_cast<bool>(T())>()(true, U());
            }
            constexpr bool operator()() const noexcept
            {
                return static_cast<bool>(*this);
            }
        };

        template<typename T>
        struct not_
        {
            constexpr explicit operator bool() const noexcept
            {
                return !static_cast<bool>(T());
            }
            constexpr bool operator()() const noexcept
            {
                return static_cast<bool>(*this);
            }
        };

        template<typename T, typename U>
        constexpr bool_<and_<T, U>> operator&&(bool_<T>, bool_<U>)
        {
            return {};
        }

        template<typename T, typename U>
        constexpr detail::undef<T, U> operator&&(bool_<T>, U);
        template<typename T, typename U>
        constexpr detail::undef<T, U> operator&&(T, bool_<U>);

        template<typename T, typename U>
        constexpr bool_<or_<T, U>> operator||(bool_<T>, bool_<U>)
        {
            return {};
        }

        template<typename T, typename U>
        constexpr detail::undef<T, U> operator||(bool_<T>, U);
        template<typename T, typename U>
        constexpr detail::undef<T, U> operator||(T, bool_<U>);

        template<typename T>
        constexpr bool_<not_<T>> operator!(bool_<T>)
        {
            return {};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // is_satisfied_by
        template<typename Concept, typename...Ts>
        struct is_satisfied_by_
        {
            template<typename C = Concept,
                typename = decltype(detail::gcc_bugs_bugs_bugs(&C::template requires_<Ts...>))>
            constexpr explicit operator bool() const noexcept
            {
                return true;
            }
            constexpr explicit operator bool() const volatile noexcept
            {
                return false;
            }
            constexpr bool operator()() const noexcept
            {
                return static_cast<bool>(*this);
            }
        };

        template<typename Concept, typename...Ts>
        using is_satisfied_by = bool_<is_satisfied_by_<Concept, Ts...>>;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // placeholder (_):
        struct _ {};

        template<typename T>
        inline detail::id<T> type()
        {
            return {};
        }

        // For placeholder requirements, like:
        //      ( a.foo() ) ->* Concept< _ [const][ref], Args...>()
        template<class T, class Concept, class... Args>
        typename std::enable_if<(bool)is_satisfied_by_<Concept, T, Args...>(), int>::type
        operator->*(T, is_satisfied_by<Concept, _, Args...>);

        template<class T, class Concept, class... Args>
        typename std::enable_if<(bool)is_satisfied_by_<Concept, T, Args...>(), int>::type
        operator->*(T &&, is_satisfied_by<Concept, _ &&, Args...>);

        template<class T, class Concept, class... Args>
        typename std::enable_if<(bool)is_satisfied_by_<Concept, T, Args...>(), int>::type
        operator->*(T &, is_satisfied_by<Concept, _ &, Args...>);

        template<class T, class Concept, class... Args>
        typename std::enable_if<(bool)is_satisfied_by_<Concept, T, Args...>(), int>::type
        operator->*(T const &, is_satisfied_by<Concept, _ const &, Args...>);

        template<class T, class U>
        auto operator->*(T &&t, detail::id<U> p) -> decltype(p((T &&) t));

        // For automatically generating tags corresponding to concept
        // subsumption relationships, for use with tag dispatching.
        template<typename Concept, typename Base = meta::nil_>
        struct tag
          : Base
        {};

        template<typename Concepts, typename... Ts>
        using tag_of =
            meta::reverse_fold<
                meta::find_if<
                    Concepts,
                    meta::bind_back<meta::quote<is_satisfied_by>, Ts...>>,
                meta::nil_,
                meta::flip<meta::quote<tag>>>;

        inline namespace concept_defns
        {
            ////////////////////////////////////////////////////////////////////////////////////////
            // Utility concepts
            ////////////////////////////////////////////////////////////////////////////////////////

            template<bool B>
            constexpr concepts::bool_<meta::bool_<B>> True()
            {
                return {};
            }

            template<typename T, typename = int T::*> // should be a class
            constexpr concepts::bool_<T> True()
            {
                return {};
            }

            template<typename T, typename = int T::*> // should be a class
            constexpr concepts::bool_<T> True(T)
            {
                return {};
            }

            template<template<typename...> class T, typename... Args>
            constexpr concepts::bool_<meta::defer<T, Args...>> True() noexcept
            {
                return {};
            }

            ////////////////////////////////////////////////////////////////////////////////////////
            // Core language concepts
            ////////////////////////////////////////////////////////////////////////////////////////

            CONCEPT_def
            (
                template(typename A, typename B)
                concept Same,
                    True<CONCEPT_PP_IS_SAME(A, B)>()
            );

            /// \cond
            CONCEPT_def
            (
                template(typename From, typename To)
                concept ImplicitlyConvertibleTo,
                    True(std::is_convertible<From, To>())
            );

            CONCEPT_def
            (
                template(typename From, typename To)
                concept ExplicitlyConvertibleTo,
                    requires (From (&from)())
                    {
                        (static_cast<To>(from()), 0) // Because To could be void
                    }
            );
            /// \endcond

            CONCEPT_def
            (
                template(typename From, typename To)
                concept ConvertibleTo,
                    ImplicitlyConvertibleTo<From, To>() &&
                    ExplicitlyConvertibleTo<From, To>()
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept DerivedFrom,
                    True(std::is_base_of<U, T>()) &&
                    ConvertibleTo<T const volatile *, U const volatile *>()
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept CommonReference,
                    Same<common_reference_t<T, U>, common_reference_t<U, T>>() &&
                    ConvertibleTo<T, common_reference_t<T, U>>() &&
                    ConvertibleTo<U, common_reference_t<T, U>>()
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept Common,
                    Same<common_type_t<T, U>, common_type_t<U, T>>() &&
                    ConvertibleTo<T, common_type_t<T, U>>() &&
                    ConvertibleTo<U, common_type_t<T, U>>() &&
                    CommonReference<
                        typename std::add_lvalue_reference<T const>::type,
                        typename std::add_lvalue_reference<U const>::type>() &&
                    CommonReference<
                        typename std::add_lvalue_reference<common_type_t<T, U>>::type,
                        common_reference_t<
                            typename std::add_lvalue_reference<T const>::type,
                            typename std::add_lvalue_reference<U const>::type>>()
            );

            CONCEPT_def
            (
                template(typename T)
                concept Integral,
                    True(std::is_integral<T>())
            );

            CONCEPT_def
            (
                template(typename T)
                concept SignedIntegral,
                    Integral<T>() &&
                    True(std::is_signed<T>())
            );

            CONCEPT_def
            (
                template(typename T)
                concept UnsignedIntegral,
                    Integral<T>() &&
                    !SignedIntegral<T>()
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept Assignable,
                    requires (T t, U &&u)
                    {
                        (t = (U &&) u) ->* Same<_&&, T>()
                    } &&
                    True(std::is_lvalue_reference<T>())
            );

            CONCEPT_def
            (
                template(typename T)
                concept Swappable,
                    requires (T &t, T &u)
                    {
                        (concepts::swap(t, u), 0)
                    }
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept SwappableWith,
                    requires (T &&t, U &&u)
                    {
                        (concepts::swap((T &&) t, (T &&) t), 0),
                        (concepts::swap((U &&) u, (U &&) u), 0),
                        (concepts::swap((U &&) u, (T &&) t), 0),
                        (concepts::swap((T &&) t, (U &&) u), 0)
                    } &&
                    CommonReference<detail::as_cref_t<T>, detail::as_cref_t<U>>()
            );

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Comparison concepts
            ////////////////////////////////////////////////////////////////////////////////////////////
            CONCEPT_def
            (
                template(typename T, typename U)
                concept WeaklyEqualityComparableWith,
                    requires (detail::as_cref_t<T> t, detail::as_cref_t<U> u)
                    {
                        (t == u) ? 1 : 0,
                        (t != u) ? 1 : 0,
                        (u == t) ? 1 : 0,
                        (u != t) ? 1 : 0
                    }
            );

            CONCEPT_def
            (
                template(typename T)
                concept EqualityComparable,
                    WeaklyEqualityComparableWith<T, T>()
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept EqualityComparableWith,
                    EqualityComparable<T>() &&
                    EqualityComparable<U>() &&
                    WeaklyEqualityComparableWith<T, U>() &&
                    CommonReference<detail::as_cref_t<T>, detail::as_cref_t<U>>() &&
                    EqualityComparable<
                        common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>()
            );

            CONCEPT_def
            (
                template(typename T)
                concept StrictTotallyOrdered,
                    requires (detail::as_cref_t<T> t, detail::as_cref_t<T> u)
                    {
                        (t < u) ? 1 : 0,
                        (t > u) ? 1 : 0,
                        (u <= t) ? 1 : 0,
                        (u >= t) ? 1 : 0
                    } &&
                    EqualityComparable<T>()
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept StrictTotallyOrderedWith,
                    requires (detail::as_cref_t<T> t, detail::as_cref_t<U> u)
                    {
                        (t < u) ? 1 : 0,
                        (t > u) ? 1 : 0,
                        (t <= u) ? 1 : 0,
                        (t >= u) ? 1 : 0,
                        (u < t) ? 1 : 0,
                        (u > t) ? 1 : 0,
                        (u <= t) ? 1 : 0,
                        (u >= t) ? 1 : 0
                    } &&
                    StrictTotallyOrdered<T>() &&
                    StrictTotallyOrdered<U>() &&
                    EqualityComparableWith<T, U>() &&
                    CommonReference<detail::as_cref_t<T>, detail::as_cref_t<U>>() &&
                    StrictTotallyOrdered<
                        common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>()
            );

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Object concepts
            ////////////////////////////////////////////////////////////////////////////////////////////

            CONCEPT_def
            (
                template(typename T)
                concept Destructible,
                    True(std::is_nothrow_destructible<T>())
            );

            CONCEPT_def
            (
                template(typename T, typename... Args)
                (concept Constructible)(T, Args...),
                    Destructible<T>() &&
                    True(std::is_constructible<T, Args...>())
            );

            CONCEPT_def
            (
                template(typename T)
                concept DefaultConstructible,
                    Constructible<T>()
            );

            CONCEPT_def
            (
                template(typename T)
                concept MoveConstructible,
                    Constructible<T, T>() &&
                    ConvertibleTo<T, T>()
            );

            CONCEPT_def
            (
                template(typename T)
                concept CopyConstructible,
                    MoveConstructible<T>() &&
                    Constructible<T, T &>() &&
                    Constructible<T, T const &>() &&
                    Constructible<T, T const>() &&
                    ConvertibleTo<T &, T>() &&
                    ConvertibleTo<T const &, T>() &&
                    ConvertibleTo<T const, T>()
            );

            CONCEPT_def
            (
                template(typename T)
                concept Movable,
                    MoveConstructible<T>() &&
                    True(std::is_object<T>()) &&
                    Assignable<T &, T>() &&
                    Swappable<T>()
            );

            CONCEPT_def
            (
                template(typename T)
                concept Copyable,
                    Movable<T>() &&
                    CopyConstructible<T>() &&
                    True(std::is_object<T>()) &&
                    // Spec requires this to be validated
                    Assignable<T &, T const &>() &&
                    // Spec does not require these to be validated
                    Assignable<T &, T &>() &&
                    Assignable<T &, T const>()
            );

            CONCEPT_def
            (
                template(typename T)
                concept Semiregular,
                    Copyable<T>() &&
                    DefaultConstructible<T>()
                // Axiom: copies are independent. See Fundamentals of Generic Programming
                // http://www.stepanovpapers.com/DeSt98.pdf
            );

            CONCEPT_def
            (
                template(typename T)
                concept Regular,
                    Semiregular<T>() &&
                    EqualityComparable<T>()
            );
        }

    } // inline namespace v1
} // namespace concepts

/// @}

#if defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
