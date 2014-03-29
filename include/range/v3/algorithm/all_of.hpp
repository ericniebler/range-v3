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
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_HPP

#include <utility>
#include <functional>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            // requires I is an InputIterator, S is a sentinel for
            // I and P is a predicate accepting an argument of type
            // value_type_t<I>.
            template<typename I, typename S, typename P>
            I all_of(I begin, S end, P pred)
            {
                while(begin != end && pred(*begin))
                    ++begin;
                return begin == end;
            }
        }

        struct all_of_fn : bindable<all_of_fn>
        {
            /// \brief template function \c all_of_fn::operator()
            ///
            /// range-based version of the \c all_of std algorithm
            ///
            /// \pre \c I is a model of InputIterable
            /// \pre \c P is a model of Predicate, accepting a single
            /// argument of type value_type_t<I>.
            template<typename I, typename P>
            static range_iterator_t<I>
            invoke(all_of_fn, I && rng, P pred)
            {
                CONCEPT_ASSERT(ranges::Iterable<I>());
                CONCEPT_ASSERT(ranges::InputItator<range_iterator_t<I>>());
                CONCEPT_ASSERT(ranges::Predicate<invokable_t<P>, range_value_t<I>>());
                return detail::all_of(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(pred)));
            }

            /// \overload
            /// for rng | all_of(pred)
            template<typename P>
            static auto invoke(all_of_fn f, P pred) ->
                decltype(f.move_bind(std::placeholders::_1, std::move(pred)))
            {
                return f.move_bind(std::placeholders::_1, std::move(pred));
            }
        };

        RANGES_CONSTEXPR all_of_fn all_of {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
