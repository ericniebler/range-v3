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
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/random.hpp>
#include <range/v3/utility/swap.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct shuffle_fn
        {
            template<typename I, typename S, typename Gen = detail::default_random_engine&,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>() && Sentinel<S, I>() &&
                    Permutable<I>() && UniformRandomNumberGenerator<Gen>() &&
                    ConvertibleTo<
                        concepts::UniformRandomNumberGenerator::result_t<Gen>,
                        difference_type_t<I>>())>
            I operator()(I const begin, S const end,
                Gen && gen = detail::get_random_engine()) const
            {
                auto mid = begin;
                if(mid == end)
                    return mid;
                std::uniform_int_distribution<difference_type_t<I>> uid{};
                using param_t = typename decltype(uid)::param_type;
                while(++mid != end)
                {
                    if(auto const i = uid(gen, param_t{0, mid - begin}))
                        ranges::iter_swap(mid - i, mid);
                }
                return mid;
            }

            template<typename Rng, typename Gen = detail::default_random_engine&,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(RandomAccessRange<Rng>() && Permutable<I>() &&
                    UniformRandomNumberGenerator<Gen>() && ConvertibleTo<
                        concepts::UniformRandomNumberGenerator::result_t<Gen>,
                        difference_type_t<I>>())>
            safe_iterator_t<Rng>
            operator()(Rng && rng, Gen && rand = detail::get_random_engine()) const
            {
                return (*this)(begin(rng), end(rng), static_cast<Gen&&>(rand));
            }
        };

        /// \sa `shuffle_fn`
        /// \ingroup group-algorithms
        RANGES_INLINE_VARIABLE(with_braced_init_args<shuffle_fn>, shuffle)
        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
