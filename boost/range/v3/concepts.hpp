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

#ifndef BOOST_RANGE_V3_CONCEPTS_HPP
#define BOOST_RANGE_V3_CONCEPTS_HPP

#include <utility>
#include <type_traits>
#include <boost/range/v3/begin_end.hpp>

namespace boost
{
    namespace range
    {
        inline namespace v3
        {
            namespace concepts
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

                    constexpr struct and_t
                    {
                    private:
                        static constexpr bool impl()
                        {
                            return true;
                        }
                        template<typename...Tail>
                        static constexpr bool impl(bool B, Tail...tail)
                        {
                            return B && and_t::impl(tail...);
                        }
                    public:
                        template<typename ...Bools>
                        std::integral_constant<bool, and_t::impl(Bools::value...)>
                        operator()(Bools...) const
                        {
                            return {};
                        }
                    } and_ {};

                    constexpr struct is_true_t
                    {
                        template<typename Bool>
                        auto operator()(Bool) const ->
                            typename std::enable_if<Bool::value, int>::type;
                    } is_true {};

                    template<typename Concept, typename T>
                    struct models_impl;
                }

                using detail::void_;
                using detail::is_void;
                using detail::valid_expr;
                using detail::same_type;
                using detail::is_true;

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
                  : Concepts...
                {};

                template<typename Concept, typename T>
                constexpr bool models()
                {
                    return decltype(detail::models_impl<Concept, T>{}(std::declval<T>()))::value;
                };

                template<typename Concept, typename T>
                auto model_of(T &&) ->
                    typename std::enable_if<concepts::models<Concept, T>(), int>::type;

                namespace detail
                {
                    template<typename Concept, typename T>
                    struct models_impl
                    {
                    private:
                        using false_t = std::false_type(*)(T &&);
                        static std::false_type false_(T &&)
                        {
                            return {};
                        }

                        std::true_type test_refines(void *)
                        {
                            return {};
                        }

                        template<typename ...Bases>
                        auto test_refines(refines<Bases...> *) -> decltype(
                            detail::and_(
                                std::integral_constant<bool, concepts::models<Bases, T>()>{}...
                            ))
                        {
                            return {};
                        }

                    public:
                        operator false_t () const
                        {
                            return &false_;
                        }

                        template<typename C = Concept>
                        auto operator()(T && t) -> decltype(
                            detail::and_(
                                C{}.requires(static_cast<T &&>(t)),
                                models_impl::test_refines((C*)nullptr)
                            ))
                        {
                            return {};
                        }
                    };
                }

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

                struct Assignable
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            t = t
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

                struct DefaultConstructible
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            T{}
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

                struct InputIterator
                  : refines<DefaultConstructible, CopyConstructible, Assignable, Comparable>
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            *t,
                            t++,
                            concepts::has_type<T &>( ++t )
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
                            t += (t-t),
                            t = t + (t-t),
                            t = (t-t) + t,
                            t -= (t-t),
                            t = t - (t-t),
                            concepts::same_type(*t, t[t-t])
                        ));
                };

                struct InputRange : refines<CopyConstructible>
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            concepts::same_type(range::begin(t), range::end(t)),
                            concepts::same_type(range::cbegin(t), range::cend(t)),
                            concepts::model_of<InputIterator>(range::begin(t)),
                            concepts::model_of<InputIterator>(range::cbegin(t)),
                            concepts::convertible_to<decltype(range::cbegin(t))>(range::begin(t))
                        ));
                };

                struct ForwardRange : refines<InputRange>
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<ForwardIterator>(range::begin(t)),
                            concepts::model_of<ForwardIterator>(range::cbegin(t))
                        ));
                };

                struct BidirectionalRange : refines<ForwardRange>
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<BidirectionalIterator>(range::begin(t)),
                            concepts::model_of<BidirectionalIterator>(range::cbegin(t))
                        ));
                };

                struct RandomAccessRange : refines<BidirectionalRange>
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            concepts::model_of<RandomAccessIterator>(range::begin(t)),
                            concepts::model_of<RandomAccessIterator>(range::cbegin(t))
                        ));
                };
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
            constexpr bool Assignable()
            {
                return concepts::models<concepts::Assignable, T>();
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
            constexpr bool Comparable()
            {
                return concepts::models<concepts::Comparable, T>();
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

            template<typename T>
            constexpr bool InputRange()
            {
                return concepts::models<concepts::InputRange, T>();
            }

            template<typename T>
            constexpr bool ForwardRange()
            {
                return concepts::models<concepts::ForwardRange, T>();
            }

            template<typename T>
            constexpr bool BidirectionalRange()
            {
                return concepts::models<concepts::BidirectionalRange, T>();
            }

            template<typename T>
            constexpr bool RandomAccessRange()
            {
                return concepts::models<concepts::RandomAccessRange, T>();
            }
        }
    }
}

#endif
