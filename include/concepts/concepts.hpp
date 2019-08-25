/// \file
//  CPP, the Concepts PreProcessor library
//
//  Copyright Eric Niebler 2018-present
//  Copyright (c) 2018-present, Facebook, Inc.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef CPP_CONCEPTS_HPP
#define CPP_CONCEPTS_HPP

// clang-format off

#include <initializer_list>
#include <utility>
#include <type_traits>
#include <concepts/swap.hpp>
#include <concepts/type_traits.hpp>

// disable buggy compatibility warning about "requires" and "concept" being
// C++20 keywords.
#if defined(__clang__) || defined(__GNUC__)
#define CPP_PP_IGNORE_CXX2A_COMPAT_BEGIN                                        \
    _Pragma("GCC diagnostic push")                                              \
    _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"")                     \
    _Pragma("GCC diagnostic ignored \"-Wpragmas\"")                             \
    _Pragma("GCC diagnostic ignored \"-Wc++2a-compat\"")                        \
    _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")                         \
    /**/
#define CPP_PP_IGNORE_CXX2A_COMPAT_END                                          \
    _Pragma("GCC diagnostic pop")
#else
#define CPP_PP_IGNORE_CXX2A_COMPAT_BEGIN
#define CPP_PP_IGNORE_CXX2A_COMPAT_END
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define CPP_WORKAROUND_MSVC_779763 // FATAL_UNREACHABLE calling constexpr function via template parameter
#define CPP_WORKAROUND_MSVC_780775 // Incorrect substitution in function template return type
#define CPP_WORKAROUND_MSVC_784772 // Failure to invoke *explicit* bool conversion in a constant expression
#endif

#if !defined(CPP_CXX_CONCEPTS)
#ifdef CPP_DOXYGEN_INVOKED
#define CPP_CXX_CONCEPTS 201800L
#elif defined(__cpp_concepts) && __cpp_concepts > 0
// gcc-6 concepts are too buggy to use
#if !defined(__GNUC__) || defined(__clang__) || __GNUC__ >= 7
#define CPP_CXX_CONCEPTS __cpp_concepts
#else
#define CPP_CXX_CONCEPTS 0L
#endif
#else
#define CPP_CXX_CONCEPTS 0L
#endif
#endif

CPP_PP_IGNORE_CXX2A_COMPAT_BEGIN

#define CPP_PP_CHECK(...) CPP_PP_CHECK_N(__VA_ARGS__, 0,)
#define CPP_PP_CHECK_N(x, n, ...) n
#define CPP_PP_PROBE(x) x, 1,
#define CPP_PP_PROBE_N(x, n) x, n,

// CPP_CXX_VA_OPT
#ifndef CPP_CXX_VA_OPT
#if __cplusplus > 201703L
#define CPP_CXX_VA_OPT_(...) CPP_PP_CHECK(__VA_OPT__(,) 1)
#define CPP_CXX_VA_OPT CPP_CXX_VA_OPT_(~)
#else
#define CPP_CXX_VA_OPT 0
#endif
#endif // CPP_CXX_VA_OPT

#define CPP_PP_CAT_(X, ...)  X ## __VA_ARGS__
#define CPP_PP_CAT(X, ...)   CPP_PP_CAT_(X, __VA_ARGS__)
#define CPP_PP_CAT2_(X, ...) X ## __VA_ARGS__
#define CPP_PP_CAT2(X, ...)  CPP_PP_CAT2_(X, __VA_ARGS__)

#define CPP_PP_EVAL(X, ...) X(__VA_ARGS__)
#define CPP_PP_EVAL2(X, ...) X(__VA_ARGS__)

#define CPP_PP_EXPAND(...) __VA_ARGS__
#define CPP_PP_EAT(...)

#define CPP_PP_IS_PAREN(x) CPP_PP_CHECK(CPP_PP_IS_PAREN_PROBE x)
#define CPP_PP_IS_PAREN_PROBE(...) CPP_PP_PROBE(~)

#define CPP_PP_COUNT(...)                                                       \
    CPP_PP_COUNT_(__VA_ARGS__,                                                  \
        50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,            \
        30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,            \
        10,9,8,7,6,5,4,3,2,1,)                                                  \
        /**/
#define CPP_PP_COUNT_(                                                          \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,                                    \
    _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,                           \
    _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,                           \
    _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,                           \
    _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, N, ...)                   \
    N                                                                           \
    /**/

#define CPP_PP_IIF(BIT) CPP_PP_CAT_(CPP_PP_IIF_, BIT)
#define CPP_PP_IIF_0(TRUE, ...) __VA_ARGS__
#define CPP_PP_IIF_1(TRUE, ...) TRUE

#define CPP_PP_LPAREN (

#define CPP_PP_NOT(BIT) CPP_PP_CAT_(CPP_PP_NOT_, BIT)
#define CPP_PP_NOT_0 1
#define CPP_PP_NOT_1 0

#define CPP_PP_EMPTY()
#define CPP_PP_COMMA() ,
#define CPP_PP_LBRACE() {
#define CPP_PP_RBRACE() }
#define CPP_PP_COMMA_IIF(X)                                                     \
    CPP_PP_IIF(X)(CPP_PP_EMPTY, CPP_PP_COMMA)()                                 \
    /**/

