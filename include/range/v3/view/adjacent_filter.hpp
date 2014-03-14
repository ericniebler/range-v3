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
          : range_facade<adjacent_filter_range_view<ForwardRange, BinaryPredicate>,
                         is_infinite<ForwardRange>::value>
        {
        private:
            friend range_core_access;
            compressed_pair<ForwardRange, invokable_t<BinaryPredicate>> rng_and_pred_;

            struct impl
            {
                using base_range = ForwardRange;
                using base_range_iterator = range_iterator_t<base_range>;

                adjacent_filter_range_view const *rng_;
                base_range_iterator it_;

                constexpr impl() = default;
                impl(adjacent_filter_range_view const &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
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
                bool equal(impl const &that) const
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
            impl begin_impl() const
            {
                return {*this, ranges::begin(base())};
            }
            impl end_impl() const
            {
                return {*this, ranges::end(base())};
            }
        public:
            adjacent_filter_range_view(ForwardRange && rng, BinaryPredicate pred)
              : rng_and_pred_{std::forward<ForwardRange>(rng), make_invokable(std::move(pred))}
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
                    CONCEPT_ASSERT(ranges::Range<ForwardRange>());
                    CONCEPT_ASSERT(ranges::ForwardIterator<range_iterator_t<ForwardRange>>());
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
