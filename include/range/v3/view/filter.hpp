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
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/compressed_pair.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputRange, typename UnaryPredicate>
        struct filter_range_view
        {
        private:
            compressed_pair<InputRange, invokable_t<UnaryPredicate>> rng_and_pred_;

            template<bool Const>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<Const>
                  , range_value_t<InputRange>
                  , decltype(true ? range_category_t<InputRange>{} : std::bidirectional_iterator_tag{})
                  , range_reference_t<detail::add_const_if_t<InputRange, Const>>
                  , range_difference_t<InputRange>
                >
            {
            private:
                friend struct filter_range_view;
                friend struct ranges::iterator_core_access;
                using base_range = detail::add_const_if_t<InputRange, Const>;
                using base_range_iterator = range_iterator_t<base_range>;
                using filter_range_view_ = detail::add_const_if_t<filter_range_view, Const>;

                filter_range_view_ *rng_;
                base_range_iterator it_;

                basic_iterator(filter_range_view_ &rng, base_range_iterator it)
                  : rng_(&rng), it_(std::move(it))
                {
                    satisfy();
                }
                void increment()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->base()));
                    ++it_; satisfy();
                }
                void decrement()
                {
                    while(!rng_->rng_and_pred_.second()(*--it_)) {}
                }
                bool equal(basic_iterator const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                range_reference_t<base_range> dereference() const
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->base()));
                    return *it_;
                }
                void satisfy()
                {
                    auto const e = ranges::end(rng_->base());
                    while(it_ != e && !rng_->rng_and_pred_.second()(*it_))
                        ++it_;
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

            filter_range_view(InputRange && rng, UnaryPredicate pred)
              : rng_and_pred_{std::forward<InputRange>(rng), make_invokable(std::move(pred))}
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