#define CPP_assert(...)                                                         \
    static_assert(static_cast<bool>(__VA_ARGS__),                               \
        "Concept assertion failed : " #__VA_ARGS__)                             \
    /**/
#define CPP_assert_msg static_assert

#ifdef CPP_WORKAROUND_MSVC_784772
#define CPP_EXPLICIT /**/
#else
#define CPP_EXPLICIT explicit
#endif

////////////////////////////////////////////////////////////////////////////////
// CPP_def
//   For defining concepts with a syntax similar to C++20. For example:
//
//     CPP_def(
//         // The assignable_from concept from the C++20
//         template(typename T, typename U)
//         concept assignable_from,
//             requires (T t, U &&u) (
//                 t = (U &&) u,
//                 ::concepts::requires_<same_as<decltype(t = (U &&) u), T>>
//             ) &&
//             std::is_lvalue_reference_v<T>
//     );
#define CPP_def(DECL, ...)                                                      \
    CPP_PP_EVAL(                                                                \
        CPP_PP_DECL_DEF,                                                        \
        CPP_PP_CAT(CPP_PP_DEF_DECL_, DECL),                                     \
        __VA_ARGS__)                                                            \
    /**/
#define CPP_PP_DECL_DEF_NAME(...)                                               \
    CPP_PP_CAT(CPP_PP_DEF_, __VA_ARGS__),                                       \
    /**/
#define CPP_PP_DECL_DEF(TPARAM, NAME, ...)                                      \
    CPP_PP_CAT(CPP_PP_DECL_DEF_, CPP_PP_IS_PAREN(NAME))(                        \
        TPARAM,                                                                 \
        NAME,                                                                   \
        __VA_ARGS__)                                                            \
    /**/
// The defn is of the form:
//   template(typename A, typename B = void, typename... Rest)
//   (concept Name)(A, B, Rest...),
//      // requirements...
#define CPP_PP_DECL_DEF_1(TPARAM, NAME, ...)                                    \
    CPP_PP_EVAL2(                                                               \
        CPP_PP_DECL_DEF_IMPL,                                                   \
        TPARAM,                                                                 \
        CPP_PP_DECL_DEF_NAME NAME,                                              \
        __VA_ARGS__)                                                            \
    /**/
// The defn is of the form:
//   template(typename A, typename B)
//   concept Name,
//      // requirements...
// Compute the template arguments (A, B) from the template introducer.
#define CPP_PP_DECL_DEF_0(TPARAM, NAME, ...)                                    \
    CPP_PP_DECL_DEF_IMPL(                                                       \
        TPARAM,                                                                 \
        CPP_PP_CAT(CPP_PP_DEF_, NAME),                                          \
        (CPP_PP_CAT(CPP_PP_AUX_, TPARAM)),                                      \
        __VA_ARGS__)                                                            \
    /**/
// Expand the template definition into a struct and template alias like:
//    struct NameConcept {
//      template<typename A, typename B>
//      static auto Requires_(/* args (optional)*/) ->
//          decltype(/*requirements...*/);
//      template<typename A, typename B>
//      static constexpr auto is_satisfied_by(int) ->
//          decltype(bool(&Requires_<A,B>)) { return true; }
//      template<typename A, typename B>
//      static constexpr bool is_satisfied_by(long) { return false; }
//    };
//    template<typename A, typename B>
//    inline constexpr bool Name = NameConcept::is_satisfied_by<A, B>(0);
#if CPP_CXX_CONCEPTS
// No requires expression
#define CPP_PP_DEF_IMPL_0(...)                                                  \
    __VA_ARGS__                                                                 \
    /**/
// Requires expression
#define CPP_PP_DEF_IMPL_1(...)                                                  \
    CPP_PP_CAT(CPP_PP_DEF_IMPL_1_, __VA_ARGS__)                                 \
    /**/
#define CPP_PP_DEF_IMPL_1_requires                                              \
    requires CPP_PP_DEF_IMPL_1_REQUIRES                                         \
    /**/
#define CPP_PP_DEF_IMPL_1_REQUIRES(...)                                         \
    (__VA_ARGS__) CPP_PP_DEF_IMPL_1_REQUIRES_BODY                               \
    /**/
#define CPP_PP_DEF_IMPL_1_REQUIRES_BODY(...)                                    \
    { __VA_ARGS__; }                                                            \
    /**/
#ifdef CPP_DOXYGEN_INVOKED
#define CPP_PP_DECL_DEF_IMPL(TPARAM, NAME, ARGS, ...)                           \
    CPP_PP_CAT(CPP_PP_DEF_, TPARAM)                                             \
    META_CONCEPT NAME = CPP_PP_DEF_IMPL(__VA_ARGS__,)(__VA_ARGS__)              \
    /**/
#else
#define CPP_PP_DECL_DEF_IMPL(TPARAM, NAME, ARGS, ...)                           \
    inline namespace _eager_ {                                                  \
        CPP_PP_CAT(CPP_PP_DEF_, TPARAM)                                         \
        META_CONCEPT NAME = CPP_PP_DEF_IMPL(__VA_ARGS__,)(__VA_ARGS__);         \
    }                                                                           \
    struct CPP_PP_CAT(NAME, _concept) {                                         \
        CPP_PP_CAT(CPP_PP_DEF_, TPARAM)                                         \
        struct Eval {                                                           \
            using Concept = CPP_PP_CAT(NAME, _concept);                         \
            CPP_EXPLICIT constexpr operator bool() const noexcept {             \
                return (bool) _eager_::NAME<CPP_PP_EXPAND ARGS>;                \
            }                                                                   \
            constexpr auto operator!() const noexcept {                         \
                return ::concepts::detail::not_<Eval>{};                        \
            }                                                                   \
            template<typename That>                                             \
            constexpr auto operator&&(That) const noexcept {                    \
                return ::concepts::detail::and_<Eval, That>{};                  \
            }                                                                   \
        };                                                                      \
    };                                                                          \
    namespace lazy {                                                            \
        CPP_PP_CAT(CPP_PP_DEF_, TPARAM)                                         \
        CPP_INLINE_VAR constexpr auto NAME =                                    \
            CPP_PP_CAT(NAME, _concept)::Eval<CPP_PP_EXPAND ARGS>{};             \
    }                                                                           \
    namespace defer {                                                           \
        using namespace _eager_;                                                \
    }                                                                           \
    using _concepts_int_ = int                                                  \
    /**/
#endif
#else
// No requires expression:
#define CPP_PP_DEF_IMPL_0(...)                                                  \
    () -> ::concepts::detail::enable_if_t<int, static_cast<bool>(__VA_ARGS__)>  \
    /**/
// Requires expression:
#define CPP_PP_DEF_IMPL_1(...)                                                  \
    CPP_PP_CAT(CPP_PP_DEF_IMPL_1_, __VA_ARGS__) )>                              \
    /**/
