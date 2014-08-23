// Boost.Range library
//
//  Copyright Eric Niebler 2014.
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

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace adl_size_detail
        {
            template<typename Rng>
            constexpr auto size_(Rng && rng, long) ->
                decltype(iterator_range_size(begin(rng), end(rng)))
            {
                return iterator_range_size(begin(rng), end(rng));
            }

            template<typename Rng>
            auto size_(Rng && rng, int) ->
                decltype(rng.size())
            {
                return rng.size();
            }

            template<typename Rng>
            auto size(Rng && rng) ->
                decltype(adl_size_detail::size_(std::forward<Rng>(rng), 42))
            {
                return adl_size_detail::size_(std::forward<Rng>(rng), 42);
            }

            // Built-in arrays
            template<typename T, std::size_t N>
            constexpr std::size_t size(T (&)[N])
            {
                return N;
            }

            template<typename T, std::size_t N>
            constexpr std::size_t size(T const (&)[N])
            {
                return N;
            }

            struct size_fn
            {
                template<typename Rng>
                constexpr auto operator()(Rng &&rng) const ->
                    decltype(size(std::forward<Rng>(rng)))
                {
                    return size(rng);
                }
            };
        }

        RANGES_CONSTEXPR adl_size_detail::size_fn size {};
    }
}

#endif
