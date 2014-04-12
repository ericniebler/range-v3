//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_LOWER_BOUND_HPP
#define RANGES_V3_ALGORITHM_LOWER_BOUND_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename V2, typename R = ordered_less, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V> >
        constexpr bool BinarySearchable()
        {
            return ForwardIterator<I>()            &&
                   Invokable<P, V>()               &&
                   InvokableRelation<R, X, V2>();
        }

        struct lower_bound_n_fn
        {
            template<typename I, typename V2, typename R = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(
                    BinarySearchable<I, V2, R, P>()
                )>
            I
            operator()(I begin, iterator_difference_t<I> d, V2 const &val, R pred = R{},
                P proj = P{}) const
            {
                RANGES_ASSERT(0 <= d);
                auto &&ipred = invokable(pred);
                auto &&iproj = invokable(proj);
                while(0 != d)
                {
                    auto half = d / 2;
                    auto middle = next(begin, half);
                    if(ipred(iproj(*middle), val))
                    {
                        begin = std::move(++middle);
                        d -= half + 1;
                    }
                    else
                        d = half;
                }
                return begin;
            }

            /// \overload
            template<typename Rng, typename V2, typename R = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng>()                 &&
                    BinarySearchable<I, V2, R, P>()
                )>
            I
            operator()(Rng &rng, iterator_difference_t<I> d, V2 const &val, R pred = R{},
                P proj = P{}) const
            {
                static_assert(!is_infinite<Rng>::val,
                    "Trying to binary search an infinite range");
                RANGES_ASSERT(0 <= d);
                RANGES_ASSERT(d <= distance(rng));
                return (*this)(begin(rng), d, std::move(pred), std::move(proj));
            }

            /// \overload
            template<typename V, typename V2, typename R = ordered_less, typename P = ident,
                typename I = V const *,
                CONCEPT_REQUIRES_(
                    BinarySearchable<I, V2, R, P>()
                )>
            I
            operator()(std::initializer_list<V> rng, std::ptrdiff_t d, V2 const &val, R pred = R{},
                P proj = P{}) const
            {
                RANGES_ASSERT(0 <= d);
                RANGES_ASSERT((std::size_t)d <= rng.size());
                return (*this)(rng.begin(), d, std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR lower_bound_n_fn lower_bound_n{};

        struct lower_bound_fn
        {
            template<typename I, typename S, typename V2, typename R = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(
                    Sentinel<S, I>()                &&
                    BinarySearchable<I, V2, R, P>()
                )>
            I
            operator()(I begin, S end, V2 const &val, R pred = R{}, P proj = P{}) const
            {
                return lower_bound_n(begin, distance(begin, end), val, std::move(pred),
                    std::move(proj));
            }

            /// \overload
            template<typename Rng, typename V2, typename R = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(
                    Iterable<Rng>()                 &&
                    BinarySearchable<I, V2, R, P>()
                )>
            I
            operator()(Rng &rng, V2 const &val, R pred = R{}, P proj = P{}) const
            {
                static_assert(!is_infinite<Rng>::value,
                    "Trying to binary search an infinite range");
                return lower_bound_n(begin(rng), distance(rng), val, std::move(pred),
                    std::move(proj));
            }

            /// \overload
            template<typename V, typename V2, typename R = ordered_less, typename P = ident,
                typename I = V const *,
                CONCEPT_REQUIRES_(
                    BinarySearchable<I, V2, R, P>()
                )>
            I
            operator()(std::initializer_list<V> rng, V2 const &val, R pred = R{}, P proj = P{}) const
            {
                return lower_bound_n(rng.begin(), (std::ptrdiff_t)rng.size(), val, std::move(pred),
                    std::move(proj));
            }
        };

        RANGES_CONSTEXPR lower_bound_fn lower_bound{};

    } // namespace v3
} // namespace ranges

#endif // include guard
