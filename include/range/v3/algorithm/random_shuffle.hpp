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
            /// \pre RandomNumberGenerator is a model of the UnaryFunction concept
            template<typename RandomAccessRange,
                     CONCEPT_REQUIRES(ranges::Range<RandomAccessRange>())>
            static RandomAccessRange invoke(random_shuffler, RandomAccessRange && rng)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                std::random_shuffle(ranges::begin(rng), ranges::end(rng));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            template<typename RandomAccessRange, typename RandomNumberGenerator>
            static RandomAccessRange invoke(random_shuffler, RandomAccessRange && rng,
                RandomNumberGenerator && gen)
            {
                CONCEPT_ASSERT(ranges::RandomAccessRange<RandomAccessRange>());
                CONCEPT_ASSERT(ranges::Callable<RandomNumberGenerator>());
                CONCEPT_ASSERT(ranges::Convertible<result_of_t<RandomNumberGenerator()>,
                                                   range_difference_t<RandomAccessRange>>());
                std::random_shuffle(ranges::begin(rng), ranges::end(rng),
                    std::forward<RandomNumberGenerator>(gen));
                return std::forward<RandomAccessRange>(rng);
            }

            /// \overload
            /// for rng | random_shuffle(gen)
            template<typename RandomNumberGenerator,
                CONCEPT_REQUIRES(!ranges::Range<RandomNumberGenerator>())>
            static auto invoke(random_shuffler random_shuffle, RandomNumberGenerator && gen)
                -> decltype(random_shuffle.move_bind(std::placeholders::_1,
                    std::forward<RandomNumberGenerator>(gen)))
            {
                return random_shuffle.move_bind(std::placeholders::_1,
                    std::forward<RandomNumberGenerator>(gen));
            }
        };

        RANGES_CONSTEXPR random_shuffler random_shuffle {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