#define CPP_PP_DEF_IMPL_1_requires(...)                                         \
    (__VA_ARGS__) -> ::concepts::detail::enable_if_t<int,                       \
        static_cast<bool>(::concepts::detail::requires_ CPP_PP_DEF_REQUIRES_BODY \
    /**/
 #define CPP_PP_DEF_REQUIRES_BODY(...)                                          \
    <decltype(__VA_ARGS__, void())>()                                           \
    /**/
#ifdef CPP_WORKAROUND_MSVC_780775
#define CPP_PP_DECL_DEF_IMPL_HACK(ARGS)                                         \
    template<typename C_ = Concept,                                             \
        decltype(&C_::template Requires_<CPP_PP_EXPAND ARGS>) = nullptr>        \
    static constexpr bool impl(int) noexcept { return true; }                   \
    /**/
#else
#define CPP_PP_DECL_DEF_IMPL_HACK(ARGS)                                         \
    template<typename C_ = Concept>                                             \
    static constexpr decltype(                                                  \
        &C_::template Requires_<CPP_PP_EXPAND ARGS>, true)                      \
    impl(int) noexcept { return true; }                                         \
    /**/
#endif
#define CPP_PP_DECL_DEF_IMPL(TPARAM, NAME, ARGS, ...)                           \
    struct CPP_PP_CAT(NAME, _concept) {                                         \
        using Concept = CPP_PP_CAT(NAME, _concept);                             \
        CPP_PP_IGNORE_CXX2A_COMPAT_BEGIN                                        \
        CPP_PP_CAT(CPP_PP_DEF_, TPARAM)                                         \
        static auto Requires_ CPP_PP_DEF_IMPL(__VA_ARGS__,)(__VA_ARGS__);       \
        CPP_PP_IGNORE_CXX2A_COMPAT_END                                          \
        CPP_PP_CAT(CPP_PP_DEF_, TPARAM)                                         \
        struct Eval {                                                           \
            CPP_PP_DECL_DEF_IMPL_HACK(ARGS)                                     \
            static constexpr bool impl(long) noexcept { return false; }         \
            CPP_EXPLICIT constexpr operator bool() const noexcept {             \
                return Eval::impl(0);                                           \
            }                                                                   \
            constexpr auto operator!() const noexcept {                         \
                return ::concepts::detail::not_<Eval>{};                        \
            }                                                                   \
            template<typename That>                                             \
            constexpr auto operator&&(That) const noexcept {                    \
                return ::concepts::detail::and_<Eval, That>{};                  \
            }                                                                   \
        };                                                                      \
    };                                                                          \
    CPP_PP_CAT(CPP_PP_DEF_, TPARAM)                                             \
    CPP_INLINE_VAR constexpr bool NAME =                                        \
        (bool)CPP_PP_CAT(NAME, _concept)::Eval<CPP_PP_EXPAND ARGS>{};           \
    namespace lazy {                                                            \
        CPP_PP_CAT(CPP_PP_DEF_, TPARAM)                                         \
        CPP_INLINE_VAR constexpr auto NAME =                                    \
            CPP_PP_CAT(NAME, _concept)::Eval<CPP_PP_EXPAND ARGS>{};             \
    }                                                                           \
    namespace defer {                                                           \
        using namespace lazy;                                                   \
    }                                                                           \
    using _concepts_int_ = int                                                  \
    /**/
#endif

#define CPP_PP_REQUIRES_PROBE_requires                                          \
    CPP_PP_PROBE(~)                                                             \
    /**/
#define CPP_PP_DEF_IMPL(REQUIRES, ...)                                          \
    CPP_PP_CAT(                                                                 \
        CPP_PP_DEF_IMPL_IS_PAREN_,                                              \
        CPP_PP_IS_PAREN(REQUIRES))(REQUIRES)                                    \
    /**/
#define CPP_PP_DEF_IMPL_IS_PAREN_0(REQUIRES)                                    \
    CPP_PP_CAT(                                                                 \
        CPP_PP_DEF_IMPL_,                                                       \
        CPP_PP_CHECK(CPP_PP_CAT(CPP_PP_REQUIRES_PROBE_, REQUIRES)))             \
    /**/
#define CPP_PP_DEF_IMPL_IS_PAREN_1(REQUIRES)                                    \
    CPP_PP_DEF_IMPL_0                                                           \
    /**/
#define CPP_PP_DEF_DECL_template(...)                                           \
    template(__VA_ARGS__),                                                      \
    /**/
#define CPP_PP_DEF_template(...)                                                \
    template<__VA_ARGS__>                                                       \
    /**/
#define CPP_PP_DEF_concept
#define CPP_PP_DEF_class
#define CPP_PP_DEF_typename
#define CPP_PP_DEF_int
#define CPP_PP_DEF_bool
#define CPP_PP_DEF_size_t
#define CPP_PP_DEF_unsigned
#define CPP_PP_AUX_template(...)                                                \
    CPP_PP_CAT2(                                                                \
        CPP_PP_TPARAM_,                                                         \
        CPP_PP_COUNT(__VA_ARGS__))(__VA_ARGS__)                                 \
    /**/
#define CPP_PP_TPARAM_1(_1)                                                     \
    CPP_PP_CAT2(CPP_PP_DEF_, _1)
#define CPP_PP_TPARAM_2(_1, ...)                                                \
    CPP_PP_CAT2(CPP_PP_DEF_, _1), CPP_PP_TPARAM_1(__VA_ARGS__)
#define CPP_PP_TPARAM_3(_1, ...)                                                \
    CPP_PP_CAT2(CPP_PP_DEF_, _1), CPP_PP_TPARAM_2(__VA_ARGS__)
#define CPP_PP_TPARAM_4(_1, ...)                                                \
    CPP_PP_CAT2(CPP_PP_DEF_, _1), CPP_PP_TPARAM_3(__VA_ARGS__)
#define CPP_PP_TPARAM_5(_1, ...)                                                \
    CPP_PP_CAT2(CPP_PP_DEF_, _1), CPP_PP_TPARAM_4(__VA_ARGS__)

#define CPP_PP_PROBE_EMPTY_PROBE_CPP_PP_PROBE_EMPTY                             \
    CPP_PP_PROBE(~)                                                             \

#define CPP_PP_PROBE_EMPTY()
#define CPP_PP_IS_NOT_EMPTY(...)                                                \
    CPP_PP_CHECK(CPP_PP_CAT(CPP_PP_PROBE_EMPTY_PROBE_,                          \
        CPP_PP_PROBE_EMPTY __VA_ARGS__ ()))                                     \
    /**/

////////////////////////////////////////////////////////////////////////////////
// CPP_template
// Usage:
//   CPP_template(typename A, typename B)
//     (requires Concept1<A> && Concept2<B>)
//   void foo(A a, B b)
//   {}
#if CPP_CXX_CONCEPTS
#define CPP_template(...)                                                       \
    template<__VA_ARGS__> CPP_PP_EXPAND                                         \
    /**/
#define CPP_template_def CPP_template                                           \
    /**/
#define CPP_member
#define CPP_ctor(TYPE) TYPE CPP_CTOR_IMPL_1_
#define CPP_CTOR_IMPL_1_(...)                                                   \
    (__VA_ARGS__) CPP_PP_EXPAND                                                 \
    /**/
#else
#define CPP_template                                                            \
    CPP_template_sfinae                                                         \
    /**/
#define CPP_template_def CPP_template_def_sfinae                                \
    /**/
#define CPP_member CPP_member_sfinae
#define CPP_ctor CPP_ctor_sfinae
#endif

#define CPP_template_sfinae(...)                                                \
    template<__VA_ARGS__ CPP_TEMPLATE_SFINAE_AUX_                               \
    /**/
#define CPP_TEMPLATE_SFINAE_AUX_(...) ,                                         \
    bool CPP_false_ = false,                                                    \
    ::concepts::detail::enable_if_t<int,                                        \
        static_cast<bool>(\
            CPP_PP_CAT(CPP_TEMPLATE_SFINAE_AUX_3_, __VA_ARGS__)) ||             \
        CPP_false_> = 0>                                                        \
    /**/
#define CPP_template_def_sfinae(...)                                            \
    template<__VA_ARGS__ CPP_TEMPLATE_DEF_SFINAE_AUX_                           \
    /**/
#define CPP_TEMPLATE_DEF_SFINAE_AUX_(...) ,                                     \
    bool CPP_false_,                                                            \
    ::concepts::detail::enable_if_t<int,                                        \
        static_cast<bool>(                                                      \
            CPP_PP_CAT(CPP_TEMPLATE_SFINAE_AUX_3_, __VA_ARGS__)) ||             \
        CPP_false_>>                                                            \
    /**/
#define CPP_TEMPLATE_SFINAE_AUX_3_requires
#define CPP_member_sfinae                                                       \
    CPP_broken_friend_member                                                    \
    /**/
#define CPP_ctor_sfinae(TYPE) TYPE CPP_CTOR_SFINAE_IMPL_1_
#define CPP_CTOR_SFINAE_IMPL_1_(...)                                            \
    (__VA_ARGS__                                                                \
        CPP_PP_COMMA_IIF(                                                       \
            CPP_PP_NOT(CPP_PP_IS_NOT_EMPTY(__VA_ARGS__)))                       \
    CPP_CTOR_SFINAE_REQUIRES                                                    \
    /**/
#define CPP_CTOR_SFINAE_PROBE_NOEXCEPT_noexcept                                 \
    CPP_PP_PROBE(~)                                                             \
    /**/
#define CPP_CTOR_SFINAE_MAKE_PROBE(FIRST,...)                                   \
    CPP_PP_CAT(CPP_CTOR_SFINAE_PROBE_NOEXCEPT_, FIRST)                          \
    /**/
#define CPP_CTOR_SFINAE_REQUIRES(...)                                           \
    CPP_PP_CAT(CPP_CTOR_SFINAE_REQUIRES_,                                       \
    CPP_PP_CHECK(CPP_CTOR_SFINAE_MAKE_PROBE(__VA_ARGS__,)))(__VA_ARGS__)        \
    /**/
// No noexcept-clause:
#define CPP_CTOR_SFINAE_REQUIRES_0(...)                                         \
    ::concepts::detail::enable_if_t<                                            \
        ::concepts::detail::Nil,                                                \
        CPP_false(::concepts::detail::xNil{}) ||                                \
        static_cast<bool>(                                                      \
            CPP_PP_CAT(CPP_TEMPLATE_SFINAE_AUX_3_, __VA_ARGS__))> = {})         \
    /**/
// Yes noexcept-clause:
#define CPP_CTOR_SFINAE_REQUIRES_1(...)                                         \
    ::concepts::detail::enable_if_t<                                            \
        ::concepts::detail::Nil,                                                \
        CPP_false(::concepts::detail::xNil{}) ||                                \
        static_cast<bool>(CPP_PP_CAT(CPP_TEMPLATE_SFINAE_AUX_3_, CPP_PP_CAT(    \
                CPP_CTOR_SFINAE_EAT_NOEXCEPT_, __VA_ARGS__)))> = {})            \
        CPP_PP_EXPAND(                                                          \
            CPP_PP_CAT(CPP_CTOR_SFINAE_SHOW_NOEXCEPT_, __VA_ARGS__)))           \
    /**/
