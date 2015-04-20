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
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_ALGORITHM_MINMAX_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MINMAX_ELEMENT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-algorithms
        /// @{
        struct minmax_element_fn
        {
            template<typename I, typename S, typename C = ordered_less, typename P = ident,
                CONCEPT_REQUIRES_(ForwardIterator<I>() && IteratorRange<I, S>() &&
                    IndirectCallableRelation<C, Project<I, P>>())>
            RANGES_RELAXED_CONSTEXPR
            std::pair<I, I> operator()(I begin, S end, C pred_ = C{}, P proj_ = P{}) const
            {
                auto && pred = as_function(pred_);
                auto && proj = as_function(proj_);
                std::pair<I, I> result{begin, begin};
                if(begin == end || ++begin == end)
                    return result;
                if(pred(proj(*begin), proj(*result.first)))
                    result.first = begin;
                else
                    result.second = begin;
                while(++begin != end)
                {
                    I tmp = begin;
                    if(++begin == end)
                    {
                        if(pred(proj(*tmp), proj(*result.first)))
                            result.first = tmp;
                        else if(!pred(proj(*tmp), proj(*result.second)))
                            result.second = tmp;
                        break;
                    }
                    else
                    {
                        if(pred(proj(*begin), proj(*tmp)))
                        {
                            if(pred(proj(*begin), proj(*result.first)))
                                result.first = begin;
                            if(!pred(proj(*tmp), proj(*result.second)))
                                result.second = tmp;
                        }
                        else
                        {
                            if(pred(proj(*tmp), proj(*result.first)))
                                result.first = tmp;
                            if(!pred(proj(*begin), proj(*result.second)))
                                result.second = begin;
                        }
                    }
                }
                return result;
            }

            template<typename Rng, typename C = ordered_less, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(ForwardIterable<Rng>() &&
                    IndirectCallableRelation<C, Project<I, P>>())>
            RANGES_RELAXED_CONSTEXPR
            meta::if_<std::is_lvalue_reference<Rng>, std::pair<I, I>, dangling<std::pair<I, I>>>
            operator()(Rng &&rng, C pred = C{}, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(pred), std::move(proj));
            }
        };

        /// \sa `minmax_element_fn`
        /// \ingroup group-algorithms
        namespace
        {
            constexpr auto&& minmax_element = static_const<with_braced_init_args<minmax_element_fn>>::value;
        }

        /// @}
    } // namespace v3
} // namespace ranges

#endif // include guard
