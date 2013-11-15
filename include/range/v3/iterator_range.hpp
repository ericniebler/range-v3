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

#ifndef RANGE_V3_ITERATOR_RANGE_HPP
#define RANGE_V3_ITERATOR_RANGE_HPP

#include <utility>
#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>

namespace range
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
              : begin_(std::move(begin)), end_(std::move(end))
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
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
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

        constexpr struct iterator_range_maker
        {
            template<typename Iter>
            constexpr iterator_range<Iter> operator()(Iter begin, Iter end) const
            {
                return {std::move(begin), std::move(end)};
            }
        } make_iterator_range {};
    }
}

#endif