#define CPP_CTOR_SFINAE_EAT_NOEXCEPT_noexcept(...)
#define CPP_CTOR_SFINAE_SHOW_NOEXCEPT_noexcept(...)                             \
    noexcept(__VA_ARGS__) CPP_PP_EAT CPP_PP_LPAREN                              \
    /**/

#ifdef CPP_DOXYGEN_INVOKED
#define CPP_broken_friend_ret(...)                                              \
    __VA_ARGS__ CPP_PP_EXPAND                                                   \
    /**/
#else
#define CPP_broken_friend_ret(...)                                              \
    ::concepts::detail::enable_if_t<__VA_ARGS__,                                \
    CPP_BROKEN_FRIEND_RETURN_TYPE_AUX_                                          \
    /**/
#define CPP_BROKEN_FRIEND_RETURN_TYPE_AUX_(...)                                 \
    CPP_BROKEN_FRIEND_RETURN_TYPE_AUX_3_(CPP_PP_CAT(                            \
        CPP_TEMPLATE_AUX_2_, __VA_ARGS__))                                      \
    /**/
#define CPP_TEMPLATE_AUX_2_requires
#define CPP_BROKEN_FRIEND_RETURN_TYPE_AUX_3_(...)                               \
    static_cast<bool>(__VA_ARGS__) || CPP_false(::concepts::detail::xNil{})>    \
    /**/
