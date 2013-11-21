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

#ifndef RANGES_V3_ADAPTOR_FILTER_HPP
#define RANGES_V3_ADAPTOR_FILTER_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <range/v3/utility/iterator_facade.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/detail/function_wrapper.hpp>
#include <range/v3/detail/compressed_pair.hpp>
#include <range/v3/utility/bindable.hpp>

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

        template<typename Rng, typename Pred>
        struct filter_range
        {
        private:
            detail::compressed_pair<Rng, detail::function_wrapper<Pred>> rng_and_pred_;

            // FltRng is either filter_range or filter_range const.
            template<typename FltRng>
            struct basic_iterator
              : ranges::iterator_facade<
                    basic_iterator<FltRng>
                  , range_value_t<Rng>
                  , decltype(detail::filter_range_category(range_category_t<Rng>{}))
                  , decltype(*ranges::begin(std::declval<FltRng &>().rng_and_pred_.first()))
                >
            {
            private:
                friend struct filter_range;
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
            using iterator       = basic_iterator<filter_range>;
            using const_iterator = basic_iterator<filter_range const>;

            filter_range(Rng && rng, Pred pred)
              : rng_and_pred_{detail::forward<Rng>(rng), detail::move(pred)}
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
            Rng & base()
            {
                return rng_and_pred_.first();
            }
            Rng const & base() const
            {
                return rng_and_pred_.first();
            }
        };

        struct filterer
        {
        private:
            template<typename Pred>
            class filterer1
            {
                Pred pred_;
            public:
                filterer1(Pred pred)
                  : pred_(detail::move(pred))
                {}
                template<typename Rng>
                filter_range<Rng, Pred> operator()(Rng && rng) &&
                {
                    return {detail::forward<Rng>(rng), detail::move(*this).pred_};
                }
                template<typename Rng>
                filter_range<Rng, Pred> operator()(Rng && rng) const &
                {
                    return {detail::forward<Rng>(rng), pred_};
                }
            };
        public:
            template<typename Rng, typename Pred>
            filter_range<Rng, Pred> operator()(Rng && rng, Pred pred) const
            {
                return {detail::forward<Rng>(rng), detail::move(pred)};
            }
            template<typename Pred>
            constexpr bindable<filterer1<Pred>> operator()(Pred pred) const
            {
                return bindable<filterer1<Pred>>{filterer1<Pred>{pred}};
            }
        };
        
        constexpr bindable<filterer> filter {};
    }
}

#endif
