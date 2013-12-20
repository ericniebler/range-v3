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
        namespace detail
        {
            auto filter_range_category(std::input_iterator_tag)           -> std::input_iterator_tag;
            auto filter_range_category(std::forward_iterator_tag)         -> std::forward_iterator_tag;
            auto filter_range_category(std::bidirectional_iterator_tag)   -> std::bidirectional_iterator_tag;
        }

        template<typename InputRange, typename UnaryPredicate>
        struct filter_range_view
        {
        private:
            compressed_pair<InputRange, invokable_t<UnaryPredicate>> rng_and_pred_;

            // FltRng is either filter_range or filter_range const.
            template<typename FltRng>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<FltRng>
                  , range_value_t<InputRange>
                  , decltype(detail::filter_range_category(range_category_t<InputRange>{}))
                  , decltype(*ranges::begin(std::declval<FltRng &>().rng_and_pred_.first()))
                >
            {
            private:
                friend struct filter_range_view;
                friend struct ranges::iterator_core_access;
                using base_range_iterator =
                    decltype(ranges::begin(std::declval<FltRng &>().rng_and_pred_.first()));

                FltRng *rng_;
                base_range_iterator it_;

                basic_iterator(FltRng &rng, base_range_iterator it)
                  : rng_(&rng), it_(detail::move(it))
                {
                    satisfy();
                }
                void increment()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_and_pred_.first()));
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
                auto dereference() const -> decltype(*it_)
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_and_pred_.first()));
                    return *it_;
                }
                void satisfy()
                {
                    auto const e = ranges::end(rng_->rng_and_pred_.first());
                    while(it_ != e && !rng_->rng_and_pred_.second()(*it_))
                        ++it_;
                }
            public:
                basic_iterator()
                  : rng_{}, it_{}
                {}
                // For iterator -> const_iterator conversion
                template<typename OtherFltRng,
                         typename = typename std::enable_if<
                                        !std::is_const<OtherFltRng>::value>::type>
                basic_iterator(basic_iterator<OtherFltRng> that)
                  : rng_(that.rng_), it_(detail::move(that).it_)
                {}
            };
        public:
            using iterator       = basic_iterator<filter_range_view>;
            using const_iterator = basic_iterator<filter_range_view const>;

            filter_range_view(InputRange && rng, UnaryPredicate pred)
              : rng_and_pred_{detail::forward<InputRange>(rng), make_invokable(detail::move(pred))}
            {}
            iterator begin()
            {
                return {*this, ranges::begin(rng_and_pred_.first())};
            }
            iterator end()
            {
                return {*this, ranges::end(rng_and_pred_.first())};
            }
            const_iterator begin() const
            {
                return {*this, ranges::begin(rng_and_pred_.first())};
            }
            const_iterator end() const
            {
                return {*this, ranges::end(rng_and_pred_.first())};
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
                      : pred_(detail::move(pred))
                    {}
                    template<typename InputRange, typename This>
                    static filter_range_view<InputRange, UnaryPredicate> pipe(InputRange && rng, This && this_)
                    {
                        return {detail::forward<InputRange>(rng), detail::forward<This>(this_).pred_};
                    }
                };
            public:
                template<typename InputRange, typename UnaryPredicate>
                static filter_range_view<InputRange, UnaryPredicate>
                invoke(filterer, InputRange && rng, UnaryPredicate pred)
                {
                    return {detail::forward<InputRange>(rng), detail::move(pred)};
                }
                template<typename UnaryPredicate>
                static filterer1<UnaryPredicate> invoke(filterer, UnaryPredicate pred)
                {
                    return {detail::move(pred)};
                }
            };

            RANGES_CONSTEXPR filterer filter {};
        }
    }
}

#endif