#ifdef CPP_WORKAROUND_MSVC_779763
#define CPP_broken_friend_member                                                \
    template<::concepts::detail::CPP_false_t const &CPP_false =                 \
        ::concepts::detail::CPP_false_>                                         \
    /**/
#else // ^^^ workaround / no workaround vvv
#define CPP_broken_friend_member                                                \
    template<bool (&CPP_false)(::concepts::detail::xNil) =                      \
        ::concepts::detail::CPP_false>                                          \
    /**/
#endif // CPP_WORKAROUND_MSVC_779763
#endif

#if CPP_CXX_CONCEPTS
#define CPP_ret(...)                                                            \
    __VA_ARGS__ CPP_PP_EXPAND                                                   \
    /**/
#else
#define CPP_ret                                                                 \
    CPP_broken_friend_ret                                                       \
    /**/
#endif

////////////////////////////////////////////////////////////////////////////////
// CPP_fun
#if CPP_CXX_CONCEPTS
#define CPP_FUN_IMPL_1_(...)                                                    \
    (__VA_ARGS__)                                                               \
    CPP_PP_EXPAND                                                               \
    /**/
#define CPP_fun(X) X CPP_FUN_IMPL_1_
#else
#define CPP_FUN_IMPL_1_(...)                                                    \
    (__VA_ARGS__                                                                \
        CPP_PP_COMMA_IIF(                                                       \
            CPP_PP_NOT(CPP_PP_IS_NOT_EMPTY(__VA_ARGS__)))                       \
    CPP_FUN_IMPL_REQUIRES                                                       \
    /**/

#define CPP_FUN_IMPL_REQUIRES(...)                                              \
    CPP_FUN_IMPL_SELECT_CONST_(__VA_ARGS__,)(__VA_ARGS__)                       \
    /**/

#define CPP_FUN_IMPL_SELECT_CONST_(MAYBE_CONST, ...)                            \
    CPP_PP_CAT(CPP_FUN_IMPL_SELECT_CONST_,                                      \
        CPP_PP_CHECK(CPP_PP_CAT(                                                \
            CPP_PP_PROBE_CONST_PROBE_, MAYBE_CONST)))                           \
    /**/

#define CPP_PP_PROBE_CONST_PROBE_const CPP_PP_PROBE(~)

#define CPP_FUN_IMPL_SELECT_CONST_1(...)                                        \
    CPP_PP_EVAL(                                                                \
        CPP_FUN_IMPL_SELECT_CONST_NOEXCEPT_,                                    \
        CPP_PP_CAT(CPP_FUN_IMPL_EAT_CONST_, __VA_ARGS__),)(                     \
        CPP_PP_CAT(CPP_FUN_IMPL_EAT_CONST_, __VA_ARGS__))                       \
    /**/

#define CPP_FUN_IMPL_SELECT_CONST_NOEXCEPT_(MAYBE_NOEXCEPT, ...)                \
    CPP_PP_CAT(CPP_FUN_IMPL_SELECT_CONST_NOEXCEPT_,                             \
        CPP_PP_CHECK(CPP_PP_CAT(                                                \
            CPP_PP_PROBE_NOEXCEPT_PROBE_, MAYBE_NOEXCEPT)))                     \
    /**/

#define CPP_PP_PROBE_NOEXCEPT_PROBE_noexcept CPP_PP_PROBE(~)

#define CPP_FUN_IMPL_SELECT_CONST_NOEXCEPT_0(...)                               \
    ::concepts::detail::enable_if_t<                                            \
        ::concepts::detail::Nil,                                                \
        static_cast<bool>(                                                      \
            CPP_PP_CAT(CPP_FUN_IMPL_EAT_REQUIRES_, __VA_ARGS__)) ||             \
        CPP_false(::concepts::detail::xNil{})> = {}) const                      \
    /**/

#define CPP_FUN_IMPL_SELECT_CONST_NOEXCEPT_1(...)                               \
    ::concepts::detail::enable_if_t<                                            \
        ::concepts::detail::Nil,                                                \
        static_cast<bool>(CPP_PP_CAT(CPP_FUN_IMPL_EAT_REQUIRES_, CPP_PP_CAT(    \
            CPP_FUN_IMPL_EAT_NOEXCEPT_, __VA_ARGS__))) ||                       \
            CPP_false(::concepts::detail::xNil{})> = {}) const CPP_PP_EXPAND(   \
            CPP_PP_CAT(CPP_FUN_IMPL_SHOW_NOEXCEPT_, __VA_ARGS__)))              \
    /**/

