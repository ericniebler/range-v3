//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_ROTATE_HPP
#define RANGES_V3_ALGORITHM_ROTATE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct rotate_fn
        {
            // With credit to Howard Hinnant
            template<typename I, typename S,
                CONCEPT_REQUIRES_(Permutable<I>() && Sentinel<S, I>())>
            I operator()(I begin, I middle, S end) const
            {
                if(begin == middle)
                    return next_to(begin, end);
                if(middle == end)
                    return begin;
                for(auto i = middle;;)
                {
                    ranges::swap(*begin, *i);
                    ++begin;
                    if(++i == end)
                        break;
                    if(begin == middle)
                        middle = i;
                }
                auto ret = begin;
                if(begin != middle)
                {
                    for(auto i = middle;;)
                    {
                        ranges::swap(*begin, *i);
                        ++begin;
                        if(++i == end)
                        {
                            if(begin == middle)
                                break;
                            i = middle;
                        }
                        else if(begin == middle)
                            middle = i;
                    }
                }
                return ret;
            }

            template<typename Rng, typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Iterable<Rng>() && Permutable<I>())>
            I operator()(Rng &rng, I middle) const
            {
                return (*this)(begin(rng), std::move(middle), end(rng));
            }
        };

        RANGES_CONSTEXPR rotate_fn rotate {};

    } // namespace v3
} // namespace ranges

#endif // include guard
