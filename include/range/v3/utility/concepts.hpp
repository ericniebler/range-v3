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
                std::true_type operator()(T &&...) const;
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
            struct models_impl;
        }

        namespace concepts
        {
            using detail::void_;
            using detail::is_void;
            using detail::valid_expr;
            using detail::same_type;
            using detail::is_true;
            using detail::is_false;

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
              : Concepts...
            {};

            template<typename Concept, typename ...Ts>
            constexpr bool models()
            {
                return decltype(detail::models_impl<Concept, Ts...>{}(std::declval<Ts>()...))::value;
            };

            template<typename Concept, typename ...Ts>
            auto model_of(Ts &&...) ->
                typename std::enable_if<concepts::models<Concept, Ts...>(), int>::type
            {
                return 0;
            }
        }

        namespace detail
        {
            template<typename Concept, typename ...Ts>
            struct models_impl
            {
            private:
                using false_t = std::false_type(*)(Ts &&...);
                static std::false_type false_(Ts &&...)
                {
                    return {};
                }

                std::true_type test_refines(void *)
                {
                    return {};
                }

                template<typename ...Bases>
                auto test_refines(concepts::refines<Bases...> *) ->
                    detail::and_<concepts::models<Bases, Ts...>()...>
                {
                    return {};
                }

            public:
                operator false_t () const
                {
                    return &false_;
                }

                template<typename C = Concept>
                auto operator()(Ts &&... ts) ->
                    std::integral_constant<bool,
                        decltype(C{}.requires(std::forward<Ts>(ts)...))::value &&
                        decltype(models_impl::test_refines((C*)nullptr))::value
                    >
                {
                    return {};
                }
            };
        }

        namespace concepts
        {
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

            struct UnaryPredicate
              : refines<Predicate>
            {
                template<typename Fun, typename Arg>
                auto requires(Fun && fun, Arg && arg) -> decltype(
                    concepts::valid_expr(
                        std::forward<Fun>(fun)(std::forward<Arg>(arg))
                    ));
            };

            struct BinaryPredicate
              : refines<Predicate>
            {
                template<typename Fun, typename Arg0, typename Arg1>
                auto requires(Fun && fun, Arg0 && arg0, Arg1 && arg1) -> decltype(
                    concepts::valid_expr(
                        std::forward<Fun>(fun)(std::forward<Arg0>(arg0),
                                               std::forward<Arg1>(arg1))
                    ));
            };

            struct Iterator
              : refines<CopyConstructible, CopyAssignable, Destructible>
            {
                template<typename T>
                auto requires(T && t) -> decltype(
                    concepts::valid_expr(
                        *t,
                        concepts::has_type<T &>(++t)
                    ));
            };

            // BUGBUG Fix multi-argument concepts
            struct OutputIterator
              //: refines<Iterator>
            {
                template<typename T, typename O>
                auto requires(T && t, O && o) -> decltype(
                    concepts::valid_expr(
                        t++,
                        *t = o,
                        *t++ = o,
                        concepts::model_of<Iterator>(std::forward<T>(t))
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
              : refines<BidirectionalIterator, Orderable>
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
                        concepts::same_type(*t, t[t-t])
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
    }
}

#define CONCEPT_REQUIRES(...) typename = typename std::enable_if<(__VA_ARGS__)>::type
#define CONCEPT_ASSERT(...) static_assert((__VA_ARGS__), "Concept check failed");

#endif // RANGES_V3_UTILITY_CONCEPTS_HPP
