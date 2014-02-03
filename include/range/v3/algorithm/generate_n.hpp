//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_GENERATE_N_HPP
#define RANGES_V3_ALGORITHM_GENERATE_N_HPP

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
            template<typename OutputIterator, typename Sentinel, typename Size, typename Generator>
            OutputIterator
            generate_n(OutputIterator out, Sentinel end, Size n, Generator gen)
            {
                for(; 0 < n && out != end; ++out, --n)
                    *out = gen();
                return out;
            }
        }

        struct generator_n : bindable<generator_n>
        {
            /// \brief template function generate_n
            ///
            /// range-based version of the generate_n std algorithm
            ///
            /// \pre OutputIterable is a model of the OutputIterable concept
            /// \pre Generator is a model of the NullaryFunction concept
            template<typename OutputIterable, typename Size, typename Generator>
            static range_iterator_t<OutputIterable>
            invoke(generator_n, OutputIterable && rng, Size n, Generator gen)
            {
                CONCEPT_ASSERT(ranges::Callable<Generator>());
                CONCEPT_ASSERT(ranges::OutputIterable<OutputIterable,
                                                      concepts::Callable::result_t<Generator>>());
                return detail::generate_n(ranges::begin(rng), ranges::end(rng), n,
                    ranges::make_invokable(std::move(gen)));
            }

            /// \overload
            /// for rng | generate_n(10, gen)
            template<typename Size, typename Generator>
            static auto invoke(generator_n generate_n, Size n, Generator gen) ->
                decltype(generate_n.move_bind(std::placeholders::_1, (Size)n, std::move(gen)))
            {
                CONCEPT_ASSERT(ranges::Callable<Generator>());
                return generate_n.move_bind(std::placeholders::_1, (Size)n, std::move(gen));
            }
        };

        RANGES_CONSTEXPR generator_n generate_n {};

    } // inline namespace v3

} // namespace ranges

#endif // include guard
