/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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
#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/nullptr_v.hpp>

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

            #if defined(__GNUC__) && !defined(__clang__)
                template<typename ...T>
                void operator()(T const &...) const;
            #endif
            } valid_expr {};

            constexpr struct same_type_t
            {
                template<typename T, typename U>
                auto operator()(T &&, U &&) const ->
                    meta::if_<std::is_same<T,U>, int>;
            } same_type {};

            constexpr struct is_true_t
            {
                template<typename Bool>
                auto operator()(Bool) const ->
                    meta::if_c<Bool::value, int>;
            } is_true {};

            constexpr struct is_false_t
            {
                template<typename Bool>
                auto operator()(Bool) const ->
                    meta::if_c<!Bool::value, int>;
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

            template<typename...Ts, typename Concept,
                typename = decltype(std::declval<Concept &>().template requires_<Ts...>(std::declval<Ts>()...))>
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

            // Workaround https://llvm.org/bugs/show_bug.cgi?id=24181
            template<typename T>
            struct avoid_empty_braces_
              : std::false_type
            {};
            template<typename T>
            struct avoid_empty_braces_<std::initializer_list<T>>
              : std::true_type
            {};
            template<typename T>
            struct avoid_empty_braces_<T*>
              : std::true_type
            {};
            template<typename T>
            using avoid_empty_braces =
                meta::_t<avoid_empty_braces_<uncvref_t<T>>>;
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

            template<typename T>
            using val_t = meta::if_<std::is_rvalue_reference<T>, T, T &>;

            template<typename T>
            val_t<T> val();

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
                void requires_(Ts &&...);
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

            struct Same
            {
                template<typename ...Ts>
                struct same : std::true_type {};
                template<typename T, typename ...Us>
                struct same<T, Us...> : meta::strict_and<std::is_same<T, Us>...> {};
                template<typename ...Ts>
                using same_t = meta::_t<same<Ts...>>;

                template<typename ...Ts>
                auto requires_(Ts &&...) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(same_t<Ts...>{})
                    ));
            };

            /// \cond
            struct ImplicitlyConvertibleTo
            {
                template<typename T, typename U>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_convertible<T, U>{})
                    ));
            };

            struct ExplicitlyConvertibleTo
            {
                template<typename T, typename U>
                auto requires_(T && t, U &&) -> decltype(
                    concepts::valid_expr(
                        static_cast<U>(std::forward<T>(t))
                    ));
            };
            /// \endcond

            struct ConvertibleTo
              : refines<ImplicitlyConvertibleTo, ExplicitlyConvertibleTo>
            {};

            struct DerivedFrom
            {
                template<typename T, typename U>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_base_of<U, T>{})
                    ));
            };

            struct CommonReference
            {
                template<typename T, typename U, typename...Rest>
                using reference_t = common_reference_t<T, U, Rest...>;

                template<typename T, typename U,
                    typename C = reference_t<T, U>>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Same, C, reference_t<U, T>>(),
                        C(val<T>()),
                        C(val<U>())
                    ));

                template<typename T, typename U, typename...Rest,
                    typename CommonReference_ = CommonReference,
                    typename C = reference_t<T, U>>
                auto requires_(T &&, U &&, Rest&&...) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<CommonReference_, T, U>(),
                        concepts::model_of<CommonReference_, C, Rest...>()
                    ));
            };

            struct Common
            {
                template<typename T, typename U, typename...Rest>
                using value_t = common_type_t<T, U, Rest...>;

                template<typename T, typename U,
                    meta::if_<std::is_same<uncvref_t<T>, uncvref_t<U>>, int> = 0>
                auto requires_(T &&, U &&) ->
                    void;

                template<typename T, typename U,
                    meta::if_c<!std::is_same<uncvref_t<T>, uncvref_t<U>>::value, int> = 0,
                    typename C = value_t<T, U>,
                    typename R = CommonReference::reference_t<T const &, U const &>>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Same, C, value_t<U, T>>(),
                        C(val<T &&>()),
                        C(val<U &&>()),
                        concepts::model_of<CommonReference, T const &, U const &>(),
                        concepts::model_of<CommonReference, C &, R &&>()
                    ));

                template<typename T, typename U, typename...Rest,
                    typename Common_ = Common,
                    typename C = value_t<T, U>>
                auto requires_(T &&, U &&, Rest&&...) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Common_, T, U>(),
                        concepts::model_of<Common_, C, Rest...>()
                    ));
            };

            struct Integral
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_integral<T>{})
                    ));
            };

            struct SignedIntegral
              : refines<Integral>
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_signed<T>{})
                    ));
            };

            struct UnsignedIntegral
              : refines<Integral>
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_unsigned<T>{})
                    ));
            };

            struct Assignable
            {
                template<typename T, typename U>
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>(static_cast<T &&>(t) = static_cast<U &&>(u))
                    ));
            };

            struct Swappable
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        ((void)swap(std::declval<T>(), std::declval<T>()), 42)
                    ));

                template<typename T, typename U>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        ((void)swap(std::declval<T>(), std::declval<U>()), 42),
                        ((void)swap(std::declval<U>(), std::declval<T>()), 42)
                    ));
            };

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Comparison concepts
            ////////////////////////////////////////////////////////////////////////////////////////////
            struct WeaklyEqualityComparable
            {
                template<typename T, typename U>
                auto requires_(T && t, U && u) -> decltype(
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
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == t),
                        concepts::convertible_to<bool>(t != t)
                    ));

                template<typename T, typename U,
                    meta::if_<std::is_same<T, U>, int> = 0>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<EqualityComparable, T>()
                    ));

                // Cross-type equality comparison from N3351:
                // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3351.pdf
                template<typename T, typename U,
                    meta::if_c<!std::is_same<T, U>::value, int> = 0,
                    typename C = CommonReference::reference_t<T const &, U const &>>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<EqualityComparable, T>(),
                        concepts::model_of<EqualityComparable, U>(),
                        concepts::model_of<WeaklyEqualityComparable, T, U>(),
                        concepts::model_of<CommonReference, T const &, U const &>(),
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
                    typename C = CommonReference::reference_t<T const &, U const &>>
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<WeaklyOrdered, T>(),
                        concepts::model_of<WeaklyOrdered, U>(),
                        concepts::model_of<CommonReference, T const &, U const &>(),
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
                void requires_(T &&);

                template<typename T, typename U>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<TotallyOrdered>(val<T>()),
                        concepts::model_of<TotallyOrdered>(val<U>())
                    ));
            };

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Object concepts
            ////////////////////////////////////////////////////////////////////////////////////////////

            struct Destructible
            {
                template<typename T,
                    meta::if_<std::is_object<T>, int> = 0>
                auto requires_(T && t, T* const p = nullptr) -> decltype(
                    concepts::valid_expr(
                        ((void)t.~T(), 42),
                        concepts::is_true(std::is_nothrow_destructible<T>()),
                        concepts::has_type<T*>(&t),
                        concepts::has_type<const T*>(&std::declval<const T&>()),
                        ((void)delete p, 42),
                        ((void)delete[] p, 42)
                    ));
            };

            /// \cond
            struct ConstructibleObject
              : refines<Destructible(_1)>
            {
                template<typename T,
                    meta::if_<detail::avoid_empty_braces<T>, int> = 0>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        T(),
                        new T()
                    ));

                template<typename T,
                    meta::if_c<!detail::avoid_empty_braces<T>::value, int> = 0>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        T{},
                        new T{}
                    ));

                // Avoid CWG DR1467.
                template<typename T, typename U>
                using DR1467 =
