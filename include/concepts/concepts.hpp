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

// Disable buggy clang compatibility warning about "requires" and "concept" being
// C++20 keywords.
// https://bugs.llvm.org/show_bug.cgi?id=43708
#if defined(__clang__) && __cplusplus <= 201703L
#define CPP_PP_IGNORE_CXX2A_COMPAT_BEGIN                                                \
    CPP_DIAGNOSTIC_PUSH                                                                 \
    CPP_DIAGNOSTIC_IGNORE_CPP2A_COMPAT                                                  \
    /**/
#define CPP_PP_IGNORE_CXX2A_COMPAT_END                                                  \
    CPP_DIAGNOSTIC_POP                                                                  \
    /**/
#else
#define CPP_PP_IGNORE_CXX2A_COMPAT_BEGIN
#define CPP_PP_IGNORE_CXX2A_COMPAT_END
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define CPP_WORKAROUND_MSVC_779763 // FATAL_UNREACHABLE calling constexpr function via template parameter
#define CPP_WORKAROUND_MSVC_780775 // Incorrect substitution in function template return type
#define CPP_WORKAROUND_MSVC_654601 // Failure to invoke *implicit* bool conversion in a constant expression
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

#define CPP_PP_FOR_EACH(M, ...) \
    CPP_PP_FOR_EACH_N(CPP_PP_COUNT(__VA_ARGS__), M, __VA_ARGS__)
#define CPP_PP_FOR_EACH_N(N, M, ...) \
    CPP_PP_CAT(CPP_PP_FOR_EACH_, N)(M, __VA_ARGS__)
#define CPP_PP_FOR_EACH_1(M, _1) \
    M(_1)
#define CPP_PP_FOR_EACH_2(M, _1, _2) \
    M(_1), M(_2)
#define CPP_PP_FOR_EACH_3(M, _1, _2, _3) \
    M(_1), M(_2), M(_3)
#define CPP_PP_FOR_EACH_4(M, _1, _2, _3, _4) \
    M(_1), M(_2), M(_3), M(_4)
#define CPP_PP_FOR_EACH_5(M, _1, _2, _3, _4, _5) \
    M(_1), M(_2), M(_3), M(_4), M(_5)
#define CPP_PP_FOR_EACH_6(M, _1, _2, _3, _4, _5, _6) \
    M(_1), M(_2), M(_3), M(_4), M(_5), M(_6)
#define CPP_PP_FOR_EACH_7(M, _1, _2, _3, _4, _5, _6, _7) \
    M(_1), M(_2), M(_3), M(_4), M(_5), M(_6), M(_7)
#define CPP_PP_FOR_EACH_8(M, _1, _2, _3, _4, _5, _6, _7, _8) \
    M(_1), M(_2), M(_3), M(_4), M(_5), M(_6), M(_7), M(_8)

#define CPP_PP_PROBE_EMPTY_PROBE_CPP_PP_PROBE_EMPTY                             \
    CPP_PP_PROBE(~)                                                             \

#define CPP_PP_PROBE_EMPTY()
#define CPP_PP_IS_NOT_EMPTY(...)                                                \
    CPP_PP_CHECK(CPP_PP_CAT(CPP_PP_PROBE_EMPTY_PROBE_,                          \
        CPP_PP_PROBE_EMPTY __VA_ARGS__ ()))                                     \
    /**/

#define CPP_assert(...)                                                         \
    static_assert(static_cast<bool>(__VA_ARGS__),                               \
        "Concept assertion failed : " #__VA_ARGS__)                             \
    /**/
#define CPP_assert_msg static_assert

#ifdef CPP_WORKAROUND_MSVC_654601
#define CPP_FORCE_TO_BOOL static_cast<bool>
#else
#define CPP_FORCE_TO_BOOL
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define CPP_INSTANCE(...) ::concepts::detail::instance_<decltype(__VA_ARGS__)>
#else
#define CPP_INSTANCE(...) __VA_ARGS__
#endif

#define CPP_fwd(ARG) \
    ((decltype(ARG)&&) ARG)

#define CPP_type_of(...) \
    CPP_PP_EXPAND(CPP_type_of_2 __VA_ARGS__))
#define CPP_type_of_2(...) \
    __VA_ARGS__ CPP_PP_EAT CPP_PP_LPAREN
#define CPP_name_of(...) \
    CPP_PP_EAT __VA_ARGS__
#define CPP_param(...) \
    auto&& CPP_name_of(__VA_ARGS__)

