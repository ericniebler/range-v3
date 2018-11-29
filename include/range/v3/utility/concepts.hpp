/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements: Thanks for Paul Fultz for the suggestions that
//                   concepts can be ordinary Boolean metafunctions.
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_CONCEPTS_HPP
#define RANGES_V3_UTILITY_CONCEPTS_HPP

#include <initializer_list>
#include <type_traits>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/nullptr_v.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            constexpr struct void_tester
            {
                template<typename T>
                friend int operator,(T &&, void_tester);
            } void_ {};

            constexpr struct is_void_t
            {
                int operator()(detail::void_tester) const;
            } is_void {};

            constexpr struct valid_expr_t
            {
                template<typename ...T>
                void operator()(T &&...) const;
            } valid_expr {};

            constexpr struct same_type_t
            {
                template<typename T, typename U>
                auto operator()(T &&, U &&) const ->
                    meta::if_<std::is_same<T,U>, int>;
            } same_type {};

            constexpr struct is_true_t
            {
                template<typename Bool_>
                auto operator()(Bool_) const ->
                    meta::if_c<Bool_::value, int>;
            } is_true {};

            constexpr struct is_false_t
            {
                template<typename Bool_>
                auto operator()(Bool_) const ->
                    meta::if_c<!Bool_::value, int>;
            } is_false {};

            template<typename Concept>
            struct base_concept
            {
                using type = Concept;
            };

            template<typename Concept, typename ...Args>
            struct base_concept<Concept(Args...)>
            {
                using type = Concept;
            };

            template<typename Concept>
            using base_concept_t = typename base_concept<Concept>::type;

            template<typename Concept, typename Enable = void>
            struct base_concepts_of
            {
                using type = meta::list<>;
            };

            template<typename Concept>
            struct base_concepts_of<Concept, meta::void_<typename Concept::base_concepts_t>>
            {
                using type = typename Concept::base_concepts_t;
            };

            template<typename Concept>
            using base_concepts_of_t = meta::_t<base_concepts_of<Concept>>;

            template<typename...Ts>
            auto models_(any) ->
                std::false_type;

#if (defined(__GNUC__) && !defined(__clang__) && __GNUC__ == 5 && __GNUC_MINOR__ < 5) || \
    defined(RANGES_WORKAROUND_MSVC_701425)
            template<typename T>
            T gcc_bugs_bugs_bugs(T);

            template<typename...Ts, typename Concept,
                typename = decltype(gcc_bugs_bugs_bugs(&Concept::template requires_<Ts...>))>
#else
            template<typename...Ts, typename Concept,
                typename = decltype(&Concept::template requires_<Ts...>)>