#define CPP_FUN_IMPL_EAT_NOEXCEPT_noexcept(...)
#define CPP_FUN_IMPL_SHOW_NOEXCEPT_noexcept(...)                                \
    noexcept(__VA_ARGS__) CPP_PP_EAT CPP_PP_LPAREN                              \
    /**/

#define CPP_FUN_IMPL_EAT_NOEXCEPT_noexcept(...)

#define CPP_FUN_IMPL_EXPAND_NOEXCEPT_noexcept(...)                              \
    noexcept(__VA_ARGS__)                                                       \
    /**/

#define CPP_FUN_IMPL_SELECT_CONST_0(...)                                        \
    CPP_FUN_IMPL_SELECT_NONCONST_NOEXCEPT_(__VA_ARGS__,)(__VA_ARGS__)           \
    /**/

#define CPP_FUN_IMPL_SELECT_NONCONST_NOEXCEPT_(MAYBE_NOEXCEPT, ...)             \
    CPP_PP_CAT(CPP_FUN_IMPL_SELECT_NONCONST_NOEXCEPT_,                          \
          CPP_PP_CHECK(CPP_PP_CAT(                                              \
            CPP_PP_PROBE_NOEXCEPT_PROBE_, MAYBE_NOEXCEPT)))                     \
    /**/

#define CPP_FUN_IMPL_SELECT_NONCONST_NOEXCEPT_0(...)                            \
    ::concepts::detail::enable_if_t<                                            \
        ::concepts::detail::Nil,                                                \
        static_cast<bool>(                                                      \
            CPP_PP_CAT(CPP_FUN_IMPL_EAT_REQUIRES_, __VA_ARGS__)) ||             \
        CPP_false(::concepts::detail::xNil{})> = {})                            \
    /**/

#define CPP_FUN_IMPL_SELECT_NONCONST_NOEXCEPT_1(...)                            \
    ::concepts::detail::enable_if_t<                                            \
        ::concepts::detail::Nil,                                                \
        static_cast<bool>(CPP_PP_CAT(CPP_FUN_IMPL_EAT_REQUIRES_, CPP_PP_CAT(    \
            CPP_FUN_IMPL_EAT_NOEXCEPT_, __VA_ARGS__))) ||                       \
            CPP_false(::concepts::detail::xNil{})> = {})                        \
        CPP_PP_EXPAND(CPP_PP_CAT(CPP_FUN_IMPL_SHOW_NOEXCEPT_, __VA_ARGS__)))    \
    /**/

#define CPP_FUN_IMPL_EAT_CONST_const
#define CPP_FUN_IMPL_EAT_REQUIRES_requires

////////////////////////////////////////////////////////////////////////////////
// CPP_fun
// Usage:
//   template <typename A, typename B>
//   void CPP_fun(foo)(A a, B b)([const]opt [noexcept(true)]opt
//       requires Concept1<A> && Concept2<B>)
//   {}
//
// Note: This macro cannot be used when the last function argument is a
//       parameter pack.
#define CPP_fun(X) X CPP_FUN_IMPL_1_
#endif

////////////////////////////////////////////////////////////////////////////////
// CPP_auto_fun
// Usage:
//   template <typename A, typename B>
//   auto CPP_auto_fun(foo)(A a, B b)([const]opt [noexcept(true)]opt)opt
//   (
//       return a + b
//   )
#define CPP_auto_fun(X) X CPP_AUTO_FUN_IMPL_
#define CPP_AUTO_FUN_IMPL_(...) (__VA_ARGS__) CPP_AUTO_FUN_RETURNS_
#define CPP_AUTO_FUN_RETURNS_(...)                                              \
    CPP_AUTO_FUN_SELECT_RETURNS_(__VA_ARGS__,)(__VA_ARGS__)                     \
    /**/
#define CPP_AUTO_FUN_SELECT_RETURNS_(MAYBE_CONST, ...)                          \
    CPP_PP_CAT(CPP_AUTO_FUN_RETURNS_CONST_,                                     \
        CPP_PP_CHECK(CPP_PP_CAT(                                                \
            CPP_PP_PROBE_CONST_MUTABLE_PROBE_, MAYBE_CONST)))                   \
    /**/
#define CPP_PP_PROBE_CONST_MUTABLE_PROBE_const CPP_PP_PROBE_N(~, 1)
#define CPP_PP_PROBE_CONST_MUTABLE_PROBE_mutable CPP_PP_PROBE_N(~, 2)
#define CPP_PP_EAT_MUTABLE_mutable
#define CPP_AUTO_FUN_RETURNS_CONST_2(...)                                       \
    CPP_PP_CAT(CPP_PP_EAT_MUTABLE_, __VA_ARGS__) CPP_AUTO_FUN_RETURNS_CONST_0
#define CPP_AUTO_FUN_RETURNS_CONST_1(...)                                       \
    __VA_ARGS__ CPP_AUTO_FUN_RETURNS_CONST_0                                    \
    /**/
#define CPP_AUTO_FUN_RETURNS_CONST_0(...)                                       \
    CPP_PP_EVAL(CPP_AUTO_FUN_DECLTYPE_NOEXCEPT_,                                \
        CPP_PP_CAT(CPP_AUTO_FUN_RETURNS_, __VA_ARGS__))                         \
    /**/
#define CPP_AUTO_FUN_RETURNS_return

#ifdef __cpp_guaranteed_copy_elision
#define CPP_AUTO_FUN_DECLTYPE_NOEXCEPT_(...)                                    \
    noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__)                    \
    { return (__VA_ARGS__); }                                                   \
    /**/
#else
#define CPP_AUTO_FUN_DECLTYPE_NOEXCEPT_(...)                                    \
    noexcept(noexcept(decltype(__VA_ARGS__)(__VA_ARGS__))) ->                   \
    decltype(__VA_ARGS__)                                                       \
    { return (__VA_ARGS__); }                                                   \
    /**/
