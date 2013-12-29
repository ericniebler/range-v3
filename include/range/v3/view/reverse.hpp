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

#ifndef RANGES_V3_VIEW_REVERSE_HPP
#define RANGES_V3_VIEW_REVERSE_HPP

#include <utility>
#include <iterator>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/debug_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct reverse_iterator_maker : bindable<reverse_iterator_maker>
        {
            template<typename Iterator>
            static std::reverse_iterator<Iterator>
            invoke(reverse_iterator_maker, Iterator it)
            {
                return std::reverse_iterator<Iterator>{std::move(it)};
            }
        };

        RANGES_CONSTEXPR reverse_iterator_maker make_reverse_iterator {};

        template<typename BidirectionalRange>
        struct reverse_range_view
        {
        private:
            BidirectionalRange rng_;

        public:
            using iterator =
                RANGES_DEBUG_ITERATOR(reverse_range_view,
                    std::reverse_iterator<range_iterator_t<BidirectionalRange>>);

            using const_iterator =
                RANGES_DEBUG_ITERATOR(reverse_range_view const,
                    std::reverse_iterator<range_iterator_t<BidirectionalRange const>>);

            explicit reverse_range_view(BidirectionalRange && rng)
              : rng_(std::forward<BidirectionalRange>(rng))
            {}
            iterator begin()
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    ranges::make_reverse_iterator(ranges::end(rng_)));
            }
            iterator end()
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    ranges::make_reverse_iterator(ranges::begin(rng_)));
            }
            const_iterator begin() const
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    ranges::make_reverse_iterator(ranges::end(rng_)));
            }
            const_iterator end() const
            {
                return RANGES_MAKE_DEBUG_ITERATOR(*this,
                    ranges::make_reverse_iterator(ranges::begin(rng_)));
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            BidirectionalRange & base()
            {
                return rng_;
            }
            BidirectionalRange const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct reverser : bindable<reverser>, pipeable<reverser>
            {
                template<typename BidirectionalRange>
                static reverse_range_view<BidirectionalRange>
                invoke(reverser, BidirectionalRange && rng)
                {
                    CONCEPT_ASSERT(ranges::BidirectionalRange<BidirectionalRange>());
                    return reverse_range_view<BidirectionalRange>{
                        std::forward<BidirectionalRange>(rng)};
                }
            };

            RANGES_CONSTEXPR reverser reverse {};
        }
    }
}

#endif
