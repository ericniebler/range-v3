/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
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
#include <range/v3/view/interface.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/counted_iterator.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/iterator/default_sentinel.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename I>
    struct counted_view
      : view_interface<counted_view<I>>
    {
    private:
        friend range_access;
        I it_;
        iter_difference_t<I> n_;

    public:
        counted_view() = default;
        counted_view(I it, iter_difference_t<I> n)
          : it_(it), n_(n)
        {
            RANGES_EXPECT(0 <= n_);
        }
        counted_iterator<I> begin() const
        {
            return make_counted_iterator(it_, n_);
        }
        default_sentinel_t end() const
        {
            return {};
        }
        auto size() const
        {
            using size_type_ = meta::_t<std::make_unsigned<iter_difference_t<I>>>;
            return static_cast<size_type_>(n_);
        }
    };

    namespace view
    {
        struct counted_fn
        {
            template<typename I>
            auto operator()(I it, iter_difference_t<I> n) const ->
                CPP_ret(counted_view<I>)(
                    requires Iterator<I> && (!RandomAccessIterator<I>))
            {
                return {std::move(it), n};
            }
            template<typename I>
            auto operator()(I it, iter_difference_t<I> n) const ->
                CPP_ret(subrange<I>)(
                    requires RandomAccessIterator<I>)
            {
                return {it, it + n};
            }
        };

        /// \relates counted_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(counted_fn, counted)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::counted_view)

#endif
