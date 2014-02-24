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
#include <range/v3/range_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename ForwardRange, typename BinaryPredicate>
        struct adjacent_filter_range_view
            : range_facade<adjacent_filter_range_view<ForwardRange, BinaryPredicate>>
        {
        private:
            friend struct range_core_access;
            compressed_pair<ForwardRange, invokable_t<BinaryPredicate>> rng_and_pred_;

            template<bool Const>
            struct basic_impl
            {
                using base_range = detail::add_const_if_t<ForwardRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;
                using adjacent_filter_range_view_ =
                    detail::add_const_if_t<adjacent_filter_range_view, Const>;

                adjacent_filter_range_view_ *rng_;
                base_range_iterator it_;

                constexpr basic_impl()
                  : rng_{}, it_{}
                {}
                basic_impl(adjacent_filter_range_view_ &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_impl(basic_impl<OtherConst> that)
                  : rng_(that.rng_), it_(std::move(that).it_)
                {}
                void next()
                {
                    auto const e = ranges::end(rng_->base());
                    RANGES_ASSERT(it_ != e);
                    auto &pred = rng_->rng_and_pred_.second();
                    auto const prev = it_++;
                    for(; it_ != e && !pred(*prev, *it_); ++it_)
                        ;
                }
                template<bool OtherConst>
                bool equal(basic_impl<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                auto current() const -> decltype(*it_)
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->base()));
                    return *it_;
                }
            };
            basic_impl<false> begin_impl()
            {
                return {*this, ranges::begin(base())};
            }
            basic_impl<true> begin_impl() const
            {
                return {*this, ranges::begin(base())};
            }
            basic_impl<false> end_impl()
            {
                return {*this, ranges::end(base())};
            }
            basic_impl<true> end_impl() const
            {
                return {*this, ranges::end(base())};
            }
        public:
            adjacent_filter_range_view(ForwardRange && rng, BinaryPredicate pred)
              : rng_and_pred_{std::forward<ForwardRange>(rng),
                              make_invokable(std::move(pred))}
            {}
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
                static auto invoke(adjacent_filterer adjacent_filter, BinaryPredicate pred) ->
                    decltype(adjacent_filter.move_bind(std::placeholders::_1, std::move(pred)))
                {
                    return adjacent_filter.move_bind(std::placeholders::_1, std::move(pred));
                }
            };

            RANGES_CONSTEXPR adjacent_filterer adjacent_filter {};
        }
    }
}

#endif
