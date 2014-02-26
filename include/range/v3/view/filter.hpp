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

#ifndef RANGES_V3_VIEW_FILTER_HPP
#define RANGES_V3_VIEW_FILTER_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputRange, typename UnaryPredicate>
        struct filter_range_view
          : range_facade<filter_range_view<InputRange, UnaryPredicate>,
                         is_infinite<InputRange>::value>
        {
        private:
            friend range_core_access;
            compressed_pair<InputRange, invokable_t<UnaryPredicate>> rng_and_pred_;

            template<bool Const>
            struct basic_impl
            {
                using base_range = detail::add_const_if_t<InputRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;
                using filter_range_view_ = detail::add_const_if_t<filter_range_view, Const>;

                filter_range_view_ *rng_;
                base_range_iterator it_;

                // TODO use this in range_facade
                using difference_type = range_difference_t<InputRange>;

                constexpr basic_impl()
                  : rng_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, enable_if_t<!OtherConst> = 0>
                basic_impl(basic_impl<OtherConst> that)
                  : rng_(that.rng_), it_(std::move(that).it_)
                {}
                basic_impl(filter_range_view_ &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {
                    satisfy();
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
                void next()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->base()));
                    ++it_;
                    satisfy();
                }
                CONCEPT_REQUIRES(BidirectionalIterable<InputRange>())
                void prev()
                {
                    while(!rng_->rng_and_pred_.second()(*--it_))
                        ;
                }
                void satisfy()
                {
                    auto const e = ranges::end(rng_->base());
                    while(it_ != e && !rng_->rng_and_pred_.second()(*it_))
                        ++it_;
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
            filter_range_view(InputRange && rng, UnaryPredicate pred)
              : rng_and_pred_{std::forward<InputRange>(rng), make_invokable(std::move(pred))}
            {}
            InputRange & base()
            {
                return rng_and_pred_.first();
            }
            InputRange const & base() const
            {
                return rng_and_pred_.first();
            }
        };

        namespace view
        {
            struct filterer : bindable<filterer>
            {
            private:
                template<typename UnaryPredicate>
                struct filterer1 : pipeable<filterer1<UnaryPredicate>>
                {
                private:
                    UnaryPredicate pred_;
                public:
                    filterer1(UnaryPredicate pred)
                      : pred_(std::move(pred))
                    {}
                    template<typename InputRange, typename This>
                    static filter_range_view<InputRange, UnaryPredicate> pipe(InputRange && rng, This && this_)
                    {
                        return {std::forward<InputRange>(rng), std::forward<This>(this_).pred_};
                    }
                };
            public:
                template<typename InputRange, typename UnaryPredicate>
                static filter_range_view<InputRange, UnaryPredicate>
                invoke(filterer, InputRange && rng, UnaryPredicate pred)
                {
                    return {std::forward<InputRange>(rng), std::move(pred)};
                }
                template<typename UnaryPredicate>
                static filterer1<UnaryPredicate> invoke(filterer, UnaryPredicate pred)
                {
                    return {std::move(pred)};
                }
            };

            RANGES_CONSTEXPR filterer filter {};
        }
    }
}

#endif
