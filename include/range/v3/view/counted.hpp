// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_COUNTED_HPP
#define RANGES_V3_VIEW_COUNTED_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/counted_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename D /* = iterator_difference_t<I>*/>
        struct counted_view
          : range_facade<counted_view<I>>
        {
        private:
            friend range_access;
            using size_type = iterator_size_t<I>;
            I it_;
            D n_;

            detail::counted_cursor<I> begin_cursor() const
            {
                return {it_, n_};
            }
            detail::counted_sentinel end_cursor() const
            {
                return {};
            }
        public:
            counted_view() = default;
            counted_view(I it, D n)
              : it_(it), n_(n)
            {
                RANGES_ASSERT(0 <= n_);
            }
            size_type size() const
            {
                return static_cast<size_type>(n_);
            }
        };

        namespace view
        {
            struct counted_fn : bindable<counted_fn>
            {
                template<typename I, typename D>
                static counted_view<I, D> invoke(counted_fn, I it, D n)
                {
                    // Nothing wrong with a weak counted output iterator!
                    CONCEPT_ASSERT(WeakIterator<I>());
                    return {std::move(it), n};
                }
            };

            RANGES_CONSTEXPR counted_fn counted {};
        }
    }
}

#endif
