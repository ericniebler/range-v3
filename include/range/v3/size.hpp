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
                // An Iterable with a .size() member function; e.g., std::list
                struct SizedIterableConcept
                  : concepts::refines<concepts::Iterable>
                {
                    template<typename T>
                    auto requires(T && t) -> decltype(
                        concepts::valid_expr(
                            concepts::convertible_to<range_difference_t<T>>(t.size())
                        ));
                };

                template<typename T>
                using SizedIterable = concepts::models<SizedIterableConcept, T>;

                struct Int { Int(long) {} };

                template<typename Iterable,
                    CONCEPT_REQUIRES_(SizedIterable<Iterable>())>
                range_difference_t<Iterable>
                size(Iterable && rng, int)
                {
                    return static_cast<range_difference_t<Iterable>>(rng.size());
                }

                template<typename Iterable,
                    CONCEPT_REQUIRES_(CountedIterable<Iterable>())>
                range_difference_t<Iterable>
                size(Iterable && rng, long)
                {
                    return rng.end().count();
                }

                template<typename RandomAccessRange,
                    CONCEPT_REQUIRES_(ranges::RandomAccessRange<RandomAccessRange>())>
                range_difference_t<RandomAccessRange>
                size(RandomAccessRange && rng, Int)
                {
                    return ranges::end(rng) - ranges::begin(rng);
                }
            }

            template<typename Iterable>
            inline auto size(Iterable && rng) -> decltype(impl::size(rng, 42))
            {
                return impl::size(rng, 42);
            }

            struct sizer : bindable<sizer>
            {
                template<typename Iterable>
                static auto invoke(sizer, Iterable && rng) -> decltype(size(rng))
                {
                    return size(rng);
                }
            };
        }

        RANGES_CONSTEXPR adl_size_detail::sizer size {};
    }
}

#endif