#define CPP_requires(...) \
    CPP_requires_n(CPP_PP_COUNT(__VA_ARGS__), __VA_ARGS__)

#define CPP_requires_(...) \
    CPP_requires_n_(CPP_PP_COUNT(__VA_ARGS__), __VA_ARGS__)

#if CPP_CXX_CONCEPTS || defined(CPP_DOXYGEN_INVOKED)
#define CPP_concept META_CONCEPT
#define CPP_concept_bool META_CONCEPT
#ifdef CPP_DOXYGEN_INVOKED
#define CPP_arg_2(...) __VA_ARGS__
#define CPP_valid_expressions
#else
#define CPP_arg_2(...) ::concepts::detail::id_t<__VA_ARGS__>
#define CPP_valid_expressions(...) \
    {__VA_ARGS__;}
#endif
#define CPP_arg(ARG) CPP_arg_2 ARG
#define CPP_requires_n(N, ...) \
    requires(CPP_PP_FOR_EACH_N(N, CPP_arg, __VA_ARGS__)) \
        CPP_valid_expressions
#define CPP_requires_n_ CPP_requires_n
#define CPP_defer_(CONCEPT, ...) \
    CONCEPT<__VA_ARGS__>
#define CPP_defer(CONCEPT, ...) \
    CONCEPT<__VA_ARGS__>
#define CPP_type(...) \
    __VA_ARGS__
#define CPP_literal(...) \
    __VA_ARGS__
#define CPP_concept_fragment(NAME, ARGS, ...) \
    META_CONCEPT NAME = __VA_ARGS__
#define CPP_fragment(NAME, ...) \
    NAME<__VA_ARGS__>
