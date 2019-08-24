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

#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct reverse_fn
    {
    private:
        template<typename I>
        static void impl(I first, I last, detail::bidirectional_iterator_tag_)
        {
            while(first != last)
            {
                if(first == --last)
                    break;
                ranges::iter_swap(first, last);
                ++first;
            }
        }

        template<typename I>
        static void impl(I first, I last, detail::random_access_iterator_tag_)
        {
            if(first != last)
                for(; first < --last; ++first)
                    ranges::iter_swap(first, last);
        }

    public:
        template<typename I, typename S>
        auto operator()(I first, S end_) const -> CPP_ret(I)( //
            requires bidirectional_iterator<I> && sentinel_for<S, I> && permutable<I>)
        {
            I last = ranges::next(first, end_);
            reverse_fn::impl(first, last, iterator_tag_of<I>{});
            return last;
        }

        template<typename Rng, typename I = iterator_t<Rng>>
        auto operator()(Rng && rng) const -> CPP_ret(safe_iterator_t<Rng>)( //
            requires bidirectional_range<Rng> && permutable<I>)
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
