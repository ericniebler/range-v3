/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Michel Morin 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_DISTANCE_HPP
#define RANGES_V3_DISTANCE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        struct enumerate_fn : iter_enumerate_fn
        {
        private:
            template<typename Rng, typename D, typename I = iterator_t<Rng>>
            std::pair<D, I> impl_r(Rng &rng, D d, concepts::Range*, concepts::Range*) const
            {
                return iter_enumerate(begin(rng), end(rng), d);
            }
            template<typename Rng, typename D, typename I = iterator_t<Rng>>
            std::pair<D, I> impl_r(Rng &rng, D d, concepts::BoundedRange*, concepts::SizedRange*) const
            {
                return {static_cast<D>(size(rng)) + d, end(rng)};
            }
        public:
            using iter_enumerate_fn::operator();

            template<typename Rng, typename D = range_difference_type_t<Rng>,
                typename I = iterator_t<Rng>,
                CONCEPT_REQUIRES_(Integral<D>() && Range<Rng>())>
            std::pair<D, I> operator()(Rng &&rng, D d = 0) const
            {
                // Better not be trying to compute the distance of an infinite range:
                RANGES_EXPECT(!is_infinite<Rng>::value);
                auto result = this->impl_r(rng, d, bounded_range_concept<Rng>(),
                    sized_range_concept<Rng>());
                RANGES_EXPECT(result.first >= d);
                return result;
            }
        };

        /// \ingroup group-core
        /// \sa `enumerate_fn`
        RANGES_INLINE_VARIABLE(enumerate_fn, enumerate)

        struct distance_fn : iter_distance_fn
        {
        private:
            template<typename Rng, typename D>
            D impl_r(Rng &rng, D d, concepts::Range*) const
            {
                return enumerate(rng, d).first;
            }
            template<typename Rng, typename D>
            RANGES_CXX14_CONSTEXPR
            D impl_r(Rng &rng, D d, concepts::SizedRange*) const
            {
                return static_cast<D>(size(rng)) + d;
            }
        public:
            using iter_distance_fn::operator();

            template<typename Rng, typename D = range_difference_type_t<Rng>,
                CONCEPT_REQUIRES_(Integral<D>() && Range<Rng>())>
            RANGES_CXX14_CONSTEXPR
            D operator()(Rng &&rng, D d = 0) const
            {
                // Better not be trying to compute the distance of an infinite range:
                RANGES_EXPECT(!is_infinite<Rng>::value);
                auto result = this->impl_r(rng, d, sized_range_concept<Rng>());
                RANGES_EXPECT(result >= d);
                return result;
            }
        };

        /// \ingroup group-core
        /// \sa `distance_fn`
        RANGES_INLINE_VARIABLE(distance_fn, distance)

        // The interface of distance_compare is taken from Util.listLengthCmp in the GHC API.
        struct distance_compare_fn : iter_distance_compare_fn
        {
        private:
            template<typename Rng,
                CONCEPT_REQUIRES_(!is_infinite<Rng>())>
            int impl_r(Rng &rng, range_difference_type_t<Rng> n, concepts::Range*) const
            {
                return iter_distance_compare(begin(rng), end(rng), n);
            }
            template<typename Rng,
                CONCEPT_REQUIRES_(is_infinite<Rng>())>
            int impl_r(Rng &, range_difference_type_t<Rng>, concepts::Range*) const
            {
                // Infinite ranges are always compared to be larger than a finite number.
                return 1;
            }
            template<typename Rng>
            int impl_r(Rng &rng, range_difference_type_t<Rng> n, concepts::SizedRange*) const
            {
                auto dist = distance(rng); // O(1) since rng is a SizedRange
                if(dist > n)
                    return  1;
                else if(dist < n)
                    return -1;
                else
                    return  0;
            }
        public:
            using iter_distance_compare_fn::operator();

            template<typename Rng,
                CONCEPT_REQUIRES_(Range<Rng>())>
            int operator()(Rng &&rng, range_difference_type_t<Rng> n) const
            {
                return this->impl_r(rng, n, sized_range_concept<Rng>());
            }
        };

        /// \ingroup group-core
        /// \sa `distance_compare_fn`
        RANGES_INLINE_VARIABLE(distance_compare_fn, distance_compare)
        /// @}
    }
}

#endif
