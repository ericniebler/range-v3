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
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            struct dummy_count
            {
                template<typename T>
                void operator+=(T const &)
                {}
            };
        }

        struct lower_bound_n_fn
        {
        private:
            template<typename ForwardIterator, typename Value, typename BinaryPredicate,
                typename Projection, typename Count>
            std::pair<ForwardIterator, Count>
            operator()(ForwardIterator begin, iterator_difference_t<ForwardIterator> dist,
                Value const& value, BinaryPredicate pred, Projection proj, Count c) const
            {
                RANGES_ASSERT(0 <= dist);
                using namespace std::placeholders;
                auto &&ipred = std::bind(pred, std::bind(proj, _1), _2);
                while(0 != dist)
                {
                    auto half = dist / 2;
                    auto middle = ranges::next(begin, half);
                    if(ipred(*middle, value))
                    {
                        begin = std::move(++middle);
                        dist -= half + 1;
                        c += half + 1;
                    }
                    else
                        dist = half;
                }
                return {begin, c};
            }

        public:
            template<typename ForwardIterator, typename Value,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::ForwardIterator<ForwardIterator>() &&
                    ranges::Invokable<Projection, iterator_value_t<ForwardIterator>>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, iterator_value_t<ForwardIterator>>,
                        Value>())>
            ForwardIterator
            operator()(ForwardIterator begin, iterator_difference_t<ForwardIterator> dist,
                Value const& value, BinaryPredicate pred = BinaryPredicate{},
                Projection proj = Projection{}) const
            {
                return (*this)(std::move(begin), dist, value, std::move(pred), std::move(proj),
                    detail::dummy_count{}).first;
            }

            /// \cond
            // Optimization for counted_iterators
            template<typename ForwardIterator, typename Value,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::ForwardIterator<ForwardIterator>() &&
                    ranges::Invokable<Projection, iterator_value_t<ForwardIterator>>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, iterator_value_t<ForwardIterator>>,
                        Value>())>
            counted_iterator<ForwardIterator>
            operator()(counted_iterator<ForwardIterator> begin,
                iterator_difference_t<ForwardIterator> dist, Value const& value,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                RANGES_ASSERT(0 <= dist);
                auto &&p = (*this)(begin.base(), dist, std::move(pred), std::move(proj),
                    iterator_difference_t<ForwardIterator>{0});
                return {p.first, begin.count() + p.second};
            }
            /// \endcond

            /// \overload
            template<typename ForwardIterable, typename Value,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable>() &&
                    ranges::ForwardIterator<range_iterator_t<ForwardIterable>>()&&
                    ranges::Invokable<Projection, range_value_t<ForwardIterable>>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, range_value_t<ForwardIterable>>,
                        Value>())>
            range_iterator_t<ForwardIterable>
            operator()(ForwardIterable &rng, range_difference_t<ForwardIterable> dist,
                Value const& value, BinaryPredicate pred = BinaryPredicate{},
                Projection proj = Projection{}) const
            {
                RANGES_ASSERT(0 <= dist);
                RANGES_ASSERT(dist <= ranges::distance(rng));
                return (*this)(ranges::begin(rng), dist, std::move(pred), std::move(proj));
            }

            /// \overload
            template<typename Value, typename Value2,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(
                    ranges::Invokable<Projection, Value>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, Value>,
                        Value2>())>
            Value const *
            operator()(std::initializer_list<Value> const &rng, std::ptrdiff_t dist,
                Value2 const& value, BinaryPredicate pred = BinaryPredicate{},
                Projection proj = Projection{}) const
            {
                RANGES_ASSERT(0 <= dist);
                RANGES_ASSERT((std::size_t)dist <= rng.size());
                return (*this)(rng.begin(), dist, std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR lower_bound_n_fn lower_bound_n{};

        struct lower_bound_fn
        {
            template<typename ForwardIterator, typename Sentinel, typename Value,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::ForwardIterator<ForwardIterator>() &&
                    ranges::Sentinel<Sentinel, ForwardIterator>() &&
                    ranges::Invokable<Projection, iterator_value_t<ForwardIterator>>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, iterator_value_t<ForwardIterator>>,
                        Value>())>
            ForwardIterator
            operator()(ForwardIterator begin, Sentinel end, Value const& value,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                return ranges::lower_bound_n(begin, ranges::distance(begin, end), value,
                    std::move(pred), std::move(proj));
            }

            /// \overload
            template<typename ForwardIterable, typename Value,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable>() &&
                    ranges::ForwardIterator<range_iterator_t<ForwardIterable>>()&&
                    ranges::Invokable<Projection, range_value_t<ForwardIterable>>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, range_value_t<ForwardIterable>>,
                        Value>())>
            range_iterator_t<ForwardIterable>
            operator()(ForwardIterable &rng, Value const& value,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                return ranges::lower_bound_n(ranges::begin(rng), ranges::distance(rng), value,
                    std::move(pred), std::move(proj));
            }

            /// \overload
            template<typename Value, typename Value2,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(
                    ranges::Invokable<Projection, Value>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, Value>,
                        Value2>())>
            Value const *
            operator()(std::initializer_list<Value> const &rng, Value2 const& value,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                return ranges::lower_bound_n(rng.begin(), (std::ptrdiff_t)rng.size(), value,
                    std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR lower_bound_fn lower_bound{};

    } // namespace v3
} // namespace ranges

#endif // include guard
