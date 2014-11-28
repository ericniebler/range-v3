// Range v3 library
//
//  Copyright Eric Niebler 2014
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

namespace ranges
{
    inline namespace v3
    {
        struct enumerate_fn : iter_enumerate_fn
        {
        private:
            template<typename Rng, typename D, typename I = range_iterator_t<Rng>>
            std::pair<D, I> impl_r(Rng &rng, D d, concepts::Iterable*, concepts::Iterable*) const
            {
                return iter_enumerate(begin(rng), end(rng), d);
            }
            template<typename Rng, typename D, typename I = range_iterator_t<Rng>>
            std::pair<D, I> impl_r(Rng &rng, D d, concepts::BoundedIterable*, concepts::SizedIterable*) const
            {
                return {static_cast<D>(size(rng)) + d, end(rng)};
            }
        public:
            using iter_enumerate_fn::operator();

            template<typename Rng, typename D = range_difference_t<Rng>,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Integral<D>() && Iterable<Rng>())>
            std::pair<D, I> operator()(Rng &&rng, D d = 0) const
            {
                static_assert(!is_infinite<Rng>::value,
                    "Trying to compute the length of an infinite range!");
                return this->impl_r(rng, d, bounded_iterable_concept<Rng>(),
                    sized_iterable_concept<Rng>());
            }
        };

        constexpr enumerate_fn enumerate{};

        struct distance_fn : iter_distance_fn
        {
        private:
            template<typename Rng, typename D>
            D impl_r(Rng &rng, D d, concepts::Iterable*) const
            {
                return enumerate(rng, d).first;
            }
            template<typename Rng, typename D>
            D impl_r(Rng &rng, D d, concepts::SizedIterable*) const
            {
                return static_cast<D>(size(rng)) + d;
            }
        public:
            using iter_distance_fn::operator();

            template<typename Rng, typename D = range_difference_t<Rng>,
                CONCEPT_REQUIRES_(Integral<D>() && Iterable<Rng>())>
            D operator()(Rng &&rng, D d = 0) const
            {
                static_assert(!is_infinite<Rng>::value,
                    "Trying to compute the length of an infinite range!");
                return this->impl_r(rng, d, sized_iterable_concept<Rng>());
            }
        };

        constexpr distance_fn distance {};

        // The interface of distance_compare is taken from Util.listLengthCmp in the GHC API.
        struct distance_compare_fn : iter_distance_compare_fn
        {
        private:
            template<typename Rng,
                CONCEPT_REQUIRES_(!is_infinite<Rng>())>
            int impl_r(Rng &rng, range_difference_t<Rng> n, concepts::Iterable*) const
            {
                return iter_distance_compare(begin(rng), end(rng), n);
            }
            template<typename Rng,
                CONCEPT_REQUIRES_(is_infinite<Rng>())>
            int impl_r(Rng &rng, range_difference_t<Rng> n, concepts::Iterable*) const
            {
                // Infinite ranges are always compared to be larger than a finite number.
                return 1;
            }
            template<typename Rng>
            int impl_r(Rng &rng, range_difference_t<Rng> n, concepts::SizedIterable*) const
            {
                range_difference_t<Rng> dist = static_cast<range_difference_t<Rng>>(size(rng));
                if (dist > n)
                    return  1;
                else if (dist < n)
                    return -1;
                else
                    return  0;
            }
        public:
            using iter_distance_compare_fn::operator();

            template<typename Rng,
                CONCEPT_REQUIRES_(Iterable<Rng>())>
            int operator()(Rng &&rng, range_difference_t<Rng> n) const
            {
                return this->impl_r(rng, n, sized_iterable_concept<Rng>());
            }
        };

        constexpr distance_compare_fn distance_compare {};
    }
}

#endif