#else
// Use CPP_concept_bool instead of CPP_concept on gcc-8 and earlier to avoid:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=87512
#define CPP_concept_bool CPP_INLINE_VAR constexpr bool
#define CPP_concept CPP_INLINE_VAR constexpr auto
#define CPP_requires_n_(N, ...) \
    (true ? nullptr : ::concepts::detail::test_concept_( \
        [](auto const CPP_arg) -> \
            ::concepts::detail::invoke_result_t<\
                decltype(CPP_arg), int, \
                CPP_PP_FOR_EACH_N(N, CPP_type_of, __VA_ARGS__)> {\
                    return {}; \
                }, \
        [](auto CPP_arg, CPP_PP_FOR_EACH_N(N, CPP_param, __VA_ARGS__)) \
        CPP_valid_expressions
#define CPP_requires_n(N, ...) \
    (true ? nullptr : ::concepts::detail::test_concept( \
        (::concepts::detail::tag< \
            CPP_PP_FOR_EACH_N(N, CPP_type_of, __VA_ARGS__) > *) nullptr, \
        [](auto CPP_arg, CPP_PP_FOR_EACH_N(N, CPP_param, __VA_ARGS__)) \
        CPP_valid_expressions
#define CPP_valid_expressions(...) \
    -> decltype(__VA_ARGS__, void(), ::concepts::detail::true_type{}) \
    { (void)CPP_arg; return {}; })).value()
#define CPP_type(...) \
    ::concepts::detail::first_t<__VA_ARGS__, decltype(CPP_arg)>
#define CPP_literal(...) \
    (CPP_arg, void(), __VA_ARGS__)
#define CPP_type_(ARG) \
    CPP_type(CPP_PP_IIF(CPP_PP_NOT(CPP_PP_IS_PAREN(ARG)))(, CPP_PP_EXPAND) ARG)
#define CPP_defer_(CONCEPT, ...)\
    true? nullptr \
        : ::concepts::detail::make_boolean(\
            [](auto CPP_arg) { \
                (void) CPP_arg; \
                return std::integral_constant<bool, (CONCEPT<__VA_ARGS__>)>{};\
            })\
    /**/
#define CPP_defer(CONCEPT, ...)\
    CPP_defer_(CONCEPT, CPP_PP_FOR_EACH(CPP_type_, __VA_ARGS__))
#define CPP_concept_fragment(NAME, ARGS, ...) \
    std::enable_if_t<__VA_ARGS__, ::concepts::detail::true_type> \
    CPP_PP_CAT(NAME, CPP_concept_fragment_)( \
        ::concepts::detail::tag<CPP_PP_EXPAND ARGS> *); \
    ::concepts::detail::false_type \
    CPP_PP_CAT(NAME, CPP_concept_fragment_)(void *) \
    /**/
#define CPP_fragment(NAME, ...) \
    decltype(CPP_PP_CAT(NAME, CPP_concept_fragment_)(\
        (::concepts::detail::tag<__VA_ARGS__> *) nullptr))::value() \
    /**/
#endif

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
    typename CPP_true_ = std::true_type,                                        \
    std::enable_if_t<                                                           \
        CPP_FORCE_TO_BOOL(                                                      \
            CPP_PP_CAT(CPP_TEMPLATE_SFINAE_AUX_3_, __VA_ARGS__) && CPP_true_{}  \
        ),                                                                      \
        int                                                                     \
    > = 0>                                                                      \
    /**/
#define CPP_template_def_sfinae(...)                                            \
    template<__VA_ARGS__ CPP_TEMPLATE_DEF_SFINAE_AUX_                           \
    /**/
#define CPP_TEMPLATE_DEF_SFINAE_AUX_(...) ,                                     \
    typename CPP_true_,                                                         \
    std::enable_if_t<                                                           \
        CPP_FORCE_TO_BOOL(                                                      \
            CPP_PP_CAT(CPP_TEMPLATE_SFINAE_AUX_3_, __VA_ARGS__) && CPP_true_{}  \
        ),                                                                      \
        int                                                                     \
    >>                                                                          \
    /**/
#define CPP_TEMPLATE_SFINAE_AUX_3_requires
#define CPP_member_sfinae                                                       \
    CPP_broken_friend_member                                                    \
    /**/
#define CPP_ctor_sfinae(TYPE)                                                   \
    CPP_PP_IGNORE_CXX2A_COMPAT_BEGIN                                            \
    TYPE CPP_CTOR_SFINAE_IMPL_1_                                                \
    /**/
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
    std::enable_if_t<                                                           \
        CPP_FORCE_TO_BOOL(                                                      \
            CPP_PP_CAT(CPP_TEMPLATE_SFINAE_AUX_3_, __VA_ARGS__) &&              \
            CPP_INSTANCE(CPP_true(::concepts::detail::xNil{}))                  \
        ),                                                                      \
        ::concepts::detail::Nil                                                 \
    > = {})                                                                     \
    CPP_PP_IGNORE_CXX2A_COMPAT_END                                              \
    /**/
// Yes noexcept-clause:
#define CPP_CTOR_SFINAE_REQUIRES_1(...)                                         \
    std::enable_if_t<                                                           \
        CPP_FORCE_TO_BOOL(                                                      \
            CPP_PP_CAT(CPP_TEMPLATE_SFINAE_AUX_3_,                              \
                CPP_PP_CAT(CPP_CTOR_SFINAE_EAT_NOEXCEPT_, __VA_ARGS__)          \
            ) && CPP_INSTANCE(CPP_true(::concepts::detail::xNil{}))             \
        ),                                                                      \
        ::concepts::detail::Nil                                                 \
    > = {})                                                                     \
    CPP_PP_EXPAND(CPP_PP_CAT(CPP_CTOR_SFINAE_SHOW_NOEXCEPT_, __VA_ARGS__)))     \
    /**/
#define CPP_CTOR_SFINAE_EAT_NOEXCEPT_noexcept(...)
#define CPP_CTOR_SFINAE_SHOW_NOEXCEPT_noexcept(...)                             \
    noexcept(__VA_ARGS__)                                                       \
    CPP_PP_IGNORE_CXX2A_COMPAT_END                                              \
    CPP_PP_EAT CPP_PP_LPAREN                                                    \
    /**/

#ifdef CPP_DOXYGEN_INVOKED
#define CPP_broken_friend_ret(...)                                              \
    __VA_ARGS__ CPP_PP_EXPAND                                                   \
    /**/
#else
#define CPP_broken_friend_ret(...)                                              \
    ::concepts::return_t<                                                       \
        __VA_ARGS__,                                                            \
        std::enable_if_t<CPP_FORCE_TO_BOOL(CPP_BROKEN_FRIEND_RETURN_TYPE_AUX_   \
    /**/
#define CPP_BROKEN_FRIEND_RETURN_TYPE_AUX_(...)                                 \
    CPP_BROKEN_FRIEND_RETURN_TYPE_AUX_3_(CPP_PP_CAT(                            \
        CPP_TEMPLATE_AUX_2_, __VA_ARGS__))                                      \
    /**/
#define CPP_TEMPLATE_AUX_2_requires
#define CPP_BROKEN_FRIEND_RETURN_TYPE_AUX_3_(...)                               \
    __VA_ARGS__ && CPP_INSTANCE(CPP_true(::concepts::detail::xNil{})))>>        \
    /**/
