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
#ifndef RANGES_V3_ALGORITHM_RANDOM_SHUFFLE_HPP
#define RANGES_V3_ALGORITHM_RANDOM_SHUFFLE_HPP

#include <utility>
#include <algorithm>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct random_shuffler : bindable<random_shuffler>,
                                 pipeable<random_shuffler>
        {
            /// \brief template function random_shuffle
            ///
            /// range-based version of the random_shuffle std algorithm
            ///
            /// \pre RandomAccessRange is a model of the RandomAccessRange concept
            /// \pre Generator is a model of the UnaryFunction concept
            template<typename RandomAccessRange,
                     CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange invoke(random_shuffler, RandomAccessRange && rng)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::random_shuffle(ranges::begin(rng), ranges::end(rng));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename Generator>
            static RandomAccessRange invoke(random_shuffler, RandomAccessRange && rng, Generator& gen)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::random_shuffle(ranges::begin(rng), ranges::end(rng), gen);
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            /// for rng | random_shuffle(gen)
            template<typename Generator,
                CONCEPT_REQUIRES(!ranges::Range<Generator>())>
            static auto invoke(random_shuffler random_shuffle, Generator& gen)
                -> decltype(random_shuffle(std::placeholders::_1, std::ref(gen)))
            {
                return random_shuffle(std::placeholders::_1, std::ref(gen));
            }
        };

        RANGES_CONSTEXPR random_shuffler random_shuffle {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
