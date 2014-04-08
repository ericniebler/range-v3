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
#ifndef RANGES_V3_ALGORITHM_COPY_HPP
#define RANGES_V3_ALGORITHM_COPY_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct copy_fn
        {
            template<typename InputIterator, typename Sentinel, typename OutputIterator,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::InputIterator<InputIterator>() &&
                    ranges::Sentinel<Sentinel, InputIterator>() &&
                    ranges::Invokable<Projection, iterator_value_t<InputIterator>>() &&
                    ranges::WeakOutputIterator<OutputIterator,
                        concepts::Invokable::result_t<Projection, iterator_value_t<InputIterator>>>())>
            std::pair<InputIterator, OutputIterator>
            operator()(InputIterator begin, Sentinel end, OutputIterator out,
                Projection proj = Projection{}) const
            {
                auto &&iproj = make_invokable(proj);
                for(; begin != end; ++begin, ++out)
                    *out = iproj(*begin);
                return {begin, out};
            }

            template<typename InputIterable, typename OutputIterator,
                typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::Iterable<InputIterable>() &&
                    ranges::InputIterator<range_iterator_t<InputIterable>>() &&
                    ranges::Invokable<Projection, range_value_t<InputIterable>>() &&
                    ranges::WeakOutputIterator<OutputIterator,
                        concepts::Invokable::result_t<Projection, range_value_t<InputIterable>>>())>
            std::pair<range_iterator_t<InputIterable>, OutputIterator>
            operator()(InputIterable &rng, OutputIterator out, Projection proj = Projection{}) const
            {
                return (*this)(ranges::begin(rng), ranges::end(rng), std::move(out), std::move(proj));
            }

            template<typename Value, typename OutputIterator, typename Projection = ranges::ident,
                CONCEPT_REQUIRES_(ranges::Invokable<Projection, Value>() &&
                    ranges::WeakOutputIterator<OutputIterator,
                        concepts::Invokable::result_t<Projection, Value>>())>
            std::pair<Value const *, OutputIterator>
            operator()(std::initializer_list<Value> const &rng, OutputIterator out,
                Projection proj = Projection{}) const
            {
                return (*this)(rng.begin(), rng.end(), std::move(out), std::move(proj));
            }
        };

        RANGES_CONSTEXPR copy_fn copy{};

    } // namespace v3
} // namespace ranges

#endif // include guard