#ifdef CPP_WORKAROUND_MSVC_779763
#define CPP_broken_friend_member                                                \
    template<::concepts::detail::CPP_true_t const &CPP_true =                   \
        ::concepts::detail::CPP_true_>                                          \
    /**/
#else // ^^^ workaround / no workaround vvv
#define CPP_broken_friend_member                                                \
    template<std::true_type (&CPP_true)(::concepts::detail::xNil) =             \
        ::concepts::detail::CPP_true>                                           \
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
    std::enable_if_t<                                                           \
        CPP_FORCE_TO_BOOL(                                                      \
            CPP_PP_CAT(CPP_FUN_IMPL_EAT_REQUIRES_, __VA_ARGS__) &&              \
            CPP_INSTANCE(CPP_true(::concepts::detail::xNil{}))                  \
        ),                                                                      \
        ::concepts::detail::Nil                                                 \
    > = {}) const                                                               \
    CPP_PP_IGNORE_CXX2A_COMPAT_END                                              \
    /**/

#define CPP_FUN_IMPL_SELECT_CONST_NOEXCEPT_1(...)                               \
    std::enable_if_t<                                                           \
        CPP_FORCE_TO_BOOL(                                                      \
            CPP_PP_CAT(                                                         \
                CPP_FUN_IMPL_EAT_REQUIRES_,                                     \
                CPP_PP_CAT(CPP_FUN_IMPL_EAT_NOEXCEPT_, __VA_ARGS__)             \
            ) && CPP_INSTANCE(CPP_true(::concepts::detail::xNil{}))             \
        ),                                                                      \
        ::concepts::detail::Nil                                                 \
    > = {}) const                                                               \
    CPP_PP_EXPAND(CPP_PP_CAT(CPP_FUN_IMPL_SHOW_NOEXCEPT_, __VA_ARGS__)))        \
    /**/

#define CPP_FUN_IMPL_EAT_NOEXCEPT_noexcept(...)
#define CPP_FUN_IMPL_SHOW_NOEXCEPT_noexcept(...)                                \
    noexcept(__VA_ARGS__) CPP_PP_IGNORE_CXX2A_COMPAT_END                        \
    CPP_PP_EAT CPP_PP_LPAREN                                                    \
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
    std::enable_if_t<                                                           \
        CPP_FORCE_TO_BOOL(                                                      \
            CPP_PP_CAT(CPP_FUN_IMPL_EAT_REQUIRES_, __VA_ARGS__) &&              \
            CPP_INSTANCE(CPP_true(::concepts::detail::xNil{}))                  \
        ),                                                                      \
        ::concepts::detail::Nil                                                 \
    > = {})                                                                     \
    CPP_PP_IGNORE_CXX2A_COMPAT_END                                              \
    /**/

#define CPP_FUN_IMPL_SELECT_NONCONST_NOEXCEPT_1(...)                            \
    std::enable_if_t<                                                           \
        CPP_FORCE_TO_BOOL(                                                      \
            CPP_PP_CAT(                                                         \
                CPP_FUN_IMPL_EAT_REQUIRES_,                                     \
                CPP_PP_CAT(CPP_FUN_IMPL_EAT_NOEXCEPT_, __VA_ARGS__)             \
            ) &&                                                                \
            CPP_INSTANCE(CPP_true(::concepts::detail::xNil{}))                  \
        ),                                                                      \
        ::concepts::detail::Nil                                                 \
    > = {})                                                                     \
    CPP_PP_EXPAND(CPP_PP_CAT(CPP_FUN_IMPL_SHOW_NOEXCEPT_, __VA_ARGS__)))        \
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
#define CPP_fun(X) CPP_PP_IGNORE_CXX2A_COMPAT_BEGIN X CPP_FUN_IMPL_1_
#endif

