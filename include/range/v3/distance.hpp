// Boost.Range library
//
//  Copyright Eric Niebler 2014.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_DISTANCE_HPP
#define RANGES_V3_DISTANCE_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator_concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct enumerate_fn
        {
        private:
            template<typename I, typename S, typename D>
            std::pair<D, I> impl_i(I begin, S end, D d, concepts::InputIterator) const
            {
                for(; begin != end; ++begin)
                    ++d;
                return {d, begin};
            }

            template<typename I, typename D>
            std::pair<D, I> impl_i(I begin, I end, D d, concepts::RandomAccessIterator) const
            {
                return {(end - begin) + d, end};
            }

            template<typename I, typename D, typename C>
            std::pair<D, counted_iterator<I>>
            impl_i(counted_iterator<I> begin, counted_iterator<I> end, D d, C) const
            {
                return {(end.count() - begin.count()) + d, end};
            }

            template<typename Rng, typename D, typename I = range_iterator_t<Rng>>
            std::pair<D, I> impl_r(Rng &rng, D d, concepts::Iterable, concepts::Iterable) const
            {
                return (*this)(begin(rng), end(rng), d);
            }

            template<typename Rng, typename D, typename I = range_iterator_t<Rng>>
            std::pair<D, I> impl_r(Rng &rng, D d, concepts::Range, concepts::SizedIterable) const
            {
                return {static_cast<D>(size(rng)) + d, end(rng)};
            }
        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(InputIterator<I, S>() && Integral<D>())>
            std::pair<D, I> operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d,
                    iterator_concept_t<I>{});
            }

            template<typename Rng, typename D = range_difference_t<Rng>,
                typename I = range_iterator_t<Rng>,
                CONCEPT_REQUIRES_(Integral<D>() && Iterable<Rng>())>
            std::pair<D, I> operator()(Rng &&rng, D d = 0) const
            {
                static_assert(!is_infinite<Rng>::value,
                    "Trying to compute the length of an infinite range!");
                return this->impl_r(rng, d, range_concept_t<Rng>{}, sized_iterable_concept_t<Rng>{});
            }
        };

        RANGES_CONSTEXPR enumerate_fn enumerate{};

        struct distance_fn
        {
        private:
            template<typename I, typename S, typename D>
            D impl_i(I begin, S end, D d) const
            {
                return enumerate(std::move(begin), std::move(end), d).first;
            }

            template<typename I, typename D>
            D impl_i(counted_iterator<I> begin, counted_sentinel<I> end, D d) const
            {
                return static_cast<D>(end.count() - begin.count()) + d;
            }

            template<typename Rng, typename D>
            D impl_r(Rng &rng, D d, concepts::Iterable) const
            {
                return enumerate(rng, d).first;
            }

            template<typename Rng, typename D>
            D impl_r(Rng &rng, D d, concepts::SizedIterable) const
            {
                return static_cast<D>(size(rng)) + d;
            }

        public:
            template<typename I, typename S, typename D = iterator_difference_t<I>,
                CONCEPT_REQUIRES_(InputIterator<I, S>() && Integral<D>())>
            D operator()(I begin, S end, D d = 0) const
            {
                return this->impl_i(std::move(begin), std::move(end), d);
            }

            template<typename Rng, typename D = range_difference_t<Rng>,
                CONCEPT_REQUIRES_(Integral<D>() && Iterable<Rng>())>
            D operator()(Rng &&rng, D d = 0) const
            {
                static_assert(!is_infinite<Rng>::value,
                    "Trying to compute the length of an infinite range!");
                return this->impl_r(rng, d, sized_iterable_concept_t<Rng>{});
            }
        };

        RANGES_CONSTEXPR distance_fn distance {};
    }
}

#endif
