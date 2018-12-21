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
#ifndef RANGES_V3_ALGORITHM_COPY_BACKWARD_HPP
#define RANGES_V3_ALGORITHM_COPY_BACKWARD_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/result_types.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        template<typename I, typename O>
        using copy_backward_result = detail::in_out_result<I, O>;

        struct copy_backward_fn
        {
            template<typename I, typename S, typename O>
            auto operator()(I begin, S end_, O out) const ->
                CPP_ret(copy_backward_result<I, O>)(
                    requires BidirectionalIterator<I> && Sentinel<S, I> &&
                        BidirectionalIterator<O> && IndirectlyCopyable<I, O>)
            {
                I i = ranges::next(begin, end_), end = i;
                while(begin != i)
                    *--out = *--i;
                return {end, out};
            }

            template<typename Rng, typename O>
            auto operator()(Rng &&rng, O out) const ->
                CPP_ret(copy_backward_result<safe_iterator_t<Rng>, O>)(
                    requires BidirectionalRange<Rng> && BidirectionalIterator<O> &&
                        IndirectlyCopyable<iterator_t<Rng>, O>)
            {
                return (*this)(begin(rng), end(rng), std::move(out));
            }
        };

        /// \sa `copy_backward_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<copy_backward_fn>,
                               copy_backward)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