////////////////////////////////////////////////////////////////////////////////
// CPP_auto_fun
// Usage:
//   template <typename A, typename B>
//   auto CPP_auto_fun(foo)(A a, B b)([const]opt [noexcept(cond)]opt)opt
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

    template<typename>
    struct return_t_
    {
        template<typename T>
        using invoke = T;
    };

    template<typename T, typename EnableIf>
    using return_t = meta::invoke<return_t_<EnableIf>, T>;

    namespace detail
    {
        struct ignore
        {
            template<class... Args>
            constexpr ignore(Args&&...) noexcept {}
        };

        struct false_type
        {
            false_type() = default;
            constexpr false_type(decltype(nullptr)) noexcept {}
            static constexpr bool value() noexcept { return false; }
        };

        struct true_type
        {
            true_type() = default;
            constexpr true_type(decltype(nullptr)) noexcept {}
            static constexpr bool value() noexcept { return true; }
        };

        template<typename...>
        struct tag;

        template<unsigned U>
        struct first_impl
        {
            template<class T>
            using invoke = T;
        };

        template<class T, class U>
        using first_t = meta::invoke<first_impl<sizeof(U) ^ sizeof(U)>, T>;

        template<typename Fun, typename... Args>
        using invoke_result_t =
            decltype(((Fun &&(*)()) nullptr)()(((Args &&(*)()) nullptr)()...));

        template<typename ...Args, typename ExprsFn>
        auto test_concept(tag<Args...> *, ExprsFn) ->
            first_t<true_type, invoke_result_t<ExprsFn, int, Args...>>
        {
            return {};
        }
        inline false_type test_concept(void *, ignore)
        {
            return {};
        }
        template<typename ArgsFn, typename ExprsFn>
        auto test_concept_(ArgsFn const args, ExprsFn exprs) ->
            decltype(args((ExprsFn &&) exprs))
        {
            return {};
        }
        inline false_type test_concept_(ignore, ignore)
        {
            return {};
        }

        template<class T>
        using id_t = T;

        template<typename T>
        CPP_INLINE_VAR constexpr T instance_ = T{};

        template<typename>
        constexpr bool requires_()
        {
            return true;
        }

        struct boolean
        {
            friend bool operator&&(boolean, bool) = delete;
            friend bool operator&&(bool, boolean) = delete;
            friend bool operator||(boolean, bool) = delete;
            friend bool operator||(bool, boolean) = delete;
        };
        template<typename T, typename U>
        struct and_;
        template<typename T, typename U>
        struct or_;
        template<typename T>
        struct not_ : boolean
        {
            constexpr operator bool() const noexcept
            {
                return !(bool) T{};
            }
            constexpr T operator!() const noexcept
            {
                return T{};
            }
            template<typename That>
            constexpr and_<not_, That> operator&&(That) const noexcept
            {
                return {};
            }
            template<typename That>
            constexpr or_<not_, That> operator||(That) const noexcept
            {
                return {};
            }
        };

        template<typename T, typename U>
        struct and_ : boolean
        {
            static constexpr bool impl(std::false_type) noexcept
            {
                return false;
            }
            static constexpr bool impl(std::true_type) noexcept
            {
                return (bool) U{};
            }
            constexpr operator bool() const noexcept
            {
                return and_::impl(bool_<(bool) T{}>{});
            }
            constexpr not_<and_> operator!() const noexcept
            {
                return not_<and_>{};
            }
            template<typename That>
            constexpr and_<and_, That> operator&&(That) const noexcept
            {
                static_assert(
                    !META_IS_SAME(That, bool),
                    "All expressions in a conjunction should be "
                    "defer:: concepts");
                return {};
            }
            template<typename That>
            constexpr or_<and_, That> operator||(That) const noexcept
            {
                static_assert(
                    !META_IS_SAME(That, bool),
                    "All expressions in a disjunction should be "
                    "defer:: concepts");
                return {};
            }
        };

        template<typename T, typename U>
        struct or_ : boolean
        {
            static constexpr bool impl(std::true_type) noexcept
            {
                return true;
            }
            static constexpr bool impl(std::false_type) noexcept
            {
                return (bool) U{};
            }
            constexpr operator bool() const noexcept
            {
                return or_::impl(bool_<(bool) T{}>{});
            }
            constexpr not_<or_> operator!() const noexcept
            {
                return {};
            }
            template<typename That>
            constexpr and_<or_, That> operator&&(That) const noexcept
            {
                static_assert(
                    !META_IS_SAME(That, bool),
                    "All expressions in a disjunction should be "
                    "defer:: concepts");
                return {};
            }
            template<typename That>
            constexpr or_<or_, That> operator||(That) const noexcept
            {
                static_assert(
                    !META_IS_SAME(That, bool),
                    "All expressions in a disjunction should be "
                    "defer:: concepts");
                return {};
            }
        };

        template<class Fn>
        struct boolean_ : boolean
        {
            boolean_() = default;
            constexpr boolean_(decltype(nullptr)) noexcept {}
            template<bool = true>
            constexpr operator bool() const noexcept
            {
                return decltype(std::declval<Fn>()(std::true_type{}))::value;
            }
            constexpr not_<boolean_> operator!() const noexcept
            {
                return {};
            }
            template<typename That>
            constexpr and_<boolean_, That> operator&&(That) const noexcept
            {
                static_assert(
                    !META_IS_SAME(That, bool),
                    "All expressions in a disjunction should be "
                    "defer:: concepts");
                return {};
            }
            template<typename That>
            constexpr or_<boolean_, That> operator||(That) const noexcept
            {
                static_assert(
                    !META_IS_SAME(That, bool),
                    "All expressions in a disjunction should be "
                    "defer:: concepts");
                return {};
            }
        };
        template<class Fn>
        constexpr boolean_<Fn> make_boolean(Fn) noexcept { return nullptr; }

        struct Nil
        {};

#ifdef CPP_WORKAROUND_MSVC_779763
        enum class xNil {};

        struct CPP_true_t
        {
            constexpr std::true_type operator()(Nil) const noexcept
            {
                return {};
            }
            constexpr std::true_type operator()(xNil) const noexcept
            {
                return {};
            }
        };

        CPP_INLINE_VAR constexpr CPP_true_t CPP_true_{};

        constexpr std::true_type CPP_true(xNil)
        {
            return {};
        }
#else
        using xNil = Nil;
#endif

        constexpr std::true_type CPP_true(Nil)
        {
            return {};
        }

        template<typename T>
        using remove_cvref_t =
            typename std::remove_cv<typename std::remove_reference<T>::type>::type;

        CPP_DIAGNOSTIC_PUSH
        CPP_DIAGNOSTIC_IGNORE_FLOAT_EQUAL

        template<typename T, typename U>
        CPP_concept_bool weakly_equality_comparable_with_ =
            CPP_requires ((detail::as_cref_t<T>) t, (detail::as_cref_t<U>) u) //
            (
                (t == u) ? 1 : 0,
                (t != u) ? 1 : 0,
                (u == t) ? 1 : 0,
                (u != t) ? 1 : 0
            );

        CPP_DIAGNOSTIC_POP
    } // namespace detail

