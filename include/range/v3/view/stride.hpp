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

#ifndef RANGES_V3_VIEW_STRIDE_HPP
#define RANGES_V3_VIEW_STRIDE_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/next_prev.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputRange>
        struct stride_range_view
        {
        private:
            using difference_type = range_difference_t<InputRange const>;
            InputRange rng_;
            difference_type stride_;

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<detail::add_const_if_t<InputRange, Const>>
                  , range_category_t<detail::add_const_if_t<InputRange, Const>>
                  , range_reference_t<detail::add_const_if_t<InputRange, Const>>
                  , range_difference_t<detail::add_const_if_t<InputRange, Const>>
                >
            {
            private:
                friend struct stride_range_view;
                friend struct ranges::iterator_core_access;
                using stride_range_view_ = detail::add_const_if_t<stride_range_view, Const>;
                using base_range =
                    detail::add_const_if_t<InputRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;

                stride_range_view_ *rng_;
                base_range_iterator it_;

                basic_iterator(stride_range_view_ &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {}
                void increment()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    detail::advance_bounded(it_, rng_->stride_, ranges::end(rng_->rng_));
                }
                bool equal(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                range_reference_t<base_range> dereference() const
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    return *it_;
                }
                void decrement()
                {
                    RANGES_ASSERT(it_ != ranges::begin(rng_->rng_));
                    detail::advance_bounded(it_, -rng_->stride_, ranges::begin(rng_->rng_));
                }
                range_difference_t<InputRange> distance_to(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    range_difference_t<InputRange> n = that.it_ - it_,
                                                   e = rng_->stride_ - 1;
                    RANGES_ASSERT(it_ == ranges::end(rng_->rng_) ||
                                  that.it_ == ranges::end(rng_->rng_) ||
                                  0 == n % rng_->stride_);
                    return (n + (0 < n ? e : -e)) / rng_->stride_;
                }
                void advance(range_difference_t<InputRange> n)
                {
                    detail::advance_bounded(it_, n * rng_->stride_,
                        0 < n ? ranges::end(rng_->rng_) : ranges::begin(rng_->rng_));
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

            stride_range_view(InputRange && rng, difference_type n)
              : rng_(std::forward<InputRange>(rng))
              , stride_(n)
            {
                RANGES_ASSERT(stride_ > 0);
            }
            iterator begin()
            {
                return {*this, ranges::begin(rng_)};
            }
            iterator end()
            {
                return {*this, ranges::end(rng_)};
            }
            const_iterator begin() const
            {
                return {*this, ranges::begin(rng_)};
            }
            const_iterator end() const
            {
                return {*this, ranges::end(rng_)};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            InputRange & base()
            {
                return rng_;
            }
            InputRange const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct strider : bindable<strider>
            {
                template<typename InputRange>
                static stride_range_view<InputRange>
                invoke(strider, InputRange && rng, range_difference_t<InputRange> step)
                {
                    CONCEPT_ASSERT(ranges::InputRange<InputRange>());
                    return {std::forward<InputRange>(rng), step};
                }

                template<typename Difference>
                static auto invoke(strider stride, Difference step)
                    -> decltype(stride(std::placeholders::_1, std::move(step)))
                {
                    CONCEPT_ASSERT(ranges::Integral<Difference>());
                    return stride(std::placeholders::_1, std::move(step));
                }
            };

            RANGES_CONSTEXPR strider stride{};
        }
    }
}

#endif
