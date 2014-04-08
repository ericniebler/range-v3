// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Acknowledgements: Thanks for Paul Fultz for the suggestions that
//                   concepts can be ordinary Boolean metafunctions.
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_CONCEPTS_HPP
#define RANGES_V3_UTILITY_CONCEPTS_HPP

#include <iosfwd>
#include <utility>
#include <type_traits>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/typelist.hpp>
#include <range/v3/utility/logical_ops.hpp>
#include <range/v3/utility/common_type.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            constexpr struct void_tester
            {
                template<typename T>
                friend int operator,(T&&, void_tester);
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
                    typename std::enable_if<std::is_same<T,U>::value, int>::type;
            } same_type {};

            constexpr struct is_true_t
            {
                template<typename Bool>
                auto operator()(Bool) const ->
                    typename std::enable_if<Bool::value, int>::type;
            } is_true {};

            constexpr struct is_false_t
            {
                template<typename Bool>
                auto operator()(Bool) const ->
                    typename std::enable_if<!Bool::value, int>::type;
            } is_false {};

            template<typename Concept, typename ...Ts>
            struct models_;

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
        }

        namespace concepts
        {
            using detail::void_;
            using detail::is_void;
            using detail::valid_expr;
            using detail::same_type;
            using detail::is_true;
            using detail::is_false;

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
                decltype(concepts::returns_<int>(static_cast<T>(u)));

            template<typename T, typename U>
            auto has_type(U &&) ->
                typename std::enable_if<std::is_same<T,U>::value, int>::type;

            template<typename ...Concepts>
            struct refines
              : virtual detail::base_concept_t<Concepts>...
            {
                template<typename...Ts>
                void requires(Ts &&...);
            };

            template<typename Concept, typename ...Ts>
            using models = decltype(detail::models_<Concept, Ts...>{}());

            template<typename Concept, typename ...Ts>
            auto model_of(Ts &&...) ->
                typename std::enable_if<(concepts::models<Concept, Ts...>()), int>::type;
        }

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // base_concepts
            typelist<> base_concepts_of_impl_(void *);

            template<typename...BaseConcepts>
            typelist<BaseConcepts...> base_concepts_of_impl_(concepts::refines<BaseConcepts...> *);

            template<typename Concept>
            using base_concepts_of_t = decltype(detail::base_concepts_of_impl_((Concept *)nullptr));

            ////////////////////////////////////////////////////////////////////////////////////
            // models_refines_
            template<typename Concepts, typename...Ts>
            struct models_refines_;

            template<typename...Ts>
            struct models_refines_<typelist<>, Ts...>
              : std::true_type
            {};

            template<typename...Concepts, typename...Ts>
            struct models_refines_<typelist<Concepts...>, Ts...>
              : logical_and<concepts::models<Concepts, Ts...>::value...>
            {};

            ////////////////////////////////////////////////////////////////////////////////////
            // models_
            template<typename Concept, typename ...Ts>
            struct models_
            {
                auto operator()() const -> std::false_type;

                template<typename C = Concept, typename = decltype(C{}.requires(std::declval<Ts>()...))>
                auto operator()() -> models_refines_<base_concepts_of_t<C>, Ts...>;
            };

            template<typename Concept, typename...Args, typename ...Ts>
            struct models_<Concept(Args...), Ts...>
              : models_<Concept, typelist_element_t<Args::value, typelist<Ts...>>...>
            {};

            template<typename Concept, typename...Ts>
            struct models_hack_for_clang
              : concepts::models<Concept, Ts...>
            {};
        }

        namespace concepts
        {
            ////////////////////////////////////////////////////////////////////////////////////////////
            // most_refined
            // Find the first concept in a list of concepts that is modeled by the Args
            template<typename Concepts, typename...Ts>
            struct most_refined
              : typelist_front<
                    typelist_find_if_t<
                        meta_bind_back<detail::models_hack_for_clang, Ts...>::template apply,
                        Concepts>>
            {};

            template<typename Concepts, typename...Ts>
            using most_refined_t = meta_apply<most_refined, Concepts, Ts...>;

            struct Same
            {
                template<typename T, typename U>
                auto requires(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(t, u)
                    ));
            };

            struct Convertible
            {
                template<typename T, typename U>
                auto requires(T &&t, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<U>(t)
                    ));
            };

            struct Derived
            {
                template<typename T, typename U>
                auto requires(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_base_of<U, T>{})
                    ));
            };

            struct Common
            {
                template<typename T, typename U>
                using common_t = common_type_t<T, U>;

                template<typename T, typename U, typename C = common_t<T, U>>
                auto requires(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<C>((T &&) t),
                        concepts::convertible_to<C>((U &&) u)
                    ));
            };

            struct Integral
            {
                template<typename T>
                auto requires(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_integral<T>{})
                    ));
            };

            struct SignedIntegral
              : refines<Integral>
            {
                template<typename T>
                auto requires(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_signed<T>{})
                    ));
            };

            struct Destructible
            {
                template<typename T>
                auto requires(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_destructible<T>{})
                    ));
            };

            struct Constructible
              : refines<Destructible(_1)>
            {
                template<typename T, typename ...Us>
                auto requires(T &&, Us &&...) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_constructible<T, Us...>{})
                    ));
            };

            struct DefaultConstructible
              : refines<Destructible>
            {
                template<typename T>
                auto requires(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_default_constructible<T>{})
                    ));
            };

            struct MoveConstructible
              : refines<Destructible>
            {
                template<typename T>
                auto requires(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_move_constructible<T>{})
                    ));
            };

            struct CopyConstructible
              : refines<MoveConstructible>
            {
                template<typename T>
                auto requires(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_copy_constructible<T>{})
                    ));
            };

            struct Assignable
            {
                template<typename T, typename U>
                auto requires(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_assignable<T, U>{})
                    ));
            };

            struct MoveAssignable
            {
                template<typename T>
                auto requires(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_move_assignable<T>{})
                    ));
            };

            struct CopyAssignable
              : refines<MoveAssignable>
            {
                template<typename T>
                auto requires(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_copy_assignable<T>{})
                    ));
            };

            struct Movable
              : refines<MoveConstructible, MoveAssignable>
            {};

            struct Copyable
              : refines<CopyConstructible, CopyAssignable>
            {};

            struct EqualityComparable
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == t),
                        concepts::convertible_to<bool>(t != t)
                    ));

                // Cross-type equality comparison from N3351:
                // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3351.pdf
                template<typename T, typename U, typename C = common_type_t<T, U>>
                auto requires(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<EqualityComparable>(t),
                        concepts::model_of<EqualityComparable>(u),
                        concepts::model_of<Common>(t, u),
                        concepts::model_of<EqualityComparable>(static_cast<C>(t)),
                        concepts::convertible_to<bool>(t == u),
                        concepts::convertible_to<bool>(u == t),
                        concepts::convertible_to<bool>(t != u),
                        concepts::convertible_to<bool>(u != t)
                    ));
            };

            struct WeaklyOrdered
            {
                template<typename T>
                auto requires(T t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t < t),
                        concepts::convertible_to<bool>(t > t),
                        concepts::convertible_to<bool>(t <= t),
                        concepts::convertible_to<bool>(t >= t)
                    ));

                template<typename T, typename U, typename C = common_type_t<T, U>>
                auto requires(T t, U u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<WeaklyOrdered>(t),
                        concepts::model_of<WeaklyOrdered>(u),
                        concepts::model_of<Common>(t, u),
                        concepts::model_of<WeaklyOrdered>(static_cast<C>(t)),
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
                auto requires(T) -> std::true_type;

                template<typename T, typename U>
                auto requires(T t, U u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<TotallyOrdered>(t),
                        concepts::model_of<TotallyOrdered>(u)
                    ));
            };

            struct Allocatable
            {
                template<typename T>
                auto requires(T t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T *>(new T),
                        ((delete new T), 42),
                        concepts::has_type<T *>(new T[42]),
                        ((delete[] new T[42]), 42)
                    ));
            };

            struct SemiRegular
              : refines<DefaultConstructible, CopyConstructible, Destructible, CopyAssignable,
                    Allocatable>
            {
                template<typename T>
                auto requires(T t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T *>(&t)
                    ));
            };

            struct Regular
              : refines<SemiRegular, EqualityComparable>
            {};

            struct Swappable
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        ranges::swap((T&&)t, (T&&)t)
                    ));

                template<typename T, typename U>
                auto requires(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        ranges::swap((T&&)t, (U&&)u),
                        ranges::swap((U&&)u, (T&&)t)
                    ));
            };

            struct Callable
            {
                template<typename Fun, typename ...Args>
                using result_t = decltype(std::declval<Fun>()(std::declval<Args>()...));

                template<typename Fun, typename ...Args>
                auto requires(Fun && fun, Args &&... args) -> decltype(
                    concepts::valid_expr(
                        (static_cast<void>(std::forward<Fun>(fun)(
                            std::forward<Args>(args)...)), 42)
                    ));
            };

            struct Predicate
              : refines<Callable>
            {
                template<typename Fun, typename ...Args>
                auto requires(Fun && fun, Args &&... args) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(
                            std::forward<Fun>(fun)(std::forward<Args>(args)...))
                    ));
            };

            struct Addable
            {
                template<typename T, typename U>
                using result_t = decltype(std::declval<T>() + std::declval<U>());

                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t + t
                    ));

                template<typename T, typename U>
                auto requires(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        t + u
                    ));
            };
        }

        template<typename T, typename U>
        using Same = concepts::models<concepts::Same, T, U>;

        template<typename T, typename U>
        using Convertible = concepts::models<concepts::Convertible, T, U>;

        template<typename T, typename U>
        using Derived = concepts::models<concepts::Derived, T, U>;

        template<typename T, typename U>
        using Common = concepts::models<concepts::Common, T, U>;

        template<typename T>
        using Integral = concepts::models<concepts::Integral, T>;

        template<typename T>
        using SignedIntegral = concepts::models<concepts::SignedIntegral, T>;

        template<typename T>
        using Destructible = concepts::models<concepts::Destructible, T>;

        template<typename T, typename...Us>
        using Constructible = concepts::models<concepts::Constructible, T, Us...>;

        template<typename T>
        using DefaultConstructible = concepts::models<concepts::DefaultConstructible, T>;

        template<typename T>
        using MoveConstructible = concepts::models<concepts::MoveConstructible, T>;

        template<typename T>
        using CopyConstructible = concepts::models<concepts::CopyConstructible, T>;

        template<typename T, typename U>
        using Assignable = concepts::models<concepts::Assignable, T, U>;

        template<typename T>
        using MoveAssignable = concepts::models<concepts::MoveAssignable, T>;

        template<typename T>
        using CopyAssignable = concepts::models<concepts::CopyAssignable, T>;

        template<typename T>
        using Movable = concepts::models<concepts::Movable, T>;

        template<typename T>
        using Copyable = concepts::models<concepts::Copyable, T>;

        template<typename T, typename U = T>
        using EqualityComparable = concepts::models<concepts::EqualityComparable, T, U>;

        template<typename T, typename U = T>
        using WeaklyOrdered = concepts::models<concepts::WeaklyOrdered, T, U>;

        template<typename T, typename U = T>
        using TotallyOrdered = concepts::models<concepts::TotallyOrdered, T, U>;

        template<typename T>
        using Allocatable = concepts::models<concepts::Allocatable, T>;

        template<typename T>
        using SemiRegular = concepts::models<concepts::SemiRegular, T>;

        template<typename T>
        using Regular = concepts::models<concepts::Regular, T>;

        template<typename T, typename U = T>
        using Swappable = concepts::models<concepts::Swappable, T, U>;

        template<typename Fun, typename ...Args>
        using Callable = concepts::models<concepts::Callable, Fun, Args...>;

        template<typename Fun, typename ...Args>
        using Predicate = concepts::models<concepts::Predicate, Fun, Args...>;

        template<typename T, typename U = T>
        using Addable = concepts::models<concepts::Addable, T, U>;

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename Enable = void>
            struct difference_type
            {};

            template<typename T>
            struct difference_type<T *, void>
            {
                using type = std::ptrdiff_t;
            };

            template<typename T>
            struct difference_type<T[], void>
            {
                using type = std::ptrdiff_t;
            };

            template<typename T, std::size_t N>
            struct difference_type<T[N], void>
            {
                using type = std::ptrdiff_t;
            };

            template<typename T>
            struct difference_type<T, always_t<void, typename T::difference_type>>
            {
                using type = typename T::difference_type;
            };

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename Enable = void>
            struct value_type
            {};

            template<typename T>
            struct value_type<T *, void>
              : std::remove_cv<T>
            {};

            template<typename T>
            struct value_type<T[], void>
              : std::remove_cv<T>
            {};

            template<typename T, std::size_t N>
            struct value_type<T[N], void>
              : std::remove_cv<T>
            {};

            template<typename T>
            struct value_type<T, always_t<void, typename T::value_type>>
            {
                using type = typename T::value_type;
            };

            template<typename T>
            struct value_type<T, enable_if_t<std::is_base_of<std::ios_base, T>::value, void>>
            {
                using type = typename T::char_type;
            };
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        //
        template<typename T>
        struct difference_type
          : detail::difference_type<detail::uncvref_t<T>>
        {};

        template<typename T>
        struct value_type
          : detail::value_type<detail::uncvref_t<T>>
        {};
    }
}

#define CONCEPT_PP_CAT_(X, Y) X ## Y
#define CONCEPT_PP_CAT(X, Y)  CONCEPT_PP_CAT_(X, Y)

#define CONCEPT_REQUIRES_(...)                                                      \
    int CONCEPT_PP_CAT(_concept_requires_, __LINE__) = 42,                          \
    ranges::enable_if_t<                                                            \
        (CONCEPT_PP_CAT(_concept_requires_, __LINE__) == 43) || (__VA_ARGS__)       \
    > = 0                                                                           \
    /**/

#define CONCEPT_REQUIRES(...)                                                       \
    template<CONCEPT_REQUIRES_(__VA_ARGS__)>                                        \
    /**/

#define CONCEPT_ASSERT(...) static_assert((__VA_ARGS__), "Concept check failed")

#include <range/v3/utility/predicate.hpp>

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
