//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP
#define RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP

#include <utility>
#include <functional>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {

        struct binary_search_fn
        {
            /// \brief function template \c binary_search_fn::operator()
            ///
            /// range-based version of the \c binary_search std algorithm
            ///
            /// \pre \c ForwardIterable is a model of the ForwardIterable concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardIterator, typename Sentinel, typename Value,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::ForwardIterator<ForwardIterator>() &&
                    ranges::Sentinel<Sentinel, ForwardIterator>() &&
                    ranges::Invokable<Projection, iterator_value_t<ForwardIterator>>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, iterator_value_t<ForwardIterator>>,
                        Value>())>
            bool operator()(ForwardIterator begin, Sentinel end, Value const &val,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                using namespace std::placeholders;
                begin = ranges::lower_bound(std::move(begin), end, val, pred, proj);
                auto &&ipred = std::bind(pred, _1, std::bind(proj, _2));
                return begin != end && !ipred(val, *begin);
            }

            /// \overload
            template<typename ForwardIterable, typename Value,
                typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::Iterable<ForwardIterable>() &&
                    ranges::ForwardIterator<range_iterator_t<ForwardIterable>>() &&
                    ranges::Invokable<Projection, range_value_t<ForwardIterable>>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, range_value_t<ForwardIterable>>,
                        Value>())>
            bool operator()(ForwardIterable &&rng, Value const &val,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                static_assert(!ranges::is_infinite<ForwardIterable>::value,
                    "Trying to binary search an infinite range");
                return (*this)(ranges::begin(rng), ranges::end(rng), val, std::move(pred),
                    std::move(proj));
            }

            /// \overload
            template<typename Value, typename Value2, typename BinaryPredicate = ranges::less,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::Invokable<Projection, Value>() &&
                    ranges::Invokable<BinaryPredicate,
                        concepts::Invokable::result_t<Projection, Value>,
                        Value2>())>
            bool operator()(std::initializer_list<Value> const &rng, Value2 const &val,
                BinaryPredicate pred = BinaryPredicate{}, Projection proj = Projection{}) const
            {
                return (*this)(rng.begin(), rng.end(), val, std::move(pred), std::move(proj));
            }
        };

        RANGES_CONSTEXPR binary_search_fn binary_search{};

    } // namespace v3
} // namespace ranges

#endif // include guard
