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

#include <iosfwd>
#include <utility>
#include <type_traits>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/nullptr_v.hpp>

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
            struct base_concepts_of<Concept, void_t<typename Concept::base_concepts_t>>
            {
                using type = typename Concept::base_concepts_t;
            };

            template<typename Concept>
            using base_concepts_of_t = meta::eval<base_concepts_of<Concept>>;

            template<typename...Bools>
            struct lazy_and
            {
                static constexpr bool value{meta::fast_and<Bools...>::value};
            };

            template<typename...Ts>
            auto models_(any) ->
                std::false_type;

            template<typename...Ts, typename Concept,
                typename = decltype(std::declval<Concept &>().template requires_<Ts...>(std::declval<Ts>()...))>
            auto models_(Concept *) ->
                meta::apply_list<
                    meta::quote<lazy_and>,
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
                enable_if_t<std::is_same<T, U>::value>;

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
              : meta::bool_<decltype(detail::models_<Ts...>(_nullptr_v<Concept>()))::value>
            {};

            template<typename Concept, typename...Args, typename...Ts>
            struct models<Concept(Args...), Ts...>
              : models<Concept, meta::list_element<Args, meta::list<Ts...> >...>
            {};

            ////////////////////////////////////////////////////////////////////////////////////////////
            // model_of
            template<typename Concept, typename ...Ts>
            auto model_of(Ts &&...) ->
                enable_if_t<concepts::models<Concept, Ts...>::value>;

            template<typename Concept, typename ...Ts>
            auto model_of() ->
                enable_if_t<concepts::models<Concept, Ts...>::value>;

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
            using most_refined_t = meta::eval<most_refined<Concepts, Ts...>>;

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Core language concepts
            ////////////////////////////////////////////////////////////////////////////////////////////

            struct Same
            {
                template<typename T, typename U>
                auto requires_(T, U) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_same<T, U>{})
                    ));
            };

            struct Convertible
            {
                template<typename T, typename U>
                auto requires_(T, U) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_convertible<T, U>{})
                    ));
            };

            struct Derived
            {
                template<typename T, typename U>
                auto requires_(T, U) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_base_of<U, T>{})
                    ));
            };

            struct Common
            {
                template<typename T, typename U>
                using common_t = common_type_t<T, U>;

                template<typename T, typename U,
                    enable_if_t<std::is_same<uncvref_t<T>, uncvref_t<U>>::value> = 0>
                auto requires_(T, U) ->
                    void;

                template<typename T, typename U,
                    enable_if_t<!std::is_same<uncvref_t<T>, uncvref_t<U>>::value> = 0,
                    typename C = common_t<T, U>>
                auto requires_(T t, U u) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<C>(val<T>()),
                        concepts::convertible_to<C>(val<U>())
                    ));
            };

            struct Integral
            {
                template<typename T>
                auto requires_(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_integral<T>{})
                    ));
            };

            struct SignedIntegral
              : refines<Integral>
            {
                template<typename T>
                auto requires_(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_signed<T>{})
                    ));
            };

            struct UnsignedIntegral
              : refines<Integral>
            {
                template<typename T>
                auto requires_(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_unsigned<T>{})
                    ));
            };

            struct Destructible
            {
                template<typename T>
                auto requires_(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_destructible<T>{})
                    ));
            };

            struct Constructible
              : refines<Destructible(_1)>
            {
                template<typename T, typename ...Us>
                auto requires_(T, Us...) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_constructible<T, Us...>{})
                    ));
            };

            struct DefaultConstructible
              : refines<Destructible>
            {
                template<typename T>
                auto requires_(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_default_constructible<T>{})
                    ));
            };

            struct MoveConstructible
              : refines<Destructible>
            {
                template<typename T>
                auto requires_(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_move_constructible<T>{})
                    ));
            };

            struct CopyConstructible
              : refines<MoveConstructible>
            {
                template<typename T>
                auto requires_(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_copy_constructible<T>{})
                    ));
            };

            struct Assignable
            {
                template<typename T, typename U>
                auto requires_(T, U) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_assignable<T &, U>{})
                    ));
            };

            struct MoveAssignable
            {
                template<typename T>
                auto requires_(T) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_move_assignable<T>{})
                    ));
            };

            struct CopyAssignable
              : refines<MoveAssignable>
            {
                template<typename T>
                auto requires_(T) -> decltype(
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
                auto requires_(T t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == t),
                        concepts::convertible_to<bool>(t != t)
                    ));

                template<typename T, typename U,
                    enable_if_t<std::is_same<T, U>::value> = 0>
                auto requires_(T t, U u) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == u),
                        concepts::convertible_to<bool>(u == t),
                        concepts::convertible_to<bool>(t != u),
                        concepts::convertible_to<bool>(u != t)
                    ));

                // Cross-type equality comparison from N3351:
                // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3351.pdf
                template<typename T, typename U,
                    enable_if_t<!std::is_same<T, U>::value> = 0,
                    typename C = common_type_t<T, U>>
                auto requires_(T t, U u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<EqualityComparable, T>(),
                        concepts::model_of<EqualityComparable, U>(),
                        concepts::model_of<Common, T, U>(),
                        concepts::model_of<EqualityComparable, C>(),
                        concepts::convertible_to<bool>(t == u),
                        concepts::convertible_to<bool>(u == t),
                        concepts::convertible_to<bool>(t != u),
                        concepts::convertible_to<bool>(u != t)
                    ));
            };

            struct WeaklyOrdered
            {
                template<typename T>
                auto requires_(T t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t < t),
                        concepts::convertible_to<bool>(t > t),
                        concepts::convertible_to<bool>(t <= t),
                        concepts::convertible_to<bool>(t >= t)
                    ));

                template<typename T, typename U, typename C = common_type_t<T, U>>
                auto requires_(T t, U u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<WeaklyOrdered, T>(),
                        concepts::model_of<WeaklyOrdered, U>(),
                        concepts::model_of<Common, T, U>(),
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
                void requires_(T);

                template<typename T, typename U>
                auto requires_(T t, U u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<TotallyOrdered>(val<T>()),
                        concepts::model_of<TotallyOrdered>(val<U>())
                    ));
            };

            struct Allocatable
            {
                template<typename T>
                auto requires_(T t) -> decltype(
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
                auto requires_(T t) -> decltype(
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
                auto requires_(T t) -> decltype(
                    concepts::valid_expr(
                        (swap(val<T>(), val<T>()), 42)
                    ));

                template<typename T, typename U>
                auto requires_(T t, U u) -> decltype(
                    concepts::valid_expr(
                        (swap(val<T>(), val<U>()), 42),
                        (swap(val<U>(), val<T>()), 42)
                    ));
            };

            struct Function
              : refines<Destructible(_1), CopyConstructible(_1)>
            {
                template<typename Fun, typename ...Args>
                using result_t = decltype(val<Fun>()(val<Args>()...));

                template<typename Fun, typename ...Args,
                    typename UnRefFun = meta::eval<std::remove_reference<Fun>>,
                    typename UnCvRefFun = meta::eval<std::remove_cv<UnRefFun>>>
                auto requires_(Fun fun, Args... args) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<UnRefFun *>(&fun),
                        concepts::has_type<UnCvRefFun *>(new UnCvRefFun(fun)),
                        (delete new UnCvRefFun(fun), 42),
                        (static_cast<void>(val<Fun>()(val<Args>()...)), 42)
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
                auto requires_(Fun fun, Args... args) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(val<Fun>()(val<Args>()...))
                    ));
            };

            struct Relation
              : refines<RegularFunction>
            {
                template<typename Fun, typename T>
                auto requires_(Fun fun, T t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate>(val<Fun>(), val<T>(), val<T>())
                    ));

                template<typename Fun, typename T, typename U,
                    enable_if_t<std::is_same<T, U>::value> = 0>
                auto requires_(Fun fun, T t, T u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate>(val<Fun>(), val<T>(), val<U>())
                    ));

                template<typename Fun, typename T, typename U,
                    enable_if_t<!std::is_same<T, U>::value> = 0,
                    typename C = common_type_t<T, U>>
                auto requires_(Fun fun, T t, U u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Relation, Fun, T, T>(),
                        concepts::model_of<Relation, Fun, U, U>(),
                        concepts::model_of<Common, T, U>(),
                        concepts::model_of<Relation, Fun, C, C>(),
                        concepts::model_of<Predicate, Fun, T, U>(),
                        concepts::model_of<Predicate, Fun, U, T>()
                    ));
            };

            struct Transform
              : refines<RegularFunction>
            {
                template<typename F, typename T>
                auto requires_(F && f, T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<T>(val<F>()(val<T>()))
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
        using UnsignedIntegral = concepts::models<concepts::UnsignedIntegral, T>;

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
        using Function = concepts::models<concepts::Function, Fun, Args...>;

        template<typename Fun, typename ...Args>
        using RegularFunction = concepts::models<concepts::RegularFunction, Fun, Args...>;

        template<typename Fun, typename ...Args>
        using Predicate = concepts::models<concepts::Predicate, Fun, Args...>;

        template<typename Fun, typename T, typename U = T>
        using Relation = concepts::models<concepts::Relation, Fun, T, U>;

        template<typename F, typename T>
        using Transform = concepts::models<concepts::Transform, F, T>;

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
            struct difference_type<T, void_t<typename T::difference_type>>
            {
                using type = typename T::difference_type;
            };

            template<typename T>
            struct difference_type<T, typename std::enable_if<std::is_integral<T>::value>::type>
            {
                using type = decltype(std::declval<T>() - std::declval<T>());
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
            struct value_type<T, void_t<typename T::value_type>>
              : std::enable_if<!std::is_void<typename T::value_type>::value, typename T::value_type>
            {
                // The use of enable_if is to accomodate output iterators that are
                // allowed to use void as their value type. We want treat output
                // iterators as non-Readable. value_type<OutIt> should be
                // SFINAE-friendly.
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
          : detail::difference_type<uncvref_t<T>>
        {};

        template<typename T>
        struct size_type
          : std::make_unsigned<meta::eval<difference_type<T>>>
        {};

        template<typename T>
        struct value_type
          : detail::value_type<uncvref_t<T>>
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

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
