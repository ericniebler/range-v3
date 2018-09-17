/// \file
//  Concepts emulation library
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

#ifndef CONCEPT_S_HPP
#define CONCEPT_S_HPP

// clang-format off

#include <initializer_list>
#include <utility>
#include <type_traits>
#include <concepts/swap.hpp>
#include <concepts/type_traits.hpp>

// disable buggy compatibility warning about "requires" and "concept" being
// C++20 keywords.
#if defined(__clang__) || defined(__GNUC__)
#define CONCEPT_PP_IGNORE_CXX2A_COMPAT_BEGIN \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"") \
    _Pragma("GCC diagnostic ignored \"-Wpragmas\"") \
    _Pragma("GCC diagnostic ignored \"-Wc++2a-compat\"") \
    _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"") \
    /**/
#define CONCEPT_PP_IGNORE_CXX2A_COMPAT_END \
    _Pragma("GCC diagnostic pop")
#else
#define CONCEPT_PP_IGNORE_CXX2A_COMPAT_BEGIN
#define CONCEPT_PP_IGNORE_CXX2A_COMPAT_END
#endif

CONCEPT_PP_IGNORE_CXX2A_COMPAT_BEGIN

#if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606
#define CONCEPT_INLINE_VAR inline
#else
#define CONCEPT_INLINE_VAR
#endif

#ifdef __clang__
#define CONCEPT_PP_IS_SAME(...) __is_same(__VA_ARGS__)
#elif defined(__GNUC__) && __GNUC__ >= 6
#define CONCEPT_PP_IS_SAME(...) __is_same_as(__VA_ARGS__)
#else
#define CONCEPT_PP_IS_SAME(...) std::is_same<__VA_ARGS__>::value
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
#if __cplusplus > 201703L
#define CONCEPT_CXX_VA_OPT_(...) CONCEPT_PP_CHECK(__VA_OPT__(,) 1)
#define CONCEPT_CXX_VA_OPT CONCEPT_CXX_VA_OPT_(~)
#else
#define CONCEPT_CXX_VA_OPT 0
#endif
#endif // CONCEPT_CXX_VA_OPT

#define CONCEPT_PP_CAT_(X, ...)  X ## __VA_ARGS__
#define CONCEPT_PP_CAT(X, ...)   CONCEPT_PP_CAT_(X, __VA_ARGS__)
#define CONCEPT_PP_CAT2_(X, ...) X ## __VA_ARGS__
#define CONCEPT_PP_CAT2(X, ...)  CONCEPT_PP_CAT2_(X, __VA_ARGS__)

#define CONCEPT_PP_EVAL(X, ...) X(__VA_ARGS__)
#define CONCEPT_PP_EVAL2(X, ...) X(__VA_ARGS__)

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

#define CONCEPT_PP_IIF(BIT) CONCEPT_PP_CAT_(CONCEPT_PP_IIF_, BIT)
#define CONCEPT_PP_IIF_0(TRUE, ...) __VA_ARGS__
#define CONCEPT_PP_IIF_1(TRUE, ...) TRUE

#define CONCEPT_PP_EMPTY()
#define CONCEPT_PP_COMMA() ,
#define CONCEPT_PP_COMMA_IIF(X)                                                 \
    CONCEPT_PP_IIF(X)(CONCEPT_PP_EMPTY, CONCEPT_PP_COMMA)()

#define CONCEPT_assert(...)                                                     \
    static_assert((bool) (__VA_ARGS__),                                         \
        "Concept assertion failed : " #__VA_ARGS__)
#define CONCEPT_assert_msg static_assert

////////////////////////////////////////////////////////////////////////////////
// CONCEPT_DEF
//   For defining concepts with a syntax symilar to C++20. For example:
//
//     CONCEPT_DEF(
//         // The Assignable concept from the C++20
//         template(typename T, typename U)
//         concept Assignable,
//             requires (T t, U &&u) (
//                 t = (U &&) u,
//                 ::concepts::requires_<Same<decltype(t = (U &&) u), T>>
//             ) &&
//             std::is_lvalue_reference_v<T>
//     );
#define CONCEPT_def(DECL, ...)                                                  \
    CONCEPT_PP_EVAL(                                                            \
        CONCEPT_PP_DECL_DEF,                                                    \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_DECL_, DECL),                             \
        __VA_ARGS__)                                                            \
    /**/
