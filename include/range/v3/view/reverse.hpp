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
#include <type_traits>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename BidirectionalRange>
        struct reverse_range_view
        {
        private:
            BidirectionalRange rng_;

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<detail::add_const_if_t<BidirectionalRange, Const>>
                  , range_category_t<detail::add_const_if_t<BidirectionalRange, Const>>
                  , range_reference_t<detail::add_const_if_t<BidirectionalRange, Const>>
                  , range_difference_t<detail::add_const_if_t<BidirectionalRange, Const>>
                >
            {
            private:
                friend struct reverse_range_view;
                friend struct ranges::iterator_core_access;
                using base_range = detail::add_const_if_t<BidirectionalRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;
                using reverse_range_view_ = detail::add_const_if_t<reverse_range_view, Const>;

                reverse_range_view_ *rng_;
                base_range_iterator it_;

                basic_iterator(reverse_range_view_ &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {}
                void increment()
                {
                    RANGES_ASSERT(it_ != ranges::begin(rng_->rng_));
                    --it_;
                }
                void decrement()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    ++it_;
                }
                void advance(range_difference_t<base_range> n)
                {
                    it_ -= n;
                }
                range_difference_t<base_range> distance_to(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ - that.it_;
                }
                bool equal(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                range_reference_t<base_range> dereference() const
                {
                    RANGES_ASSERT(it_ != ranges::begin(rng_->rng_));
                    return *std::prev(it_);
                }
            public:
                basic_iterator()
                  : rng_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst,
                         typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : rng_(that.rng_), it_(std::move(that).it_)
                {}
            };

        public:
            using iterator       = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;

            explicit reverse_range_view(BidirectionalRange && rng)
              : rng_(std::forward<BidirectionalRange>(rng))
            {}
            iterator begin()
            {
                return {*this, ranges::end(rng_)};
            }
            iterator end()
            {
                return {*this, ranges::begin(rng_)};
            }
            const_iterator begin() const
            {
                return {*this, ranges::end(rng_)};
            }
            const_iterator end() const
            {
                return {*this, ranges::begin(rng_)};
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