#if defined(__clang__) || defined(_MSC_VER)
    template<bool B>
    std::enable_if_t<B> requires_()
    {}
#else
    template<bool B>
    CPP_INLINE_VAR constexpr std::enable_if_t<B, int> requires_ = 0;
#endif

    inline namespace defs
    {
        ////////////////////////////////////////////////////////////////////////////////////////
        // Utility concepts
        ////////////////////////////////////////////////////////////////////////////////////////

        template<bool B>
        CPP_concept_bool is_true = B;

        template<typename... Args>
        CPP_concept_bool type = true;

        template<class T, template<typename...> class Trait, typename... Args>
        CPP_concept_bool satisfies =
            static_cast<bool>(Trait<T, Args...>::type::value);

        ////////////////////////////////////////////////////////////////////////////////////////
        // Core language concepts
        ////////////////////////////////////////////////////////////////////////////////////////

        template<typename A, typename B>
        CPP_concept_bool same_as =
            META_IS_SAME(A, B) && META_IS_SAME(B, A);

        /// \cond
        template<typename A, typename B>
        CPP_concept_bool not_same_as_ =
            (!same_as<detail::remove_cvref_t<A>, detail::remove_cvref_t<B>>);

        // Workaround bug in the Standard Library:
        // From cannot be an incomplete class type despite that
        // is_convertible<X, Y> should be equivalent to is_convertible<X&&, Y>
        // in such a case.
        template<typename From, typename To>
        CPP_concept_bool implicitly_convertible_to =
            std::is_convertible<std::add_rvalue_reference_t<From>, To>::value;

        template<typename From, typename To>
        CPP_concept_bool explicitly_convertible_to =
            CPP_requires_ ((CPP_type(From)(*)()) from) //
            (
                static_cast<To>(from())
            );
        /// \endcond

        template<typename From, typename To>
        CPP_concept_bool convertible_to =
            implicitly_convertible_to<From, To> &&
            explicitly_convertible_to<From, To>;

        template<typename T, typename U>
        CPP_concept_fragment(derived_from_, (T, U),
            convertible_to<T const volatile *, U const volatile *>
        );
        template<typename T, typename U>
        CPP_concept_bool derived_from =
            META_IS_BASE_OF(U, T) &&
            CPP_fragment(concepts::derived_from_, T, U);

        template<typename T, typename U>
        CPP_concept_fragment(common_reference_with_, (T, U),
            same_as<common_reference_t<T, U>,
                    common_reference_t<U, T>> &&
            convertible_to<T, common_reference_t<T, U>> &&
            convertible_to<U, common_reference_t<T, U>>
        );
        template<typename T, typename U>
        CPP_concept_bool common_reference_with =
            CPP_fragment(concepts::common_reference_with_, T, U);

        template<typename T, typename U>
        CPP_concept_fragment(common_with_, (T, U),
            same_as<common_type_t<T, U>,
                    common_type_t<U, T>> &&
            convertible_to<T, common_type_t<T, U>> &&
            convertible_to<U, common_type_t<T, U>> &&
            common_reference_with<
                std::add_lvalue_reference_t<T const>,
                std::add_lvalue_reference_t<U const>> &&
            common_reference_with<
                std::add_lvalue_reference_t<common_type_t<T, U>>,
                common_reference_t<
                    std::add_lvalue_reference_t<T const>,
                    std::add_lvalue_reference_t<U const>>>
        );
        template<typename T, typename U>
        CPP_concept_bool common_with =
            CPP_fragment(concepts::common_with_, T, U);

        template<typename T>
        CPP_concept_bool integral =
            std::is_integral<T>::value;

        template<typename T>
        CPP_concept_bool signed_integral =
            integral<T> &&
            std::is_signed<T>::value;

        template<typename T>
        CPP_concept_bool unsigned_integral =
            integral<T> &&
            !signed_integral<T>;

        template<typename T, typename U>
        CPP_concept_bool assignable_from =
            std::is_lvalue_reference<T>::value &&
            CPP_requires ((T) t, (U &&) u) //
            (
                t = (U &&) u,
                requires_<same_as<T, decltype(t = (U &&) u)>>
            );

        template<typename T>
        CPP_concept_bool swappable =
            CPP_requires ((T &) t, (T &) u) //
            (
                concepts::swap(t, u)
            );

        template<typename T, typename U>
        CPP_concept_bool swappable_with =
            common_reference_with<detail::as_cref_t<T>, detail::as_cref_t<U>> &&
            CPP_requires ((T &&) t, (U &&) u) //
            (
                concepts::swap(CPP_fwd(t), CPP_fwd(t)),
                concepts::swap(CPP_fwd(u), CPP_fwd(u)),
                concepts::swap(CPP_fwd(u), CPP_fwd(t)),
                concepts::swap(CPP_fwd(t), CPP_fwd(u))
            );

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Comparison concepts
        ////////////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        CPP_concept_bool equality_comparable =
            detail::weakly_equality_comparable_with_<T, T>;

        template<typename T, typename U>
        CPP_concept_fragment(equality_comparable_with_, (T, U),
            equality_comparable<
                common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
        );
        template<typename T, typename U>
        CPP_concept_bool equality_comparable_with =
            equality_comparable<T> &&
            equality_comparable<U> &&
            detail::weakly_equality_comparable_with_<T, U> &&
            common_reference_with<detail::as_cref_t<T>, detail::as_cref_t<U>> &&
            CPP_fragment(concepts::equality_comparable_with_, T, U);

        template<typename T>
        CPP_concept_bool totally_ordered =
            equality_comparable<T> &&
            CPP_requires ((detail::as_cref_t<T>) t, (detail::as_cref_t<T>) u) //
            (
                t < u ? 1 : 0,
                t > u ? 1 : 0,
                u <= t ? 1 : 0,
                u >= t ? 1 : 0
            );

        template<typename T, typename U>
        CPP_concept_fragment(totally_ordered_with_, (T, U),
            totally_ordered<
                common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
        );
        template<typename T, typename U>
        CPP_concept_bool totally_ordered_with =
            CPP_requires ((detail::as_cref_t<T>) t, (detail::as_cref_t<U>) u) //
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
            CPP_fragment(concepts::totally_ordered_with_, T, U);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Object concepts
        ////////////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        CPP_concept_bool destructible =
            std::is_nothrow_destructible<T>::value;

        template<typename T, typename... Args>
        CPP_concept_bool constructible_from =
            destructible<T> &&
            META_IS_CONSTRUCTIBLE(T, Args...);

        template<typename T>
        CPP_concept_bool default_constructible =
            constructible_from<T>;

        template<typename T>
        CPP_concept_bool move_constructible =
            constructible_from<T, T> &&
            convertible_to<T, T>;

        template<typename T>
        CPP_concept_bool copy_constructible =
            move_constructible<T> &&
            constructible_from<T, T &> &&
            constructible_from<T, T const &> &&
            constructible_from<T, T const> &&
            convertible_to<T &, T> &&
            convertible_to<T const &, T> &&
            convertible_to<T const, T>;

        template<typename T>
        CPP_concept_bool movable =
            std::is_object<T>::value &&
            move_constructible<T> &&
            assignable_from<T &, T> &&
            swappable<T>;

        template<typename T>
        CPP_concept_bool copyable =
            copy_constructible<T> &&
            movable<T> &&
            assignable_from<T &, T const &>;

        template<typename T>
        CPP_concept_bool semiregular =
            copyable<T> &&
            default_constructible<T>;
            // Axiom: copies are independent. See Fundamentals of Generic Programming
            // http://www.stepanovpapers.com/DeSt98.pdf

        template<typename T>
        CPP_concept_bool regular =
            semiregular<T> &&
            equality_comparable<T>;

        namespace defer
        {
            template<bool B>
            CPP_concept is_true =
                CPP_defer_(defs::is_true, B);

            template<typename... Ts>
            CPP_concept type =
                CPP_defer(defs::type, meta::list<Ts...>);

            template<class T, template<typename...> class Trait, typename... Args>
            CPP_concept satisfies =
                CPP_defer_(defs::satisfies, CPP_type(T), Trait, Args...);

            template<typename A, typename B>
            CPP_concept same_as =
                CPP_defer(defs::same_as, A, B);

            /// \cond
            template<typename A, typename B>
            CPP_concept not_same_as_ =
                CPP_defer(defs::not_same_as_, A, B);
            /// \endcond

            template<typename From, typename To>
            CPP_concept convertible_to =
                CPP_defer(defs::convertible_to, From, To);

            template<typename T, typename U>
            CPP_concept derived_from =
                CPP_defer(defs::derived_from, T, U);

            template<typename T, typename U>
            CPP_concept common_reference_with =
                CPP_defer(defs::common_reference_with, T, U);

            template<typename T, typename U>
            CPP_concept common_with =
                CPP_defer(defs::common_with, T, U);

            template<typename T>
            CPP_concept integral =
                CPP_defer(defs::integral, T);

            template<typename T>
            CPP_concept signed_integral =
                CPP_defer(defs::signed_integral, T);

            template<typename T>
            CPP_concept unsigned_integral =
                CPP_defer(defs::unsigned_integral, T);

            template<typename T, typename U>
            CPP_concept assignable_from =
                CPP_defer(defs::assignable_from, T, U);

            template<typename T>
            CPP_concept swappable =
                CPP_defer(defs::swappable, T);

            template<typename T, typename U>
            CPP_concept swappable_with =
                CPP_defer(defs::swappable_with, T, U);

            template<typename T>
            CPP_concept equality_comparable =
                CPP_defer(defs::equality_comparable, T);

            template<typename T, typename U>
            CPP_concept equality_comparable_with =
                CPP_defer(defs::equality_comparable_with, T, U);

            template<typename T>
            CPP_concept totally_ordered =
                CPP_defer(defs::totally_ordered, T);

            template<typename T, typename U>
            CPP_concept totally_ordered_with =
                CPP_defer(defs::totally_ordered_with, T, U);

            template<typename T>
            CPP_concept destructible =
                CPP_defer(defs::destructible, T);

            template<typename T, typename... Args>
            CPP_concept constructible_from =
                CPP_defer_(defs::constructible_from, CPP_type(T), Args...);

            template<typename T>
            CPP_concept default_constructible =
                CPP_defer(defs::default_constructible, T);

            template<typename T>
            CPP_concept move_constructible =
                CPP_defer(defs::move_constructible, T);

            template<typename T>
            CPP_concept copy_constructible =
                CPP_defer(defs::copy_constructible, T);

            template<typename T>
            CPP_concept movable =
                CPP_defer(defs::movable, T);

            template<typename T>
            CPP_concept copyable =
                CPP_defer(defs::copyable, T);

            template<typename T>
            CPP_concept semiregular =
                CPP_defer(defs::semiregular, T);

            template<typename T>
            CPP_concept regular =
                CPP_defer(defs::regular, T);
        }
    } // inline namespace defs
} // namespace concepts

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
