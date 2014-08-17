//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_REMOVE_COPY_HPP
#define RANGES_V3_ALGORITHM_REMOVE_COPY_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/range_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename I, typename O, typename T, typename P = ident,
            typename V = iterator_value_t<I>,
            typename X = concepts::Invokable::result_t<P, V>>
        constexpr bool RemoveCopyable()
        {
            return InputIterator<I>() &&
                   WeaklyIncrementable<O>() &&
                   EqualityComparable<X, T>() &&
                   IndirectlyProjectedCopyable<I, P, O>();
        }

        struct remove_copy_fn
        {
            template<typename I, typename S, typename O, typename T, typename P = ident,
                CONCEPT_REQUIRES_(RemoveCopyable<I, O, T, P>() && Sentinel<S, I>())>
            std::pair<I, O> operator()(I begin, S end, O out, T const &val, P proj_ = P{}) const
            {
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin)
                {
                    auto &&v = proj(*begin);
                    if(!(v == val))
                    {
                        *out = v;
                        ++out;
                    }
                }
                return {begin, out};
            }

            template<typename Rng, typename O, typename T, typename P = ident,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(RemoveCopyable<I, O, T, P>() && InputRange<Rng>())>
            std::pair<I, O> operator()(Rng &rng, O out, T const &val, P proj = P{}) const
            {
                return (*this)(begin(rng), end(rng), std::move(out), val, std::move(proj));
            }
        };

        RANGES_CONSTEXPR range_algorithm<remove_copy_fn> remove_copy{};

    } // namespace v3
} // namespace ranges

#endif // include guard