#endif

namespace concepts
{
    template<bool B>
    using bool_ = std::integral_constant<bool, B>;

#if defined(__cpp_fold_expressions) && __cpp_fold_expressions >= 201603
    template<bool...Bs>
    CPP_INLINE_VAR constexpr bool and_v = (Bs &&...);

    template<bool...Bs>
    CPP_INLINE_VAR constexpr bool or_v = (Bs ||...);
#else
    namespace detail
    {
        template<bool...>
        struct bools;
    } // namespace detail

    template<bool...Bs>
    CPP_INLINE_VAR constexpr bool and_v =
        META_IS_SAME(detail::bools<Bs..., true>, detail::bools<true, Bs...>);

    template<bool...Bs>
    CPP_INLINE_VAR constexpr bool or_v =
        !META_IS_SAME(detail::bools<Bs..., false>, detail::bools<false, Bs...>);
#endif

    namespace detail
    {
        template<typename>
        constexpr bool requires_()
        {
            return true;
        }
        template<typename T, typename U>
        struct and_;
        template<typename T>
        struct not_
        {
            CPP_EXPLICIT constexpr operator bool() const noexcept
            {
                return !(bool) T{};
            }
            constexpr auto operator!() const noexcept
            {
                return T{};
            }
            template<typename That>
            constexpr auto operator&&(That) const noexcept
            {
                return and_<not_, That>{};
            }
        };
        template<typename T, typename U>
        struct and_
        {
            static constexpr bool impl(std::false_type) noexcept
            {
                return false;
            }
            static constexpr bool impl(std::true_type) noexcept
            {
                return (bool) U{};
            }
            CPP_EXPLICIT constexpr operator bool() const noexcept
            {
                return and_::impl(bool_<(bool) T{}>{});
            }
            constexpr auto operator!() const noexcept
            {
                return not_<and_>{};
            }
            template<typename That>
            constexpr auto operator&&(That) const noexcept
            {
                return detail::and_<and_, That>{};
            }
        };

        template<typename...>
        struct identity
        {
            template<typename T>
            using invoke = T;
        };

        template<typename T, bool Enable>
        using enable_if_t = meta::invoke<identity<std::enable_if_t<Enable>>, T>;

        struct Nil
        {};

#ifdef CPP_WORKAROUND_MSVC_779763
        enum class xNil {};

        struct CPP_false_t
        {
            constexpr bool operator()(Nil) const noexcept
            {
                return false;
            }
            constexpr bool operator()(xNil) const noexcept
            {
                return false;
            }
        };

        CPP_INLINE_VAR constexpr CPP_false_t CPP_false_{};

        constexpr bool CPP_false(xNil)
        {
            return false;
        }
#else
        using xNil = Nil;
#endif

        constexpr bool CPP_false(Nil)
        {
            return false;
        }

        template<typename T>
        using remove_cvref_t =
            typename std::remove_cv<typename std::remove_reference<T>::type>::type;

        CPP_def
        (
            template(typename T, typename U)
            concept weakly_equality_comparable_with_,
                requires (detail::as_cref_t<T> t, detail::as_cref_t<U> u)
                (
                    (t == u) ? 1 : 0,
                    (t != u) ? 1 : 0,
                    (u == t) ? 1 : 0,
                    (u != t) ? 1 : 0
                )
        );
    } // namespace detail

#if defined(__clang__) || defined(_MSC_VER)
    template<bool B>
    ::concepts::detail::enable_if_t<void, B> requires_()
    {}
#else
    template<bool B>
    CPP_INLINE_VAR constexpr ::concepts::detail::enable_if_t<int, B> requires_ = 0;
#endif

