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
#ifndef RANGES_V3_RANGE_FACADE_HPP
#define RANGES_V3_RANGE_FACADE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_access.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/basic_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Derived>
            using facade_cursor_t =
                decltype(range_access::begin_cursor(std::declval<Derived &>()));

            template<typename Derived>
            using facade_sentinel2_t =
                decltype(range_access::end_cursor(std::declval<Derived &>()));

            template<typename Derived>
            using facade_iterator_t =
                basic_iterator<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>;

            template<typename Derived>
            using facade_sentinel_t =
                conditional_t<
                    (Same<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>()),
                    basic_iterator<facade_cursor_t<Derived>, facade_sentinel2_t<Derived>>,
                    basic_sentinel<facade_sentinel2_t<Derived>>>;
        }

        struct default_sentinel
        {
            template<typename Cur>
            static constexpr bool equal(Cur const &pos)
            {
                return range_access::done(pos);
            }
        };

        template<typename Derived, bool Infinite>
        struct range_facade
          : private detail::is_infinite<Infinite>
          , private range_base
        {
        private:
            friend Derived;
            friend range_access;
            using range_facade_t = range_facade;
        protected:
            Derived & derived()
            {
                return static_cast<Derived &>(*this);
            }
            Derived const & derived() const
            {
                return static_cast<Derived const &>(*this);
            }
            // Default implementations
            Derived begin_cursor() const
            {
                return derived();
            }
            default_sentinel end_cursor() const
            {
                return {};
            }
        public:
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D> begin()
            {
                return {range_access::begin_cursor(derived())};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D> end()
            {
                return {range_access::end_cursor(derived())};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D const> begin() const
            {
                return {range_access::begin_cursor(derived())};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D const> end() const
            {
                return {range_access::end_cursor(derived())};
            }
            bool empty() const
            {
                return begin() == end();
            }
            bool operator!() const
            {
                return empty();
            }
            explicit operator bool() const
            {
                return !empty();
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() &&
                    SizedIteratorRange<range_iterator_t<D>, range_sentinel_t<D>>())>
            range_size_t<D> size() const
            {
                return iterator_range_size(begin(), end());
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            range_reference_t<D> front()
            {
                return *begin();
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>())>
            range_reference_t<D const> front() const
            {
                return *begin();
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && BoundedRange<D>() && BidirectionalRange<D>())>
            range_reference_t<D> back()
            {
                return *prev(end());
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && BoundedRange<D const>() && BidirectionalRange<D const>())>
            range_reference_t<D const> back() const
            {
                return *prev(end());
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && RandomAccessRange<D>())>
            auto operator[](range_difference_t<D> n) ->
                decltype(std::declval<D &>().begin()[n])
            {
                return begin()[n];
            }
            template<typename D = Derived,
                CONCEPT_REQUIRES_(Same<D, Derived>() && RandomAccessRange<D const>())>
            auto operator[](range_difference_t<D> n) const ->
                decltype(std::declval<D const &>().begin()[n])
            {
                return begin()[n];
            }
        };

        template<typename RangeFacade>
        using range_facade_t = meta_apply<range_access::range_facade, RangeFacade>;
    }
}

#endif
