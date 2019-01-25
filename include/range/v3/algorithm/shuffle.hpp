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
#ifndef RANGES_V3_ALGORITHM_SHUFFLE_HPP
#define RANGES_V3_ALGORITHM_SHUFFLE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/random.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/swap.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    struct shuffle_fn
    {
        template<typename I, typename S, typename Gen = detail::default_random_engine&>
        auto operator()(I const begin, S const end,
                Gen &&gen = detail::get_random_engine()) const ->
            CPP_ret(I)(
                requires RandomAccessIterator<I> && Sentinel<S, I> && Permutable<I> &&
                    UniformRandomNumberGenerator<Gen> &&
                    ConvertibleTo<invoke_result_t<Gen &>, iter_difference_t<I>>)
        {
            auto mid = begin;
            if(mid == end)
                return mid;
            std::uniform_int_distribution<iter_difference_t<I>> uid{};
            using param_t = typename decltype(uid)::param_type;
            while(++mid != end)
            {
                if(auto const i = uid(gen, param_t{0, mid - begin}))
                    ranges::iter_swap(mid - i, mid);
            }
            return mid;
        }

        template<typename Rng, typename Gen = detail::default_random_engine&>
        auto operator()(Rng &&rng, Gen &&rand = detail::get_random_engine()) const ->
            CPP_ret(safe_iterator_t<Rng>)(
                requires RandomAccessRange<Rng> && Permutable<iterator_t<Rng>> &&
                    UniformRandomNumberGenerator<Gen> &&
                    ConvertibleTo<invoke_result_t<Gen &>, iter_difference_t<iterator_t<Rng>>>)
        {
            return (*this)(begin(rng), end(rng), static_cast<Gen &&>(rand));
        }
    };

    /// \sa `shuffle_fn`
    /// \ingroup group-algorithms
    RANGES_INLINE_VARIABLE(shuffle_fn, shuffle)
    /// @}
} // namespace ranges

#endif // include guard
