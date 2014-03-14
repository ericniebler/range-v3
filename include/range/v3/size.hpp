// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_SIZE_HPP
#define RANGES_V3_SIZE_HPP

#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_size_detail
        {
            namespace impl
            {
                // Not a real concept!
                struct HasSizeConcept
                  : concepts::refines<concepts::Iterable>
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            concepts::convertible_to<range_difference_t<T>>(t.size())
                        ));
                };

                template<typename T>
                using HasSize = concepts::models<HasSizeConcept, T>;

                struct Int { Int(long) {} };

                template<typename Iterable,
                    CONCEPT_REQUIRES_(HasSize<Iterable>())>
                range_difference_t<Iterable>
                size(Iterable const &rng, int)
                {
                    return static_cast<range_difference_t<Iterable>>(rng.size());
                }

                template<typename RandomAccessRange,
                    CONCEPT_REQUIRES_(ranges::Range<RandomAccessRange>() &&
                                      ranges::RandomAccessIterator<range_iterator_t<RandomAccessRange>>())>
                range_difference_t<RandomAccessRange>
                size(RandomAccessRange const &rng, Int)
                {
                    return ranges::end(rng) - ranges::begin(rng);
                }
            }

            template<typename Iterable>
            inline auto range_size(Iterable const &rng) -> decltype(impl::size(rng, 42))
            {
                return impl::size(rng, 42);
            }

            struct sizer : bindable<sizer>
            {
                template<typename Iterable>
                static auto invoke(sizer, Iterable const &rng) -> decltype(range_size(rng))
                {
                    return range_size(rng);
                }
            };
        }

        RANGES_CONSTEXPR adl_size_detail::sizer size {};
    }
}

#endif