#define CONCEPT_PP_DECL_DEF_NAME(...)                                           \
    CONCEPT_PP_CAT(CONCEPT_PP_DEF_, __VA_ARGS__),                               \
    /**/
#define CONCEPT_PP_DECL_DEF(TPARAM, NAME, ...)                                  \
    CONCEPT_PP_CAT(CONCEPT_PP_DECL_DEF_, CONCEPT_PP_IS_PAREN(NAME))(            \
        TPARAM,                                                                 \
        NAME,                                                                   \
        __VA_ARGS__)                                                            \
    /**/
// The defn is of the form:
//   template(typename A, typename B = void, typename... Rest)
//   (concept Name)(A, B, Rest...),
//      // requirements...
#define CONCEPT_PP_DECL_DEF_1(TPARAM, NAME, ...)                                \
    CONCEPT_PP_EVAL2(                                                           \
        CONCEPT_PP_DECL_DEF_IMPL,                                               \
        TPARAM,                                                                 \
        CONCEPT_PP_DECL_DEF_NAME NAME,                                          \
        __VA_ARGS__)                                                            \
    /**/
// The defn is of the form:
//   template(typename A, typename B)
//   concept Name,
//      // requirements...
// Compute the template arguments (A, B) from the template introducer.
#define CONCEPT_PP_DECL_DEF_0(TPARAM, NAME, ...)                                \
    CONCEPT_PP_DECL_DEF_IMPL(                                                   \
        TPARAM,                                                                 \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_, NAME),                                  \
        (CONCEPT_PP_CAT(CONCEPT_PP_AUX_, TPARAM)),                              \
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
#if defined(__cpp_concepts) && __cpp_concepts > 0
// No requires expression
#define CONCEPT_PP_DEF_IMPL_0(...)                                              \
    __VA_ARGS__                                                                 \
    /**/
// Requires expression
#define CONCEPT_PP_DEF_IMPL_1(...)                                              \
    CONCEPT_PP_CAT(CONCEPT_PP_DEF_IMPL_1_, __VA_ARGS__)                         \
    /**/
#define CONCEPT_PP_DEF_IMPL_1_requires                                          \
    requires CONCEPT_PP_DEF_IMPL_1_REQUIRES                                     \
    /**/
#define CONCEPT_PP_DEF_IMPL_1_REQUIRES(...)                                     \
    (__VA_ARGS__) CONCEPT_PP_DEF_IMPL_1_REQUIRES_BODY                           \
    /**/
#define CONCEPT_PP_DEF_IMPL_1_REQUIRES_BODY(...)                                \
    { __VA_ARGS__; }                                                            \
    /**/
#define CONCEPT_PP_DECL_DEF_IMPL(TPARAM, NAME, ARGS, ...)                       \
    inline namespace _eager_ {                                                  \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                 \
        concept bool NAME = CONCEPT_PP_DEF_IMPL(__VA_ARGS__,)(__VA_ARGS__);     \
    }                                                                           \
    struct CONCEPT_PP_CAT(NAME, Concept) {                                      \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                 \
        struct Eval {                                                           \
            using Concept = CONCEPT_PP_CAT(NAME, Concept);                      \
            explicit constexpr operator bool() const noexcept {                 \
                return (bool) _eager_::NAME<CONCEPT_PP_EXPAND ARGS>;            \
            }                                                                   \
            constexpr auto operator!() const noexcept {                         \
                return ::concepts::detail::Not<Eval>{};                         \
            }                                                                   \
            template<typename That>                                               \
            constexpr auto operator&&(That) const noexcept {                    \
                return ::concepts::detail::And<Eval, That>{};                   \
            }                                                                   \
        };                                                                      \
    };                                                                          \
    namespace lazy {                                                            \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                 \
        CONCEPT_INLINE_VAR constexpr auto NAME =                                \
            CONCEPT_PP_CAT(NAME, Concept)::Eval<CONCEPT_PP_EXPAND ARGS>{};      \
    }                                                                           \
    namespace defer {                                                           \
        using namespace _eager_;                                                \
    }                                                                           \
    using _concepts_int_ = int                                                  \
    /**/
