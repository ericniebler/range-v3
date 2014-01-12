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

#ifndef RANGES_V3_VIEW_ADJACENT_FILTER_HPP
#define RANGES_V3_VIEW_ADJACENT_FILTER_HPP

#include <utility>
#include <range/v3/next_prev.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/bindable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename ForwardRange, typename BinaryPredicate>
        struct adjacent_filter_range_view
        {
        private:
            compressed_pair<ForwardRange, invokable_t<BinaryPredicate>> rng_and_pred_;

            template<bool Const>
            struct basic_iterator
              : iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<ForwardRange>
                  , std::forward_iterator_tag
                  , range_reference_t<detail::add_const_if_t<ForwardRange, Const>>
                  , range_difference_t<ForwardRange>
                >
            {
            private:
                friend struct adjacent_filter_range_view;
                friend struct ranges::iterator_core_access;
                using base_range = detail::add_const_if_t<ForwardRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;
                using adjacent_filter_range_view_ =
                    detail::add_const_if_t<adjacent_filter_range_view, Const>;

                adjacent_filter_range_view_ *rng_;
                base_range_iterator it_;

                basic_iterator(adjacent_filter_range_view_ &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {}
                void increment()
                {
                    auto const e = ranges::end(rng_->base());
                    RANGES_ASSERT(it_ != e);
                    auto &pred = rng_->rng_and_pred_.second();
                    auto const prev = it_++;
                    for(; it_ != e && !pred(*prev, *it_); ++it_)
                        ;
                }
                template<bool OtherConst>
                bool equal(basic_iterator<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                typename basic_iterator::reference dereference() const
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->base()));
                    return *it_;
                }
            public:
                basic_iterator()
                  : rng_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_iterator(basic_iterator<OtherConst> that)
                  : rng_(that.rng_), it_(std::move(that).it_)
                {}
            };
        public:
            using iterator       = basic_iterator<false>;
            using const_iterator = basic_iterator<true>;

            adjacent_filter_range_view(ForwardRange && rng, BinaryPredicate pred)
              : rng_and_pred_{std::forward<ForwardRange>(rng),
                              make_invokable(std::move(pred))}
            {}
            iterator begin()
            {
                return {*this, ranges::begin(base())};
            }
            iterator end()
            {
                return {*this, ranges::end(base())};
            }
            const_iterator begin() const
            {
                return {*this, ranges::begin(base())};
            }
            const_iterator end() const
            {
                return {*this, ranges::end(base())};
            }
            bool operator!() const
            {
                return begin() == end();
            }
            explicit operator bool() const
            {
                return begin() != end();
            }
            ForwardRange & base()
            {
                return rng_and_pred_.first();
            }
            ForwardRange const & base() const
            {
                return rng_and_pred_.first();
            }
        };

        namespace view
        {
            struct adjacent_filterer : bindable<adjacent_filterer>
            {
                template<typename ForwardRange, typename BinaryPredicate>
                static adjacent_filter_range_view<ForwardRange, BinaryPredicate>
                invoke(adjacent_filterer, ForwardRange && rng, BinaryPredicate pred)
                {
                    CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                    CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                           range_reference_t<ForwardRange>,
                                                           range_reference_t<ForwardRange>>());
                    return {std::forward<ForwardRange>(rng), std::move(pred)};
                }
                template<typename BinaryPredicate>
                static auto invoke(adjacent_filterer adjacent_filter, BinaryPredicate pred)
                    -> decltype(adjacent_filter.move_bind(std::placeholders::_1, std::move(pred)))
                {
                    return adjacent_filter.move_bind(std::placeholders::_1, std::move(pred));
                }
            };

            RANGES_CONSTEXPR adjacent_filterer adjacent_filter {};
        }
    }
}

#endif