    inline namespace defs
    {
        ////////////////////////////////////////////////////////////////////////////////////////
        // Utility concepts
        ////////////////////////////////////////////////////////////////////////////////////////

        CPP_def
        (
            template(bool B)
            (concept is_true)(B),
                B
        );

        CPP_def
        (
            template(typename... Args)
            (concept type)(Args...),
                true
        );

        CPP_def
        (
            template(class T, template<typename...> class Trait, typename... Args)
            (concept satisfies)(T, Trait, Args...),
                static_cast<bool>(Trait<T, Args...>::type::value)
        );

        ////////////////////////////////////////////////////////////////////////////////////////
        // Core language concepts
        ////////////////////////////////////////////////////////////////////////////////////////

        CPP_def
        (
            template(typename A, typename B)
            concept same_as,
                META_IS_SAME(A, B) && META_IS_SAME(B, A)
        );

        /// \cond
        CPP_def
        (
            template(typename A, typename B)
            concept not_same_as_,
                (!same_as<detail::remove_cvref_t<A>, detail::remove_cvref_t<B>>)
        );

        // Workaround bug in the Standard Library:
        // From cannot be an incomplete class type despite that
        // is_convertible<X, Y> should be equivalent to is_convertible<X&&, Y>
        // in such a case.
        CPP_def
        (
            template(typename From, typename To)
            concept implicitly_convertible_to,
                std::is_convertible<typename std::add_rvalue_reference<From>::type, To>::value
        );

        CPP_def
        (
            template(typename From, typename To)
            concept explicitly_convertible_to,
                requires (From (&from)())
                (
                    static_cast<To>(from())
                )
        );
        /// \endcond

        CPP_def
        (
            template(typename From, typename To)
            concept convertible_to,
                implicitly_convertible_to<From, To> &&
                explicitly_convertible_to<From, To>
        );

        CPP_def
        (
            template(typename T, typename U)
            concept derived_from,
                META_IS_BASE_OF(U, T) &&
                convertible_to<T const volatile *, U const volatile *>
        );

        CPP_def
        (
            template(typename T, typename U)
            concept common_reference_with,
                same_as<common_reference_t<T, U>, common_reference_t<U, T>> &&
                convertible_to<T, common_reference_t<T, U>> &&
                convertible_to<U, common_reference_t<T, U>>
        );

        CPP_def
        (
            template(typename T, typename U)
            concept common_with,
                same_as<common_type_t<T, U>, common_type_t<U, T>> &&
                convertible_to<T, common_type_t<T, U>> &&
                convertible_to<U, common_type_t<T, U>> &&
                common_reference_with<
                    typename std::add_lvalue_reference<T const>::type,
                    typename std::add_lvalue_reference<U const>::type> &&
                common_reference_with<
                    typename std::add_lvalue_reference<common_type_t<T, U>>::type,
                    common_reference_t<
                        typename std::add_lvalue_reference<T const>::type,
                        typename std::add_lvalue_reference<U const>::type>>
        );

        CPP_def
        (
            template(typename T)
            concept integral,
                std::is_integral<T>::value
        );

        CPP_def
        (
            template(typename T)
            concept signed_integral,
                integral<T> &&
                std::is_signed<T>::value
        );

        CPP_def
        (
            template(typename T)
            concept unsigned_integral,
                integral<T> &&
                !signed_integral<T>
        );

        CPP_def
        (
            template(typename T, typename U)
            concept assignable_from,
                requires (T t, U &&u)
                (
                    t = (U &&) u,
                    requires_<same_as<T, decltype(t = (U &&) u)>>
                ) &&
                std::is_lvalue_reference<T>::value
        );

        CPP_def
        (
            template(typename T)
            concept swappable,
                requires (T &t, T &u)
                (
                    concepts::swap(t, u)
                )
        );

        CPP_def
        (
            template(typename T, typename U)
            concept swappable_with,
                requires (T &&t, U &&u)
                (
                    concepts::swap((T &&) t, (T &&) t),
                    concepts::swap((U &&) u, (U &&) u),
                    concepts::swap((U &&) u, (T &&) t),
                    concepts::swap((T &&) t, (U &&) u)
                ) &&
                common_reference_with<detail::as_cref_t<T>, detail::as_cref_t<U>>
        );

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Comparison concepts
        ////////////////////////////////////////////////////////////////////////////////////////////

        CPP_def
        (
            template(typename T)
            concept equality_comparable,
                detail::weakly_equality_comparable_with_<T, T>
        );

        CPP_def
        (
            template(typename T, typename U)
            concept equality_comparable_with,
                equality_comparable<T> &&
                equality_comparable<U> &&
                detail::weakly_equality_comparable_with_<T, U> &&
                common_reference_with<detail::as_cref_t<T>, detail::as_cref_t<U>> &&
                equality_comparable<
                    common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
        );

        CPP_def
        (
            template(typename T)
            concept totally_ordered,
                requires (detail::as_cref_t<T> t, detail::as_cref_t<T> u)
                (
                    t < u ? 1 : 0,
                    t > u ? 1 : 0,
                    u <= t ? 1 : 0,
                    u >= t ? 1 : 0
                ) &&
                equality_comparable<T>
        );

        CPP_def
        (
            template(typename T, typename U)
            concept totally_ordered_with,
                requires (detail::as_cref_t<T> t, detail::as_cref_t<U> u)
                (
                    t < u ? 1 : 0,
                    t > u ? 1 : 0,
                    t <= u ? 1 : 0,
                    t >= u ? 1 : 0,
                    u < t ? 1 : 0,
                    u > t ? 1 : 0,
                    u <= t ? 1 : 0,
                    u >= t ? 1 : 0
                ) &&
                totally_ordered<T> &&
                totally_ordered<U> &&
                equality_comparable_with<T, U> &&
                common_reference_with<detail::as_cref_t<T>, detail::as_cref_t<U>> &&
                totally_ordered<
                    common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
        );

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Object concepts
        ////////////////////////////////////////////////////////////////////////////////////////////

        CPP_def
        (
            template(typename T)
            concept destructible,
                std::is_nothrow_destructible<T>::value
        );

        CPP_def
        (
            template(typename T, typename... Args)
            (concept constructible_from)(T, Args...),
                destructible<T> &&
                META_IS_CONSTRUCTIBLE(T, Args...)
        );

        CPP_def
        (
            template(typename T)
            concept default_constructible,
                constructible_from<T>
        );

        CPP_def
        (
            template(typename T)
            concept move_constructible,
                constructible_from<T, T> &&
                convertible_to<T, T>
        );

        CPP_def
        (
            template(typename T)
            concept copy_constructible,
                move_constructible<T> &&
                constructible_from<T, T &> &&
                constructible_from<T, T const &> &&
                constructible_from<T, T const> &&
                convertible_to<T &, T> &&
                convertible_to<T const &, T> &&
                convertible_to<T const, T>
        );

        CPP_def
        (
            template(typename T)
            concept movable,
                std::is_object<T>::value &&
                move_constructible<T> &&
                assignable_from<T &, T> &&
                swappable<T>
        );

        CPP_def
        (
            template(typename T)
            concept copyable,
                copy_constructible<T> &&
                movable<T> &&
                assignable_from<T &, T const &>
        );

        CPP_def
        (
            template(typename T)
            concept semiregular,
                copyable<T> &&
                default_constructible<T>
            // Axiom: copies are independent. See Fundamentals of Generic Programming
            // http://www.stepanovpapers.com/DeSt98.pdf
        );

        CPP_def
        (
            template(typename T)
            concept regular,
                semiregular<T> &&
                equality_comparable<T>
        );
    } // inline namespace defs

    template<typename Concept, typename... Args>
    struct is_satisfied_by
      : meta::bool_<static_cast<bool>(typename Concept::template Eval<Args...>{})>
    {};

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
} // namespace concepts

CPP_PP_IGNORE_CXX2A_COMPAT_END

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
