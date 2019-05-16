/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_REVERSE_HPP
#define RANGES_V3_ALGORITHM_REVERSE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct reverse_fn
    {
    private:
        template<typename I>
        static void impl(I begin, I end, detail::bidirectional_iterator_tag_)
        {
            while(begin != end)
            {
                if(begin == --end)
                    break;
                ranges::iter_swap(begin, end);
                ++begin;
            }
        }

        template<typename I>
        static void impl(I begin, I end, detail::random_access_iterator_tag_)
        {
            if(begin != end)
                for(; begin < --end; ++begin)
                    ranges::iter_swap(begin, end);
        }

    public:
        template<typename I, typename S>
        auto operator()(I begin, S end_) const ->
            CPP_ret(I)(
                requires BidirectionalIterator<I> && Sentinel<S, I> && Permutable<I>)
        {
            I end = ranges::next(begin, end_);
            reverse_fn::impl(begin, end, iterator_tag_of<I>{});
            return end;
        }

        template<typename Rng, typename I = iterator_t<Rng>>
        auto operator()(Rng &&rng) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires BidirectionalRange<Rng> && Permutable<I>)
        {
            return (*this)(begin(rng), end(rng));
        }
    };

    /// \sa `reverse_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(reverse_fn, reverse)

    namespace cpp20
    {
        using ranges::reverse;
    }
    /// @}
} // namespace ranges

#endif // include guard
