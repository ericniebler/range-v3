// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_ITERATOR_RANGE_HPP
#define RANGES_V3_ITERATOR_RANGE_HPP

#include <utility>
#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Intentionally resisting the urge to fatten this interface to make
        // it look like a container, like iterator_range. It's a range,
        // not a container.
        template<typename Iter>
        struct iterator_range
        {
        private:
            Iter begin_, end_;
        public:
            using iterator = Iter;
            using const_iterator = Iter;

            iterator_range() = default;
            constexpr iterator_range(Iter begin, Iter end)
              : begin_(detail::move(begin)), end_(detail::move(end))
            {}
            constexpr iterator_range(std::pair<Iter, Iter> rng)
              : begin_(detail::move(rng.first)), end_(detail::move(rng.second))
            {}
            iterator begin() const
            {
                return begin_;
            }
            iterator end() const
            {
                return end_;
            }
            bool operator!() const
            {
                return begin_ == end_;
            }
            explicit operator bool() const
            {
                return begin_ != end_;
            }
            iterator_range & advance_begin(typename std::iterator_traits<Iter>::difference_type n)
            {
                std::advance(begin_, n);
                return *this;
            }
            iterator_range & advance_end(typename std::iterator_traits<Iter>::difference_type n)
            {
                std::advance(end_, n);
                return *this;
            }
        };

        struct ranger : bindable<ranger>
        {
            template<typename Iter>
            static iterator_range<Iter> invoke(ranger, Iter begin, Iter end)
            {
                return {detail::move(begin), detail::move(end)};
            }
        };

        constexpr ranger range {};
    }
}

#endif
