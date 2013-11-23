// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_UTILITY_CONCEPTS_HPP
#define RANGES_V3_UTILITY_CONCEPTS_HPP

#include <utility>
#include <type_traits>

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
                true_ operator()(T &&...) const;
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
            auto convertible(T && t, U && u) ->
                decltype(true ? static_cast<T &&>(t) : static_cast<U &&>(u));

            template<typename T, typename U>
            auto has_type(U &&) ->
                typename std::enable_if<std::is_same<T,U>::value, int>::type;

            template<typename ...Concepts>
            struct refines
              : detail::base_concept<Concepts>::type...
            {};

            template<typename Concept, typename ...Ts>
            constexpr bool models()
            {
                return decltype(detail::models_<Concept, Ts...>{}())::value;
            };

            template<typename Concept, typename ...Ts>
            auto model_of(Ts &&...) ->
                typename std::enable_if<concepts::models<Concept, Ts...>(), int>::type;
        }

        namespace detail
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // always
            template<typename A, typename...Rest>
            struct always
            {
                using type = A;
            };

            template<typename A, typename...Rest>
            using always_t = typename always<A, Rest...>::type;

            ////////////////////////////////////////////////////////////////////////////////////
            // always
            template<typename T, typename Enable = void>
            struct iterator_traits_impl
            {};

            template<typename T>
            struct iterator_traits_impl<
                T,
                detail::always_t<
                    void,
                    typename T::iterator_category,
                    typename T::value_type,
                    typename T::difference_type,
                    typename T::reference,
                    typename T::pointer>>
            {
                using iterator_category = typename T::iterator_category;
                using value_type = typename T::value_type;
                using difference_type = typename T::difference_type;
                using reference = typename T::reference;
                using pointer = typename T::pointer;
            };

            template<typename T>
            struct iterator_traits_impl<T *>
            {
                using iterator_category = std::random_access_iterator_tag;
                using value_type = typename std::remove_const<T>::type;
                using difference_type = std::ptrdiff_t;
                using reference = T &;
                using pointer = T *;
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // list
            template<typename...>
            struct list;

            ////////////////////////////////////////////////////////////////////////////////////
            // concat
            template<typename List0, typename List1>
            struct concat;

            template<typename ...List1, typename ...List2>
            struct concat<list<List1...>, list<List2...>>
            {
                using type = list<List1..., List2...>;
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // pop_front
            template<typename List>
            struct pop_front;

            template<typename Head, typename ...List>
            struct pop_front<list<Head, List...>>
            {
                using type = list<List...>;
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // front
            template<typename List>
            struct front;

            template<typename Head, typename ...List>
            struct front<list<Head, List...>>
            {
                using type = Head;
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // is_empty
            template<typename List>
            struct is_empty
              : false_
            {};

            template<>
            struct is_empty<list<>>
              : true_
            {};

            ////////////////////////////////////////////////////////////////////////////////////
            // list_of
            // Generate lists<_,_,_,..._> with N arguments in O(log N)
            template<int N, typename T, typename List = list<>>
            struct list_of
              : concat<typename list_of<N / 2, T, List>::type,
                       typename list_of<N - N / 2, T, List>::type>
            {};

            template<typename T, typename List>
            struct list_of<0, T, List>
            {
                using type = List;
            };

            template<typename T>
            struct list_of<1, T, list<>>
            {
                using type = list<T>;
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // get_nth_type
            template<typename Ignored>
            struct get_nth_impl;

            template<typename ...Ignored>
            struct get_nth_impl<list<Ignored...>>
            {
                template<typename T, typename ...Us>
                static T eval(always_t<void *, Ignored>..., T *, Us *...);
            };

            ////////////////////////////////////////////////////////////////////////////////////
            // get_nth
            template<int N, typename ...Ts>
            using get_nth =
               typename decltype(
                    get_nth_impl<typename list_of<N, decltype(nullptr)>::type>::eval(
                        static_cast<identity<Ts>*>(nullptr)...))::type;

            ////////////////////////////////////////////////////////////////////////////////////
            // models_
            template<typename Concept, typename ...Ts>
            struct models_
            {
            private:
                true_ test_refines(void *); // No refinements, ok
                template<typename ...Bases>
                auto test_refines(concepts::refines<Bases...> *) ->
                    detail::and_<concepts::models<Bases, Ts...>()...>;
            public:
                false_ operator()() const;
                template<typename C = Concept>
                auto operator()() ->
                    bool_<decltype(C{}.requires(std::declval<Ts>()...))::value &&
                          decltype(models_::test_refines((C *)nullptr))::value>;
            };

            template<typename Concept, typename...Args, typename ...Ts>
            struct models_<Concept(Args...), Ts...>
              : models_<Concept, get_nth<Args::value, Ts...>...>
            {};

            list<> base_concepts_impl_(void *);

            template<typename...BaseConcepts>
            list<BaseConcepts...> base_concepts_impl_(concepts::refines<BaseConcepts...> *);

            template<typename Concept>
            using base_concepts_t = decltype(detail::base_concepts_impl_((Concept *)nullptr));

            struct not_a_concept
            {};

            template<typename...Ts>
            struct most_refined_impl_
            {
                static not_a_concept invoke(list<> *);

                template<typename Head, typename...Tail, typename Impl = most_refined_impl_>
                static auto invoke(list<Head, Tail...> *) ->
                    typename std::conditional<
                        concepts::models<Head, Ts...>(),
                        Head,
                        decltype(Impl::invoke(
                            (typename concat<list<Tail...>, base_concepts_t<Head>>::type *)nullptr))
                    >::type;
            };
        }

        namespace concepts
        {
            using detail::not_a_concept;

            template<typename Concept, typename...Ts>
            using most_refined_t =
                decltype(detail::most_refined_impl_<Ts...>::invoke((detail::list<Concept> *)nullptr));

            template<typename Concept, typename...Ts>
            struct most_refined
            {
                using type = most_refined_t<Concept, Ts...>;
            };

            struct True
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(t)
                    ));
            };

            struct False
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::is_false(t)
                    ));
            };

            struct SameType
            {
                template<typename T, typename U>
                auto requires(T && t, U && u) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(t, u)
                    ));
            };

            struct Integral
            {
                template<typename T>
                auto requires(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_integral<T>{})
                    ));
            };

            struct SignedIntegral
              : refines<Integral>
            {
                template<typename T>
                auto requires(T &&) -> decltype(
                    concepts::valid_expr(
                        concepts::is_true(std::is_signed<T>{})
                    ));
            };

            struct Destructible
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        (t.~T(), 42)
                    ));
            };

            struct DefaultConstructible
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        T{}
                    ));
            };

            struct CopyConstructible
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        T(t)
                    ));
            };

            struct CopyAssignable
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t = t
                    ));
            };

            struct Comparable
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t == t),
                        concepts::convertible_to<bool>(t != t)
                    ));
            };

            struct Orderable
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(t < t),
                        concepts::convertible_to<bool>(t > t),
                        concepts::convertible_to<bool>(t <= t),
                        concepts::convertible_to<bool>(t >= t)
                    ));
            };

            struct Callable
            {
                template<typename Fun, typename ...Args>
                using result_t = decltype(std::declval<Fun>()(std::declval<Args>()...));

                template<typename Fun, typename ...Args>
                auto requires(Fun && fun, Args &&... args) -> decltype(
                    concepts::valid_expr(
                        (static_cast<void>(detail::forward<Fun>(fun)(
                            detail::forward<Args>(args)...)), 42)
                    ));
            };

            struct Predicate
              : refines<Callable>
            {
                template<typename Fun, typename ...Args>
                auto requires(Fun && fun, Args &&... args) -> decltype(
                    concepts::valid_expr(
                        concepts::convertible_to<bool>(
                            detail::forward<Fun>(fun)(detail::forward<Args>(args)...))
                    ));
            };

            struct UnaryPredicate
              : refines<Predicate>
            {
                template<typename Fun, typename Arg>
                auto requires(Fun && fun, Arg && arg) -> decltype(
                    concepts::valid_expr(
                        detail::forward<Fun>(fun)(detail::forward<Arg>(arg))
                    ));
            };

            struct BinaryPredicate
              : refines<Predicate>
            {
                template<typename Fun, typename Arg0, typename Arg1>
                auto requires(Fun && fun, Arg0 && arg0, Arg1 && arg1) -> decltype(
                    concepts::valid_expr(
                        detail::forward<Fun>(fun)(detail::forward<Arg0>(arg0),
                                                  detail::forward<Arg1>(arg1))
                    ));
            };

            struct Iterator
              : refines<CopyConstructible, CopyAssignable, Destructible>
            {
                // Users should specialize this to hook the traits mechanism.
                template<typename T>
                struct traits
                  : detail::iterator_traits_impl<T>
                {};

                // Associated types
                template<typename T>
                using category_t = typename traits<T>::iterator_category;

                template<typename T>
                using value_t = typename traits<T>::value_type;

                template<typename T>
                using difference_t = typename traits<T>::difference_type;

                template<typename T>
                using reference_t = typename traits<T>::reference;

                template<typename T>
                using pointer_t = typename traits<T>::pointer;

                // Valid expressions
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        *t,
                        concepts::has_type<T &>(++t)
                    ));
            };

            struct OutputIterator
              : refines<Iterator(_1)> // OutputIterator<T,U> refines Iterator<T>
            {
                template<typename T, typename O>
                auto requires(T && t, O && o) -> decltype(
                    concepts::valid_expr(
                        t++,
                        *t = o,
                        *t++ = o
                    ));
            };

            struct InputIterator
              : refines<Iterator, Comparable>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        t++,
                        concepts::convertible(*t, *t++)
                    ));
            };

            struct ForwardIterator
              : refines<InputIterator>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::same_type(*t, *t++)
                    ));
            };

            struct BidirectionalIterator
              : refines<ForwardIterator>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::has_type<T &>( --t ),
                        concepts::same_type(*t, *t--)
                    ));
            };

            struct RandomAccessIterator
              : refines<BidirectionalIterator>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<SignedIntegral>(t-t),
                        t = t + (t-t),
                        t = (t-t) + t,
                        t = t - (t-t),
                        t += (t-t),
                        t -= (t-t),
                        concepts::same_type(*t, t[t-t]),
                        concepts::model_of<Orderable>(t)
                    ));
            };
        }

        template<typename T>
        constexpr bool True()
        {
            return concepts::models<concepts::True, T>();
        }

        template<typename T>
        constexpr bool False()
        {
            return concepts::models<concepts::False, T>();
        }

        template<typename T, typename U>
        constexpr bool SameType()
        {
            return concepts::models<concepts::SameType, T, U>();
        }

        template<typename T>
        constexpr bool Integral()
        {
            return concepts::models<concepts::Integral, T>();
        }

        template<typename T>
        constexpr bool SignedIntegral()
        {
            return concepts::models<concepts::SignedIntegral, T>();
        }

        template<typename T>
        constexpr bool CopyAssignable()
        {
            return concepts::models<concepts::CopyAssignable, T>();
        }

        template<typename T>
        constexpr bool CopyConstructible()
        {
            return concepts::models<concepts::CopyConstructible, T>();
        }

        template<typename T>
        constexpr bool DefaultConstructible()
        {
            return concepts::models<concepts::DefaultConstructible, T>();
        }

        template<typename T>
        constexpr bool Destructible()
        {
            return concepts::models<concepts::Destructible, T>();
        }

        template<typename T>
        constexpr bool Comparable()
        {
            return concepts::models<concepts::Comparable, T>();
        }

        template<typename T>
        constexpr bool Orderable()
        {
            return concepts::models<concepts::Orderable, T>();
        }

        template<typename Fun, typename ...Args>
        constexpr bool Callable()
        {
            return concepts::models<concepts::Callable, Fun, Args...>();
        }

        template<typename Fun, typename ...Args>
        constexpr bool Predicate()
        {
            return concepts::models<concepts::Predicate, Fun, Args...>();
        }

        template<typename Fun, typename Arg>
        constexpr bool UnaryPredicate()
        {
            return concepts::models<concepts::UnaryPredicate, Fun, Arg>();
        }

        template<typename Fun, typename Arg0, typename Arg1>
        constexpr bool BinaryPredicate()
        {
            return concepts::models<concepts::BinaryPredicate, Fun, Arg0, Arg1>();
        }

        template<typename T>
        constexpr bool Iterator()
        {
            return concepts::models<concepts::Iterator, T>();
        }

        template<typename T, typename O>
        constexpr bool OutputIterator()
        {
            return concepts::models<concepts::OutputIterator, T, O>();
        }

        template<typename T>
        constexpr bool InputIterator()
        {
            return concepts::models<concepts::InputIterator, T>();
        }

        template<typename T>
        constexpr bool ForwardIterator()
        {
            return concepts::models<concepts::ForwardIterator, T>();
        }

        template<typename T>
        constexpr bool BidirectionalIterator()
        {
            return concepts::models<concepts::BidirectionalIterator, T>();
        }

        template<typename T>
        constexpr bool RandomAccessIterator()
        {
            return concepts::models<concepts::RandomAccessIterator, T>();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator_concept
        template<typename T>
        using iterator_concept_t =
            concepts::most_refined_t<concepts::RandomAccessIterator, T>;

        template<typename T>
        struct iterator_concept
        {
            using type = iterator_concept_t<T>;
        };
    }
}

#define CONCEPT_REQUIRES(...) typename = typename std::enable_if<(__VA_ARGS__)>::type
#define CONCEPT_ASSERT(...) static_assert((__VA_ARGS__), "Concept check failed");

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
