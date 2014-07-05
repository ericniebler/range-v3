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
                using type = typelist<>;
            };

            template<typename Concept>
            struct base_concepts_of<Concept, always_t<void, typename Concept::base_concepts_t>>
            {
                using type = typename Concept::base_concepts_t;
            };

            template<typename Concept>
            using base_concepts_of_t = meta_apply<base_concepts_of, Concept>;

            template<typename...Bools>
            struct lazy_and
            {
                static constexpr bool value{true};
            };

            template<typename Bool, typename...Bools>
            struct lazy_and<Bool, Bools...>
            {
                static constexpr bool value{Bool::value && lazy_and<Bools...>::value};
            };

            template<typename...Ts>
            auto models_(any, Ts &&...) ->
                std::false_type;

            template<typename Concept, typename...Ts>
            auto models_(Concept *c, Ts &&...ts) ->
                always_t<
                    typelist_expand_t<
                        lazy_and,
                        typelist_transform_t<
                            base_concepts_of_t<Concept>,
                            meta_bind_back<concepts::models, Ts...>::template apply>>,
                    decltype(c->requires_(std::forward<Ts>(ts)...))>;
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

                using base_concepts_t = typelist<Concepts...>;

                template<typename...Ts>
                void requires_(Ts &&...);
            };

            ////////////////////////////////////////////////////////////////////////////////////////////
            // models
            template<typename Concept, typename...Ts>
            struct models
              : std::integral_constant<bool,
                    decltype(detail::models_(nullptr_v<Concept>(), std::declval<Ts>()...))::value>
            {};

            template<typename Concept, typename...Args, typename...Ts>
            struct models<Concept(Args...), Ts...>
              : models<Concept, typelist_element_t<Args::value, typelist<Ts...> >...>
            {};

            ////////////////////////////////////////////////////////////////////////////////////////////
            // model_of
            template<typename Concept, typename ...Ts>
            auto model_of(Ts &&...) ->
                enable_if_t<concepts::models<Concept, Ts...>::value>;

            ////////////////////////////////////////////////////////////////////////////////////////////
            // most_refined
            // Find the first concept in a list of concepts that is modeled by the Args
            template<typename Concepts, typename...Ts>
            struct most_refined
              : typelist_front<
                    typelist_find_if_t<
                        meta_bind_back<models, Ts...>::template apply,
                        Concepts>>
            {
                constexpr operator meta_eval<most_refined> *() const
                {
                    return nullptr;
                }
                constexpr meta_eval<most_refined> *operator()() const
                {
                    return nullptr;
                }
            };

            template<typename Concepts, typename...Ts>
            using most_refined_t = meta_apply<most_refined, Concepts, Ts...>;

            ////////////////////////////////////////////////////////////////////////////////////////////
            // Core language concepts
            ////////////////////////////////////////////////////////////////////////////////////////////

            struct Same
            {
                template<typename T, typename U>
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type((T &&) t, (U &&) u)
                    ));
            };

            struct Convertible
            {
                template<typename T, typename U>
                auto requires_(T &&t, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<U>((T &&) t)
                    ));
            };

            struct Derived
            {
                template<typename T, typename U>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_base_of<U, T>{})
                    ));
            };

            struct Common
            {
                template<typename T, typename U>
                using common_t = common_type_t<T, U>;

                template<typename T>
                auto requires_(T &&, T &&) -> void;

                template<typename T, typename U, typename C = common_t<T, U>>
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<C>((T &&) t),
                        concepts::convertible_to<C>((U &&) u)
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

            struct Destructible
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_destructible<T>{})
                    ));
            };

            struct Constructible
              : refines<Destructible(_1)>
            {
                template<typename T, typename ...Us>
                auto requires_(T &&, Us &&...) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_constructible<T, Us...>{})
                    ));
            };

            struct DefaultConstructible
              : refines<Destructible>
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_default_constructible<T>{})
                    ));
            };

            struct MoveConstructible
              : refines<Destructible>
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_move_constructible<T>{})
                    ));
            };

            struct CopyConstructible
              : refines<MoveConstructible>
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_copy_constructible<T>{})
                    ));
            };

            struct Assignable
            {
                template<typename T, typename U>
                auto requires_(T &&, U &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_assignable<T, U>{})
                    ));
            };

            struct MoveAssignable
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_move_assignable<T>{})
                    ));
            };

            struct CopyAssignable
              : refines<MoveAssignable>
            {
                template<typename T>
                auto requires_(T &&) -> decltype(
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
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == t),
                        concepts::convertible_to<bool>(t != t)
                    ));

                template<typename T>
                auto requires_(T && t, T && u) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == u),
                        concepts::convertible_to<bool>(u == t),
                        concepts::convertible_to<bool>(t != u),
                        concepts::convertible_to<bool>(u != t)
                    ));

                // Cross-type equality comparison from N3351:
                // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3351.pdf
                template<typename T, typename U, typename C = common_type_t<T, U>>
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<EqualityComparable>((T &&) t),
                        concepts::model_of<EqualityComparable>((U &&) u),
                        concepts::model_of<Common>((T &&) t, (U &&) u),
                        concepts::model_of<EqualityComparable>(static_cast<C>((T &&) t)),
                        concepts::convertible_to<bool>(t == u),
                        concepts::convertible_to<bool>(u == t),
                        concepts::convertible_to<bool>(t != u),
                        concepts::convertible_to<bool>(u != t)
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

                template<typename T, typename U, typename C = common_type_t<T, U>>
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<WeaklyOrdered>((T &&) t),
                        concepts::model_of<WeaklyOrdered>((U &&) u),
                        concepts::model_of<Common>(t, u),
                        concepts::model_of<WeaklyOrdered>(static_cast<C>((T &&) t)),
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
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<TotallyOrdered>((T &&) t),
                        concepts::model_of<TotallyOrdered>((U &&) u)
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
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        swap((T&&)t, (T&&)t)
                    ));

                template<typename T, typename U>
                auto requires_(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        swap((T&&)t, (U&&)u),
                        swap((U&&)u, (T&&)t)
                    ));
            };

            struct Function
              : refines<Destructible(_1), CopyConstructible(_1)>
            {
                template<typename Fun, typename ...Args>
                using result_t = decltype(std::declval<Fun>()(std::declval<Args>()...));

                template<typename Fun, typename ...Args,
                    typename UnRefFun = meta_apply<std::remove_reference, Fun>,
                    typename UnCvRefFun = meta_apply<std::remove_cv, UnRefFun>>
                auto requires_(Fun && fun, Args &&... args) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<UnRefFun *>(&fun),
                        concepts::has_type<UnCvRefFun *>(new UnCvRefFun(fun)),
                        (delete new UnCvRefFun(fun), 42),
                        (static_cast<void>(std::forward<Fun>(fun)(
                            std::forward<Args>(args)...)), 42)
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
                auto requires_(Fun && fun, Args &&... args) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(
                            std::forward<Fun>(fun)(std::forward<Args>(args)...))
                    ));
            };

            struct Relation
              : refines<RegularFunction>
            {
                template<typename Fun, typename T>
                auto requires_(Fun && fun, T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate>((Fun &&) fun, (T &&) t, (T &&) t)
                    ));

                template<typename Fun, typename T>
                auto requires_(Fun && fun, T && t, T && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Predicate>((Fun &&) fun, (T &&) t, (T &&) u)
                    ));

                template<typename Fun, typename T, typename U,
                    typename C = common_type_t<T, U>>
                auto requires_(Fun && fun, T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Relation>((Fun &&) fun, (T &&) t, (T &&) t),
                        concepts::model_of<Relation>((Fun &&) fun, (U &&) u, (U &&) u),
                        concepts::model_of<Common>(t, u),
                        concepts::model_of<Relation>((Fun &&) fun, static_cast<C>((T &&) t), static_cast<C>((U &&) u)),
                        concepts::model_of<Predicate>((Fun &&) fun, (T &&) t, (U &&) u),
                        concepts::model_of<Predicate>((Fun &&) fun, (U &&) u, (T &&) t)
                    ));
            };

            struct Transform
              : refines<RegularFunction>
            {
                template<typename F, typename T>
                auto requires_(F && f, T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<T>(((F&&)f)((T&&) t))
                    ));
            };

            struct Addable
            {
                template<typename T, typename U>
                using result_t = decltype(std::declval<T>() + std::declval<U>());

                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        t + t
                    ));

                template<typename T, typename U>
                auto requires_(T && t, U && u) -> decltype(
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
        using Function = concepts::models<concepts::Function, Fun, Args...>;

        template<typename Fun, typename ...Args>
        using RegularFunction = concepts::models<concepts::RegularFunction, Fun, Args...>;

        template<typename Fun, typename ...Args>
        using Predicate = concepts::models<concepts::Predicate, Fun, Args...>;

        template<typename Fun, typename T, typename U = T>
        using Relation = concepts::models<concepts::Relation, Fun, T, U>;

        template<typename F, typename T>
        using Transform = concepts::models<concepts::Transform, F, T>;

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
        struct size_type
          : std::make_unsigned<meta_apply<difference_type, T>>
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