#if defined(__clang__) && (__clang_major__ < 3 || ( __clang_major__ == 3 && __clang_minor__ <= 4) || (__clang_major__ == 6 && __clang_minor__ == 0))
                  // 3.4 has a bug involving uniform initialization and conversion operators,
                  // so just avoid brace initialization in all cases for construction from a
                  // single argument.
                  // also: Apple clang 3.5 seemingly still has the bug and reports its version incorrectly (__clang_major__ == 6 && __clang_minor__ == 0)
                  meta::bool_<true || std::is_same<T, U>::value>;
#else
                  std::is_same<uncvref_t<T>, uncvref_t<U>>;
#endif

                template<typename T, typename U,
                    meta::if_<DR1467<T, U>, int> = 0>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        T(std::declval<U>()),
                        new T(std::declval<U>())
                    ));

                template<typename T, typename U,
                    meta::if_c<!DR1467<T, U>::value, int> = 0>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        T{std::declval<U>()},
                        new T{std::declval<U>()}
                    ));

                template<typename T, typename U, typename V, typename... Args>
                auto requires_(T &&, U &&, V &&, meta::id_t<Args> &&...) -> decltype(
                    concepts::valid_expr(
                        T{std::declval<U>(), std::declval<V>(), std::declval<Args>()...},
                        new T{std::declval<U>(), std::declval<V>(), std::declval<Args>()...}
                    ));
            };

            struct BindableReference
            {
                template<typename T, typename... Args,
                    meta::if_<std::is_reference<T>, int> = 0>
                auto requires_(T &&, meta::id_t<Args> &&...) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_constructible<T, Args...>())
                    ));
            };
            /// \endcond

            struct Constructible
            {
                template<typename T, typename... Args,
                    meta::if_c<!std::is_reference<T>::value, int> = 0>
                auto requires_(T &&, meta::id_t<Args> &&...) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<ConstructibleObject, T, Args...>()
                    ));

                template<typename T, typename... Args,
                    meta::if_<std::is_reference<T>, int> = 0>
                auto requires_(T &&, meta::id_t<Args> &&...) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<BindableReference, T, Args...>()
                    ));
            };

            struct DefaultConstructible
              : refines<Constructible>
            {
                template<typename T,
                    meta::if_c<!detail::avoid_empty_braces<T>::value, int> = 0>
                auto requires_(T &&, const std::size_t n = 42) -> decltype(
                    concepts::valid_expr(
                        new T[n]{}
                    ));

                // Workaround https://llvm.org/bugs/show_bug.cgi?id=24181
                template<typename T,
                    meta::if_<detail::avoid_empty_braces<T>, int> = 0>
                auto requires_(T &&, const std::size_t n = 42) -> decltype(
                    concepts::valid_expr(
                        new T[n]()
                    ));
            };

            struct MoveConstructible
            {
                template<typename T, typename UnCvT = meta::_t<std::remove_cv<T>>>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Constructible, T, UnCvT &&>(),
                        concepts::model_of<ImplicitlyConvertibleTo, UnCvT &&, T>()
                    ));
            };

            struct CopyConstructible
              : refines<MoveConstructible>
            {
                template<typename T, typename UnCvT = meta::_t<std::remove_cv<T>>>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        // Spec requires these to be validated
                        concepts::model_of<Constructible, T, UnCvT const &>(),
                        concepts::model_of<ImplicitlyConvertibleTo, UnCvT const &, T>(),
                        // Spec does not require these to be validated
                        concepts::model_of<Constructible, T, UnCvT &>(),
                        concepts::model_of<ImplicitlyConvertibleTo, UnCvT &, T>(),
                        concepts::model_of<Constructible, T, UnCvT const &&>(),
                        concepts::model_of<ImplicitlyConvertibleTo, UnCvT const &&, T>()
                    ));
            };

            struct Movable
              : refines<MoveConstructible>
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Assignable, T &, T &&>(),
                        concepts::model_of<Swappable, T &>()
                    ));
            };

            struct Copyable
              : refines<Movable, CopyConstructible>
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        // Spec requires this to be validated
                        concepts::model_of<Assignable, T &, T const &>(),
                        // Spec does not require these to be validated
                        concepts::model_of<Assignable, T &, T &>(),
                        concepts::model_of<Assignable, T &, T const &&>()
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

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Function concepts
            ////////////////////////////////////////////////////////////////////////////////////////////

            struct Function
            {
                template<typename Fun, typename ...Args>
                using result_t = decltype(val<Fun>()(val<Args>()...));

                template<typename Fun, typename ...Args>
                auto requires_(Fun&& fun, Args&&... args) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<CopyConstructible, uncvref_t<Fun>>(),
                        ((void)val<Fun>()(val<Args>()...), 42)
                    ));
            };

            struct RegularFunction
              : refines<Function>
            {
                // Axiom: equality_preserving(f(args...))
            };

            struct Predicate
              : refines<RegularFunction>
            {
                template<typename Fun, typename ...Args>
                auto requires_(Fun&& fun, Args&&... args) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(val<Fun>()(val<Args>()...))
                    ));
            };

            struct Relation
              : refines<RegularFunction>
            {
                template<typename Fun, typename T>
                auto requires_(Fun&&, T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate>(val<Fun>(), val<T>(), val<T>())
                    ));

                template<typename Fun, typename T, typename U,
                    meta::if_<std::is_same<T, U>, int> = 0>
                auto requires_(Fun&&, T &&, T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate>(val<Fun>(), val<T>(), val<U>())
                    ));

                template<typename Fun, typename T, typename U,
                    meta::if_c<!std::is_same<T, U>::value, int> = 0,
                    typename C = CommonReference::reference_t<T const &, U const &>>
                auto requires_(Fun &&, T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Relation, Fun, T, T>(),
                        concepts::model_of<Relation, Fun, U, U>(),
                        concepts::model_of<CommonReference, T const &, U const &>(),
                        concepts::model_of<Relation, Fun, C, C>(),
                        concepts::model_of<Predicate, Fun, T, U>(),
                        concepts::model_of<Predicate, Fun, U, T>()
                    ));
            };
        }

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

        // This is an ugly hack around the fact that the core concept-checking
        // code doesn't handle void very well.
        template<typename T, typename U, typename...Rest>
        using CommonReference =
            meta::or_<
                meta::strict_and<std::is_void<T>, std::is_void<U>, std::is_void<Rest>...>,
                concepts::models<concepts::CommonReference, T, U, Rest...>>;

        template<typename T, typename U, typename...Rest>
        using Common =
            meta::or_<
                meta::strict_and<std::is_void<T>, std::is_void<U>, std::is_void<Rest>...>,
                concepts::models<concepts::Common, T, U, Rest...>>;

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

        template<typename Fun, typename ...Args>
        using Function = concepts::models<concepts::Function, Fun, Args...>;

        template<typename Fun, typename ...Args>
        using RegularFunction = concepts::models<concepts::RegularFunction, Fun, Args...>;

        template<typename Fun, typename ...Args>
        using Predicate = concepts::models<concepts::Predicate, Fun, Args...>;

        template<typename Fun, typename T, typename U = T>
        using Relation = concepts::models<concepts::Relation, Fun, T, U>;
    }
}

#define CONCEPT_PP_CAT_(X, Y) X ## Y
#define CONCEPT_PP_CAT(X, Y)  CONCEPT_PP_CAT_(X, Y)

/// \addtogroup group-concepts
/// @{
#define CONCEPT_REQUIRES_(...)                                                      \
    int CONCEPT_PP_CAT(_concept_requires_, __LINE__) = 42,                          \
    typename std::enable_if<                                                        \
        (CONCEPT_PP_CAT(_concept_requires_, __LINE__) == 43) || (__VA_ARGS__),      \
        int                                                                         \
    >::type = 0                                                                     \
    /**/

#define CONCEPT_REQUIRES(...)                                                       \
    template<                                                                       \
        int CONCEPT_PP_CAT(_concept_requires_, __LINE__) = 42,                      \
        typename std::enable_if<                                                    \
            (CONCEPT_PP_CAT(_concept_requires_, __LINE__) == 43) || (__VA_ARGS__),  \
            int                                                                     \
        >::type = 0>                                                                \
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
