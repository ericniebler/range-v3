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
#ifndef RANGES_V3_ALGORITHM_GENERATE_HPP
#define RANGES_V3_ALGORITHM_GENERATE_HPP

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
            template<typename ForwardIterator, typename Sentinel, typename Generator>
            void generate(ForwardIterator begin, Sentinel end, Generator gen)
            {
                for(; begin != end; ++begin)
                    *begin = gen();
            }
        }

        struct generator : bindable<generator>
        {
            /// \brief template function generate
            ///
            /// range-based version of the generate std algorithm
            ///
            /// \pre ForwardIterable is a model of the ForwardIterable concept
            /// \pre Generator is a model of the NullaryFunction concept
            template<typename ForwardIterable, typename Generator>
            static ForwardIterable invoke(generator, ForwardIterable && rng, Generator gen)
            {
                CONCEPT_ASSERT(ranges::Iterable<ForwardIterable>());
                CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardIterable>>());
                CONCEPT_ASSERT(ranges::Callable<Generator>());
                std::generate(ranges::begin(rng), ranges::end(rng),
                    ranges::make_invokable(std::move(gen)));
                return std::forward<ForwardIterable>(rng);
            }

            /// \overload
            /// for rng | generate(gen)
            template<typename Generator>
            static auto invoke(generator generate, Generator gen) ->
                decltype(generate.move_bind(std::placeholders::_1, std::move(gen)))
            {
                CONCEPT_ASSERT(ranges::Callable<Generator>());
                return generate.move_bind(std::placeholders::_1, std::move(gen));
            }
        };

        RANGES_CONSTEXPR generator generate {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