#endif
            auto models_(Concept *) ->
                meta::apply<
                    meta::quote<meta::lazy::strict_and>,
                    meta::transform<
                        base_concepts_of_t<Concept>,
                        meta::bind_back<meta::quote<concepts::models>, Ts...>>>;

            template<typename List>
            struct most_refined_
            {};

            template<typename Head, typename...Tail>
            struct most_refined_<meta::list<Head, Tail...>>
            {
                using type = Head;
                constexpr operator Head*() const { return nullptr; }
                constexpr Head* operator()() const { return nullptr; }
            };
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{
        ///
        namespace concepts
        {
            using detail::void_;
            using detail::is_void;
            using detail::valid_expr;
            using detail::same_type;
            using detail::is_true;
            using detail::is_false;
            using ranges::uncvref_t;

            using _1 = std::integral_constant<int, 0>;
            using _2 = std::integral_constant<int, 1>;
            using _3 = std::integral_constant<int, 2>;
            using _4 = std::integral_constant<int, 3>;
            using _5 = std::integral_constant<int, 4>;
            using _6 = std::integral_constant<int, 5>;
            using _7 = std::integral_constant<int, 6>;
            using _8 = std::integral_constant<int, 7>;
            using _9 = std::integral_constant<int, 8>;

            template<typename Ret, typename T>
            Ret returns_(T const &);

            template<typename T, typename U>
            auto convertible_to(U && u) ->
                decltype(concepts::returns_<int>(static_cast<T>((U &&) u)));

            template<typename T, typename U>
            auto has_type(U &&) ->
                meta::if_<std::is_same<T, U>, int>;

            ////////////////////////////////////////////////////////////////////////////////////////////
            // refines
            template<typename ...Concepts>
            struct refines
              : virtual detail::base_concept_t<Concepts>...
            {
                // So that we don't create these by accident, since it's surprisingly expensive to set
                // up the vtable, given all the virtual bases.
                refines() = delete;

                using base_concepts_t = meta::list<Concepts...>;

                template<typename...Ts>
                void requires_();
            };

            ////////////////////////////////////////////////////////////////////////////////////////////
            // models
            template<typename Concept, typename...Ts>
            struct models
              : meta::bool_<meta::_t<decltype(detail::models_<Ts...>(_nullptr_v<Concept>()))>::value>
            {};

            template<typename Concept, typename...Args, typename...Ts>
            struct models<Concept(Args...), Ts...>
              : models<Concept, meta::at<meta::list<Ts...>, Args>...>
            {};

            ////////////////////////////////////////////////////////////////////////////////////////////
            // model_of
            template<typename Concept, typename ...Ts>
            auto model_of(Ts &&...) ->
                meta::if_c<concepts::models<Concept, Ts...>::value, int>;

            template<typename Concept, typename ...Ts>
            auto model_of() ->
                meta::if_c<concepts::models<Concept, Ts...>::value, int>;

            ////////////////////////////////////////////////////////////////////////////////////////////
            // most_refined
            // Find the first concept in a list of concepts that is modeled by the Args
            template<typename Concepts, typename...Ts>
            struct most_refined
              : detail::most_refined_<
                    meta::find_if<
                        Concepts,
                        meta::bind_back<meta::quote<models>, Ts...>>>
            {};

            template<typename Concepts, typename...Ts>
            using most_refined_t = meta::_t<most_refined<Concepts, Ts...>>;

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Core language concepts
            ////////////////////////////////////////////////////////////////////////////////////////////

            struct Satisfies
            {
                template<typename T, typename Trait, typename ...Ts>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(meta::invoke<Trait, T, Ts...>{})
                    ));
            };

            struct Same
            {
                template<typename ...Ts>
                struct same : std::true_type {};
                template<typename T, typename ...Us>
                struct same<T, Us...> : meta::and_c<std::is_same<T, Us>::value...> {};
                template<typename ...Ts>
                using same_t = meta::_t<same<Ts...>>;

                template<typename ...Ts>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(same_t<Ts...>{})
                    ));
            };

            /// \cond
            struct ImplicitlyConvertibleTo
            {
                template<typename From, typename To>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(detail::is_convertible<From, To>{})
                    ));
            };

            struct ExplicitlyConvertibleTo
            {
                template<typename From, typename To>
                auto requires_(From (&from)()) -> decltype(
                    concepts::valid_expr(
                        ((void) static_cast<To>(from()), 42)
                    ));
            };
            /// \endcond

            struct ConvertibleTo
              : refines<ImplicitlyConvertibleTo, ExplicitlyConvertibleTo>
            {};

            struct DerivedFrom
            {
                template<typename T, typename U>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_base_of<U, T>{}),
                        concepts::is_true(detail::is_convertible<
                            meta::_t<std::remove_cv<T>> *, meta::_t<std::remove_cv<U>> *>{})
                    ));
            };

            struct CommonReference
            {
                template<typename T, typename U, typename...Rest>
                using reference_t = common_reference_t<T, U, Rest...>;

                template<typename T, typename U>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Same, reference_t<T, U>, reference_t<U, T>>(),
                        concepts::model_of<ConvertibleTo, T, reference_t<T, U>>(),
                        concepts::model_of<ConvertibleTo, U, reference_t<T, U>>()
                    ));

                template<typename T, typename U, typename...Rest,
                    typename CommonReference_ = CommonReference>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<CommonReference_, T, U>(),
                        concepts::model_of<CommonReference_, reference_t<T, U>, Rest...>()
                    ));
            };

            struct Common
            {
                template<typename T, typename U, typename...Rest>
                using value_t = common_type_t<T, U, Rest...>;

                template<typename T, typename U>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_same<uncvref_t<T>, uncvref_t<U>>{})
                    ));

                template<typename T, typename U>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_false(std::is_same<uncvref_t<T>, uncvref_t<U>>{}),
                        concepts::model_of<Same, value_t<T, U>, value_t<U, T>>(),
                        concepts::model_of<ConvertibleTo, T, value_t<T, U>>(),
                        concepts::model_of<ConvertibleTo, U, value_t<T, U>>(),
                        concepts::model_of<
                            CommonReference, detail::as_cref_t<T>, detail::as_cref_t<U>>(),
                        concepts::model_of<
                            CommonReference,
                            value_t<T, U> &,
                            common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>()
                    ));

                template<typename T, typename U, typename...Rest,
                    typename Common_ = Common>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Common_, T, U>(),
                        concepts::model_of<Common_, value_t<T, U>, Rest...>()
                    ));
            };

            struct Integral
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_integral<T>{})
                    ));
            };

            struct SignedIntegral
              : refines<Integral>
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_signed<T>{})
                    ));
            };

            struct UnsignedIntegral
              : refines<Integral>
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_false(std::is_signed<T>{})
                    ));
            };

            struct Assignable
            {
                template<typename T, typename U>
                auto requires_(T &&t, U &&u) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_lvalue_reference<T>{}),
                        concepts::model_of<
                            CommonReference, detail::as_cref_t<T>, detail::as_cref_t<U>>(),
                        concepts::has_type<T>((T &&) t = (U &&) u)
                    ));
            };

            struct Swappable
            {
                template<typename T>
                auto requires_(T &&t) -> decltype(
                    concepts::valid_expr(
                        ((void)ranges::swap((T &&) t, (T &&) t), 42)
                    ));

                template<typename T, typename U>
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Swappable, T>(),
                        concepts::model_of<Swappable, U>(),
                        concepts::model_of<
                            CommonReference, detail::as_cref_t<T>, detail::as_cref_t<U>>(),
                        ((void)ranges::swap((T &&) t, (U &&) u), 42),
                        ((void)ranges::swap((U &&) u, (T &&) t), 42)
                    ));
            };

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Comparison concepts
            ////////////////////////////////////////////////////////////////////////////////////////////
            struct WeaklyEqualityComparable
            {
                template<typename T, typename U>
                auto requires_(detail::as_cref_t<T> t, detail::as_cref_t<U> u) -> decltype(
                    // Not to spec: doesn't compare to a Boolean trait
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == u),
                        concepts::convertible_to<bool>(t != u),
                        concepts::convertible_to<bool>(u == t),
                        concepts::convertible_to<bool>(u != t)
                    ));
            };

            struct EqualityComparable
            {
                template<typename T>
                auto requires_(detail::as_cref_t<T> t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == t),
                        concepts::convertible_to<bool>(t != t)
                    ));

                template<typename T, typename U>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_same<T, U>{}),
                        concepts::model_of<EqualityComparable, T>()
                    ));

                // Cross-type equality comparison from N3351:
                // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3351.pdf
                template<typename T, typename U,
                    typename C = common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_false(std::is_same<T, U>{}),
                        concepts::model_of<EqualityComparable, T>(),
                        concepts::model_of<EqualityComparable, U>(),
                        concepts::model_of<WeaklyEqualityComparable, T, U>(),
                        concepts::model_of<
                            CommonReference, detail::as_cref_t<T>, detail::as_cref_t<U>>(),
                        concepts::model_of<EqualityComparable, C>()
                    ));
            };

            struct WeaklyOrdered
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t < t),
                        concepts::convertible_to<bool>(t > t),
                        concepts::convertible_to<bool>(t <= t),
                        concepts::convertible_to<bool>(t >= t)
                    ));

                template<typename T, typename U,
                    typename C = common_reference_t<detail::as_cref_t<T>, detail::as_cref_t<U>>>
                auto requires_(detail::as_cref_t<T> t, detail::as_cref_t<U> u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<WeaklyOrdered, T>(),
                        concepts::model_of<WeaklyOrdered, U>(),
                        concepts::model_of<
                            CommonReference, detail::as_cref_t<T>, detail::as_cref_t<U>>(),
                        concepts::model_of<WeaklyOrdered, C>(),
                        concepts::convertible_to<bool>(t < u),
                        concepts::convertible_to<bool>(u < t),
                        concepts::convertible_to<bool>(t > u),
                        concepts::convertible_to<bool>(u > t),
                        concepts::convertible_to<bool>(t <= u),
                        concepts::convertible_to<bool>(u <= t),
                        concepts::convertible_to<bool>(t >= u),
                        concepts::convertible_to<bool>(u >= t)
                    ));
            };

            struct TotallyOrdered
              : refines<EqualityComparable, WeaklyOrdered>
            {
                template<typename T>
                void requires_();

                template<typename T, typename U>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<TotallyOrdered, T>(),
                        concepts::model_of<TotallyOrdered, U>()
                    ));
            };

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Object concepts
            ////////////////////////////////////////////////////////////////////////////////////////////

            struct Destructible
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_nothrow_destructible<T>())
                    ));
            };

            struct Constructible
              : refines<Destructible(_1)>
            {
                template<typename T, typename... Args>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_constructible<T, Args...>{})
                    ));
            };

            struct DefaultConstructible
              : refines<Constructible>
            {
                template<typename T>
                void requires_();
            };

            struct MoveConstructible
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Constructible, T, T>(),
                        concepts::model_of<ConvertibleTo, T, T>()
                    ));
            };

            struct CopyConstructible
              : refines<MoveConstructible>
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Constructible, T, T &>(),
                        concepts::model_of<Constructible, T, T const &>(),
                        concepts::model_of<Constructible, T, T const>(),
                        concepts::model_of<ConvertibleTo, T &, T>(),
                        concepts::model_of<ConvertibleTo, T const &, T>(),
                        concepts::model_of<ConvertibleTo, T const, T>()
                    ));
            };

            struct Movable
              : refines<MoveConstructible>
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_object<T>{}),
                        concepts::model_of<Assignable, T &, T>(),
                        concepts::model_of<Swappable, T &>()
                    ));
            };

            struct Copyable
              : refines<Movable, CopyConstructible>
            {
                template<typename T>
                auto requires_() -> decltype(
                    concepts::valid_expr(
                        // Spec requires this to be validated
                        concepts::model_of<Assignable, T &, T const &>(),
                        // Spec does not require these to be validated
                        concepts::model_of<Assignable, T &, T &>(),
                        concepts::model_of<Assignable, T &, T const>()
                    ));
            };

            struct SemiRegular
              : refines<Copyable, DefaultConstructible>
            {
                // Axiom: copies are independent. See Fundamentals of Generic Programming
                // http://www.stepanovpapers.com/DeSt98.pdf
            };

            struct Regular
              : refines<SemiRegular, EqualityComparable>
            {};
        }

        template<typename T, template<typename...> class Trait, typename ...Ts>
        using Satisfies = concepts::models<concepts::Satisfies, T, meta::quote<Trait>, Ts...>;

        template<typename ...Ts>
        using Same = concepts::Same::same_t<Ts...>; // This handles void better than using the Same concept

        template<typename T, typename U>
        using ImplicitlyConvertibleTo =
            concepts::models<concepts::ImplicitlyConvertibleTo, T, U>;

        template<typename T, typename U>
        using ExplicitlyConvertibleTo =
            concepts::models<concepts::ExplicitlyConvertibleTo, T, U>;

        template<typename T, typename U>
        using ConvertibleTo = concepts::models<concepts::ConvertibleTo, T, U>;

        template<typename T, typename U>
        using DerivedFrom = concepts::models<concepts::DerivedFrom, T, U>;

        template<typename T, typename U, typename...Rest>
        using CommonReference =
            concepts::models<concepts::CommonReference, T, U, Rest...>;

        template<typename T, typename U, typename...Rest>
        using Common =
            concepts::models<concepts::Common, T, U, Rest...>;

        template<typename T>
        using Integral = concepts::models<concepts::Integral, T>;

        template<typename T>
        using SignedIntegral = concepts::models<concepts::SignedIntegral, T>;

        template<typename T>
        using UnsignedIntegral = concepts::models<concepts::UnsignedIntegral, T>;

        template<typename T>
        using Destructible = concepts::models<concepts::Destructible, T>;

        template<typename T, typename ...Args>
        using Constructible = concepts::models<concepts::Constructible, T, Args...>;

        template<typename T>
        using DefaultConstructible = concepts::models<concepts::DefaultConstructible, T>;

        template<typename T>
        using MoveConstructible = concepts::models<concepts::MoveConstructible, T>;

        template<typename T>
        using CopyConstructible = concepts::models<concepts::CopyConstructible, T>;

        template<typename T, typename U>
        using Assignable = concepts::models<concepts::Assignable, T, U>;

        template<typename T>
        using Movable = concepts::models<concepts::Movable, T>;

        template<typename T>
        using Copyable = concepts::models<concepts::Copyable, T>;

        template<typename T, typename U>
        using WeaklyEqualityComparable = concepts::models<concepts::WeaklyEqualityComparable, T, U>;

        template<typename T, typename U = T>
        using EqualityComparable = concepts::models<concepts::EqualityComparable, T, U>;

        template<typename T, typename U = T>
        using WeaklyOrdered = concepts::models<concepts::WeaklyOrdered, T, U>;

        template<typename T, typename U = T>
        using TotallyOrdered = concepts::models<concepts::TotallyOrdered, T, U>;

        template<typename T>
        using SemiRegular = concepts::models<concepts::SemiRegular, T>;

        template<typename T>
        using Regular = concepts::models<concepts::Regular, T>;

        template<typename T, typename U = T>
        using Swappable = concepts::models<concepts::Swappable, T, U>;
    }
}

#define CONCEPT_PP_CAT_(X, Y) X ## Y
#define CONCEPT_PP_CAT(X, Y)  CONCEPT_PP_CAT_(X, Y)

/// \addtogroup group-concepts
/// @{
#define CONCEPT_REQUIRES_(...)                                         \
    bool CONCEPT_PP_CAT(_concept_requires_, __LINE__) = false,         \
    typename std::enable_if<                                           \
        CONCEPT_PP_CAT(_concept_requires_, __LINE__) || (__VA_ARGS__)  \
    >::type* = nullptr                                                 \
    /**/

#define CONCEPT_REQUIRES(...) template<CONCEPT_REQUIRES_(__VA_ARGS__)> \
    /**/

#if RANGES_CXX_STATIC_ASSERT >= RANGES_CXX_STATIC_ASSERT_17
#define CONCEPT_ASSERT static_assert
#else
#define CONCEPT_ASSERT(...) static_assert((__VA_ARGS__), "Concept check failed: " #__VA_ARGS__)
#endif
/// @}

#define CONCEPT_ASSERT_MSG static_assert
/// @}

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