#else
// No requires expression:
#define CONCEPT_PP_DEF_IMPL_0(...)                                              \
    () -> std::enable_if_t<bool(__VA_ARGS__), int>                              \
    /**/
// Requires expression:
#define CONCEPT_PP_DEF_IMPL_1(...)                                              \
    CONCEPT_PP_CAT(CONCEPT_PP_DEF_IMPL_1_, __VA_ARGS__) ), int>                 \
    /**/
#define CONCEPT_PP_DEF_IMPL_1_requires                                          \
    CONCEPT_PP_DEF_IMPL_1_REQUIRES                                              \
    /**/
#define CONCEPT_PP_DEF_IMPL_1_REQUIRES(...)                                     \
    (__VA_ARGS__) -> std::enable_if_t<bool(                                     \
        ::concepts::detail::requires_  CONCEPT_PP_DEF_REQUIRES_BODY             \
    /**/
 #define CONCEPT_PP_DEF_REQUIRES_BODY(...)                                      \
    <decltype(__VA_ARGS__, void())>()                                           \
    /**/
#define CONCEPT_PP_DECL_DEF_IMPL(TPARAM, NAME, ARGS, ...)                       \
    struct CONCEPT_PP_CAT(NAME, Concept) {                                      \
        using Concept = CONCEPT_PP_CAT(NAME, Concept);                          \
        CONCEPT_PP_IGNORE_CXX2A_COMPAT_BEGIN                                    \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                 \
        static auto Requires_ CONCEPT_PP_DEF_IMPL(__VA_ARGS__,)(__VA_ARGS__);   \
        CONCEPT_PP_IGNORE_CXX2A_COMPAT_END                                      \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                 \
        struct Eval {                                                           \
            template<typename C_ = Concept>                                       \
            static constexpr decltype(                                          \
                &C_::template Requires_<CONCEPT_PP_EXPAND ARGS>, true)          \
            impl(int) noexcept { return true; }                                 \
            static constexpr bool impl(long) noexcept { return false; }         \
            explicit constexpr operator bool() const noexcept {                 \
                return Eval::impl(0);                                           \
            }                                                                   \
            constexpr auto operator!() const noexcept {                         \
                return ::concepts::detail::Not<Eval>{};                         \
            }                                                                   \
            template<typename That>                                               \
            constexpr auto operator&&(That) const noexcept {                    \
                return ::concepts::detail::And<Eval, That>{};                   \
            }                                                                   \
        };                                                                      \
    };                                                                          \
    CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                     \
    CONCEPT_INLINE_VAR constexpr bool NAME =                                    \
        (bool)CONCEPT_PP_CAT(NAME, Concept)::Eval<CONCEPT_PP_EXPAND ARGS>{};    \
    namespace lazy {                                                            \
        CONCEPT_PP_CAT(CONCEPT_PP_DEF_, TPARAM)                                 \
        CONCEPT_INLINE_VAR constexpr auto NAME =                                \
            CONCEPT_PP_CAT(NAME, Concept)::Eval<CONCEPT_PP_EXPAND ARGS>{};      \
    }                                                                           \
    namespace defer {                                                           \
        using namespace lazy;                                                   \
    }                                                                           \
    using _concepts_int_ = int                                                  \
    /**/
#endif

#define CONCEPT_PP_REQUIRES_PROBE_requires                                      \
    CONCEPT_PP_PROBE(~)                                                         \
    /**/
#define CONCEPT_PP_DEF_IMPL(REQUIRES, ...)                                      \
    CONCEPT_PP_CAT(                                                             \
        CONCEPT_PP_DEF_IMPL_,                                                   \
        CONCEPT_PP_CHECK(CONCEPT_PP_CAT(CONCEPT_PP_REQUIRES_PROBE_, REQUIRES))) \
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
#define CONCEPT_PP_DEF_bool
#define CONCEPT_PP_DEF_size_t
#define CONCEPT_PP_DEF_unsigned
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

////////////////////////////////////////////////////////////////////////////////
// CONCEPT_template
// Usage:
//   CONCEPT_template (class A, typename B)
//     (requires Concept1<A> && Concept2<B>)
//   void foo(A a, B b)
//   {}
// or
//   CONCEPT_template (class A, typename B)
//     (requires requires (expr1, expr2, expr3) && Concept1<A> && Concept2<B>)
//   void foo(A a, B b)
//   {}
#if defined(__cpp_concepts) && __cpp_concepts > 0
#define CONCEPT_template(...)                                                   \
    template<__VA_ARGS__> CONCEPT_TEMPLATE_AUX_                                 \
    /**/
#define CONCEPT_TEMPLATE_AUX_(...)                                              \
    CONCEPT_TEMPLATE_AUX_4(CONCEPT_PP_CAT(CONCEPT_TEMPLATE_AUX_3_, __VA_ARGS__))\
    /**/
#define CONCEPT_TEMPLATE_AUX_3_requires
#define CONCEPT_TEMPLATE_AUX_4(...)                                             \
    CONCEPT_TEMPLATE_AUX_5(__VA_ARGS__,)(__VA_ARGS__)                           \
    /**/
#define CONCEPT_TEMPLATE_AUX_5(REQUIRES, ...)                                   \
    CONCEPT_PP_CAT(                                                             \
        CONCEPT_TEMPLATE_AUX_5_,                                                \
        CONCEPT_PP_CHECK(CONCEPT_PP_CAT(CONCEPT_PP_REQUIRES_PROBE_, REQUIRES))) \
    /**/
// No requires expression:
#define CONCEPT_TEMPLATE_AUX_5_0(...)                                           \
    requires __VA_ARGS__                                                        \
    /**/
// Requires expression
#define CONCEPT_TEMPLATE_AUX_5_1(...)                                           \
    CONCEPT_PP_CAT(CONCEPT_TEMPLATE_AUX_6_, __VA_ARGS__)                        \
    /**/
#define CONCEPT_TEMPLATE_AUX_6_requires(...)\
    requires requires { __VA_ARGS__; }
#else
#define CONCEPT_template(...)                                                   \
    template<__VA_ARGS__ CONCEPT_TEMPLATE_AUX_
#define CONCEPT_TEMPLATE_AUX_(...) ,                                            \
    int (*CONCEPT_PP_CAT(_pushmi_concept_unique_, __LINE__))[                   \
        CONCEPT_COUNTER] = nullptr,                                             \
    std::enable_if_t<CONCEPT_PP_CAT(_pushmi_concept_unique_, __LINE__) ||       \
        bool(CONCEPT_TEMPLATE_AUX_4(CONCEPT_PP_CAT(                             \
            CONCEPT_TEMPLATE_AUX_3_, __VA_ARGS__))), int> = 0>                  \
    /**/
#define CONCEPT_TEMPLATE_AUX_3_requires
#define CONCEPT_TEMPLATE_AUX_4(...)                                             \
    CONCEPT_TEMPLATE_AUX_5(__VA_ARGS__,)(__VA_ARGS__)                           \
    /**/
#define CONCEPT_TEMPLATE_AUX_5(REQUIRES, ...)                                   \
    CONCEPT_PP_CAT(                                                             \
        CONCEPT_TEMPLATE_AUX_5_,                                                \
        CONCEPT_PP_CHECK(CONCEPT_PP_CAT(CONCEPT_PP_REQUIRES_PROBE_, REQUIRES))) \
    /**/
// No requires expression:
#define CONCEPT_TEMPLATE_AUX_5_0(...)                                           \
    __VA_ARGS__                                                                 \
    /**/
#define CONCEPT_TEMPLATE_AUX_5_1(...)                                           \
    CONCEPT_PP_CAT(CONCEPT_TEMPLATE_AUX_6_, __VA_ARGS__)                        \
    /**/
#define CONCEPT_TEMPLATE_AUX_6_requires(...)                                    \
    ::concepts::detail::requires_<decltype(__VA_ARGS__)>()                      \
    /**/
#endif


#if defined(__cpp_concepts) && __cpp_concepts > 0
#define CONCEPT_BROKEN_SUBSUMPTION(...)
#define CONCEPT_TYPE_CONSTRAINT(...) __VA_ARGS__
#else
#define CONCEPT_BROKEN_SUBSUMPTION(...) __VA_ARGS__
#define CONCEPT_TYPE_CONSTRAINT(...) class
#endif


#if defined(__cpp_concepts) && __cpp_concepts > 0
#define CONCEPT_PP_CONSTRAINED_USING(REQUIRES, NAME, ...)                       \
    requires REQUIRES                                                           \
  using NAME __VA_ARGS__                                                        \
  /**/
#else
#define CONCEPT_PP_CONSTRAINED_USING(REQUIRES, NAME, ...)                       \
  using NAME std::enable_if_t<bool(REQUIRES), __VA_ARGS__>                      \
  /**/
#endif

#define CONCEPT_requires(...) CONCEPT_template(typename = void)(requires __VA_ARGS__)

#define CONCEPT_broken_friend_return_type(...)                                  \
    ::concepts::detail::enable_if_t<__VA_ARGS__,                                \
    CONCEPT_BROKEN_FRIEND_RETURN_TYPE_AUX_                                      \
    /**/
#define CONCEPT_BROKEN_FRIEND_RETURN_TYPE_AUX_(...)                             \
    CONCEPT_BROKEN_FRIEND_RETURN_TYPE_AUX_3_(CONCEPT_PP_CAT(                    \
        CONCEPT_TEMPLATE_AUX_2_, __VA_ARGS__))                                  \
    /**/
#define CONCEPT_TEMPLATE_AUX_2_requires
#define CONCEPT_BROKEN_FRIEND_RETURN_TYPE_AUX_3_(...)                           \
    bool(__VA_ARGS__)>                                                          \
    /**/

#if defined(__cpp_concepts) && __cpp_concepts > 0
#define CONCEPT_return_type(...)                                                \
    __VA_ARGS__ CONCEPT_PP_EXPAND                                               \
    /**/
#else
#define CONCEPT_return_type                                                     \
    CONCEPT_broken_friend_return_type
#endif

namespace concepts 
{
    inline namespace v1
    {
        template<bool B>
        using bool_ = std::integral_constant<bool, B>;

#if defined(__cpp_fold_expressions) && __cpp_fold_expressions >= 201603
        template<bool...Bs>
        CONCEPT_INLINE_VAR constexpr bool and_v = (Bs &&...);

        template<bool...Bs>
        CONCEPT_INLINE_VAR constexpr bool or_v = (Bs ||...);
#else
        namespace detail
        {
            template<bool...>
            struct bools;
        } // namespace detail

        template<bool...Bs>
        CONCEPT_INLINE_VAR constexpr bool and_v =
        CONCEPT_PP_IS_SAME(detail::bools<Bs..., true>, detail::bools<true, Bs...>);

        template<bool...Bs>
        CONCEPT_INLINE_VAR constexpr bool or_v =
        !CONCEPT_PP_IS_SAME(detail::bools<Bs..., false>, detail::bools<false, Bs...>);
#endif

        namespace detail
        {
            template<typename>
            inline constexpr bool requires_()
            {
                return true;
            }
            template<typename T, typename U>
            struct And;
            template<typename T>
            struct Not {
                explicit constexpr operator bool() const noexcept {
                    return !(bool) T{};
                }
                constexpr auto operator!() const noexcept {
                    return T{};
                }
                template<typename That>
                constexpr auto operator&&(That) const noexcept {
                    return And<Not, That>{};
                }
            };
            template<typename T, typename U>
            struct And {
                static constexpr bool impl(std::false_type) noexcept { return false; }
                static constexpr bool impl(std::true_type) noexcept { return (bool) U{}; }
                explicit constexpr operator bool() const noexcept {
                    return And::impl(bool_<(bool) T{}>{});
                }
                constexpr auto operator!() const noexcept {
                    return Not<And>{};
                }
                template<typename That>
                constexpr auto operator&&(That) const noexcept {
                    return detail::And<And, That>{};
                }
            };

            template<typename T, bool Enable>
            using enable_if_t = std::enable_if_t<Enable, T>;
        } // namespace detail

        template<typename T>
        constexpr bool implicitly_convertible_to(T) {
            return true;
        }

#ifdef __clang__
        template<bool B>
        std::enable_if_t<B> requires_()
        {}
#else
        template<bool B>
        CONCEPT_INLINE_VAR constexpr std::enable_if_t<B, int> requires_ = 0;
#endif

        inline namespace defs
        {
            ////////////////////////////////////////////////////////////////////////////////////////
            // Utility concepts
            ////////////////////////////////////////////////////////////////////////////////////////
            
            CONCEPT_def(
                template(typename... Args)
                (concept True)(Args...),
                    true
            );

            CONCEPT_def(
                template(typename T, template<typename...> class C, typename... Args)
                (concept Valid)(T, C, Args...),
                    True< C<T, Args...> >
            );

            CONCEPT_def(
                template (class T, template<typename...> class Trait, typename... Args)
                (concept Satisfies)(T, Trait, Args...),
                    static_cast<bool>(Trait<T>::type::value)
            );

            CONCEPT_def(
                template (bool...Bs)
                (concept And)(Bs...),
                    and_v<Bs...>
            );

            CONCEPT_def(
                template (bool...Bs)
                (concept Or)(Bs...),
                    or_v<Bs...>
            );

            ////////////////////////////////////////////////////////////////////////////////////////
            // Core language concepts
            ////////////////////////////////////////////////////////////////////////////////////////

            CONCEPT_def
            (
                template(typename A, typename B)
                concept Same,
                    CONCEPT_PP_IS_SAME(A, B) && CONCEPT_PP_IS_SAME(B, A)
            );

            /// \cond
            CONCEPT_def
            (
                template(typename From, typename To)
                concept ImplicitlyConvertibleTo,
                    std::is_convertible<From, To>::value
            );

            CONCEPT_def
            (
                template(typename From, typename To)
                concept ExplicitlyConvertibleTo,
                    requires (From (&from)())
                    (
                        static_cast<To>(from())
                    )
            );
            /// \endcond

            CONCEPT_def
            (
                template(typename From, typename To)
                concept ConvertibleTo,
                    ImplicitlyConvertibleTo<From, To> &&
                    ExplicitlyConvertibleTo<From, To>
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept DerivedFrom,
                    std::is_base_of<U, T>::value &&
                    ConvertibleTo<T const volatile *, U const volatile *>
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept CommonReference,
                    Same<common_reference_t<T, U>, common_reference_t<U, T>> &&
                    ConvertibleTo<T, common_reference_t<T, U>> &&
                    ConvertibleTo<U, common_reference_t<T, U>>
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept Common,
                    Same<common_type_t<T, U>, common_type_t<U, T>> &&
                    ConvertibleTo<T, common_type_t<T, U>> &&
                    ConvertibleTo<U, common_type_t<T, U>> &&
                    CommonReference<
                        typename std::add_lvalue_reference<T const>::type,
                        typename std::add_lvalue_reference<U const>::type> &&
                    CommonReference<
                        typename std::add_lvalue_reference<common_type_t<T, U>>::type,
                        common_reference_t<
                            typename std::add_lvalue_reference<T const>::type,
                            typename std::add_lvalue_reference<U const>::type>>
            );

            CONCEPT_def
            (
                template(typename T)
                concept Integral,
                    std::is_integral<T>::value
            );

            CONCEPT_def
            (
                template(typename T)
                concept SignedIntegral,
                    Integral<T> &&
                    std::is_signed<T>::value
            );

            CONCEPT_def
            (
                template(typename T)
                concept UnsignedIntegral,
                    Integral<T> &&
                    !SignedIntegral<T>
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept Assignable,
                    requires (T t, U &&u)
                    (
                        t = (U &&) u,
                        requires_<Same<T, decltype(t = (U &&) u)>>
                    ) &&

                    std::is_lvalue_reference<T>::value
            );

            CONCEPT_def
            (
                template(typename T)
                concept Swappable,
                    requires (T &t, T &u)
                    (
                        concepts::swap(t, u)
                    )
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept SwappableWith,
                    requires (T &&t, U &&u)
                    (
                        concepts::swap((T &&) t, (T &&) t),
                        concepts::swap((U &&) u, (U &&) u),
                        concepts::swap((U &&) u, (T &&) t),
                        concepts::swap((T &&) t, (U &&) u)
                    ) &&
                    CommonReference<detail::as_cref_t<T>, detail::as_cref_t<U>>
            );

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Comparison concepts
            ////////////////////////////////////////////////////////////////////////////////////////////
            CONCEPT_def
            (
                template(typename T, typename U)
                concept WeaklyEqualityComparableWith,
                    requires (detail::as_cref_t<T> t, detail::as_cref_t<U> u)
                    (
                        (t == u) ? 1 : 0,
                        (t != u) ? 1 : 0,
                        (u == t) ? 1 : 0,
                        (u != t) ? 1 : 0
                    )
            );

            CONCEPT_def
            (
                template(typename T)
                concept EqualityComparable,
                    WeaklyEqualityComparableWith<T, T>
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept EqualityComparableWith,
                    EqualityComparable<T> &&
                    EqualityComparable<U> &&
                    WeaklyEqualityComparableWith<T, U> &&
                    CommonReference<detail::as_cref_t<T>, detail::as_cref_t<U>> &&
                    EqualityComparable<
                        common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
            );

            CONCEPT_def
            (
                template(typename T)
                concept StrictTotallyOrdered,
                    requires (detail::as_cref_t<T> t, detail::as_cref_t<T> u)
                    (
                        t < u ? 1 : 0,
                        t > u ? 1 : 0,
                        (u <= t) ? 1 : 0,
                        (u >= t) ? 1 : 0
                    ) &&
                    EqualityComparable<T>
            );

            CONCEPT_def
            (
                template(typename T, typename U)
                concept StrictTotallyOrderedWith,
                    requires (detail::as_cref_t<T> t, detail::as_cref_t<U> u)
                    (
                        t < u ? 1 : 0,
                        t > u ? 1 : 0,
                        (t <= u) ? 1 : 0,
                        (t >= u) ? 1 : 0,
                        u < t ? 1 : 0,
                        u > t ? 1 : 0,
                        (u <= t) ? 1 : 0,
                        (u >= t) ? 1 : 0
                    ) &&
                    StrictTotallyOrdered<T> &&
                    StrictTotallyOrdered<U> &&
                    EqualityComparableWith<T, U> &&
                    CommonReference<detail::as_cref_t<T>, detail::as_cref_t<U>> &&
                    StrictTotallyOrdered<
                        common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
            );

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Object concepts
            ////////////////////////////////////////////////////////////////////////////////////////////

            CONCEPT_def
            (
                template(typename T)
                concept Destructible,
                    std::is_nothrow_destructible<T>::value
            );

            CONCEPT_def
            (
                template(typename T, typename... Args)
                (concept Constructible)(T, Args...),
                    Destructible<T> &&
                    std::is_constructible<T, Args...>::value
            );

            CONCEPT_def
            (
                template(typename T)
                concept DefaultConstructible,
                    Constructible<T>
            );

            CONCEPT_def
            (
                template(typename T)
                concept MoveConstructible,
                    Constructible<T, T> &&
                    ConvertibleTo<T, T>
            );

            CONCEPT_def
            (
                template(typename T)
                concept CopyConstructible,
                    MoveConstructible<T> &&
                    Constructible<T, T &> &&
                    Constructible<T, T const &> &&
                    Constructible<T, T const> &&
                    ConvertibleTo<T &, T> &&
                    ConvertibleTo<T const &, T> &&
                    ConvertibleTo<T const, T>
            );

            CONCEPT_def
            (
                template(typename T)
                concept Movable,
                    MoveConstructible<T> &&
                    std::is_object<T>::value &&
                    Assignable<T &, T> &&
                    Swappable<T>
            );

            CONCEPT_def
            (
                template(typename T)
                concept Copyable,
                    Movable<T> &&
                    CopyConstructible<T> &&
                    // Spec requires this to be validated
                    Assignable<T &, T const &> &&
                    // Spec does not require these to be validated
                    Assignable<T &, T &> &&
                    Assignable<T &, T const>
            );

            CONCEPT_def
            (
                template(typename T)
                concept Semiregular,
                    Copyable<T> &&
                    DefaultConstructible<T>
                // Axiom: copies are independent. See Fundamentals of Generic Programming
                // http://www.stepanovpapers.com/DeSt98.pdf
            );

            CONCEPT_def
            (
                template(typename T)
                concept Regular,
                    Semiregular<T> &&
                    EqualityComparable<T>
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

    } // namespace v1
} // namespace concepts

CONCEPT_PP_IGNORE_CXX2A_COMPAT_END

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
