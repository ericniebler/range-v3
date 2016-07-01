/// \file
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
#include <range/v3/view_facade.hpp>
#include <range/v3/iterator_range.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/counted_iterator.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename I, typename D /* = iterator_difference_t<I>*/>
        struct counted_view
          : view_facade<counted_view<I, D>, finite>
        {
        private:
            friend range_access;
            using size_type_ = iterator_size_t<I>;
            I it_;
            D n_;

            detail::counted_cursor<I, D> begin_cursor() const
            {
                return {it_, n_};
            }
        public:
            counted_view() = default;
            counted_view(I it, D n)
              : it_(it), n_(n)
            {
                RANGES_ASSERT(0 <= n_);
            }
            size_type_ size() const
            {
                return static_cast<size_type_>(n_);
            }
        };

        namespace view
        {
            struct counted_fn
            {
                template<typename I,
                    CONCEPT_REQUIRES_(Iterator<I>())>
                counted_view<I> operator()(I it, iterator_difference_t<I> n) const
                {
                    return {std::move(it), n};
                }
                // TODO Once we support contiguous iterators, we can generalize this.
                // (Note: it's not possible for RandomAccessIterators in general because
                // of cyclic iterators.
                template<typename T>
                iterator_range<T*> operator()(T *t, std::ptrdiff_t n) const
                {
                    return {t, t + n};
                }
            };

            /// \relates counted_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(counted_fn, counted)
        }
        /// @}
    }
}

#endif
