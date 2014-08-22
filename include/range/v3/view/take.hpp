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

#ifndef RANGES_V3_VIEW_TAKE_HPP
#define RANGES_V3_VIEW_TAKE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Rng>
        struct take_view
          : detail::basic_sliced_view<Rng, true>
        {
            CONCEPT_ASSERT(ConvertibleToInputRange<Rng>());

            using size_type = range_size_t<Rng>;

            take_view() = default;
            take_view(Rng && rng, size_type to)
              : detail::basic_sliced_view<Rng, true>{std::forward<Rng>(rng), to}
            {}
        };

        namespace view
        {
            struct take_fn : bindable<take_fn>
            {
                template<typename Rng,
                    CONCEPT_REQUIRES_(ConvertibleToInputRange<Rng>())>
                static take_view<Rng>
                invoke(take_fn, Rng && rng, range_size_t<Rng> to)
                {
                    return {std::forward<Rng>(rng), to};
                }
                template<typename I,
                    CONCEPT_REQUIRES_(InputIterator<I>())>
                static counted_view<I>
                invoke(take_fn, I it, iterator_size_t<I> n)
                {
                    return {std::move(it),
                            static_cast<iterator_difference_t<I>>(n)};
                }
                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                static auto
                invoke(take_fn take, Int to) ->
                    decltype(take.move_bind(std::placeholders::_1, (Int)to))
                {
                    return take.move_bind(std::placeholders::_1, (Int)to);
                }
            };

            RANGES_CONSTEXPR take_fn take {};
        }
    }
}

#endif
