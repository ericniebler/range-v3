// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_DELIMIT_HPP
#define RANGES_V3_VIEW_DELIMIT_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename InputIterable, typename Value>
        struct delimit_iterable_view
          : range_facade<delimit_iterable_view<InputIterable, Value>>
        {
        private:
            friend struct range_facade<delimit_iterable_view>;
            InputIterable rng_;
            Value value_;

            template<bool Const>
            struct basic_impl
            {
                using base_range = detail::add_const_if_t<InputIterable, Const>;
                using base_range_iterator = range_iterator_t<base_range>;
                using delimit_iterable_view_ = detail::add_const_if_t<delimit_iterable_view, Const>;

                delimit_iterable_view_ *rng_;
                base_range_iterator it_;

                constexpr basic_impl()
                  : rng_{}, it_{}
                {}
                basic_impl(delimit_iterable_view_ &rng)
                  : rng_(&rng), it_(ranges::begin(rng.rng_))
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, typename std::enable_if<!OtherConst, int>::type = 0>
                basic_impl(basic_impl<OtherConst> that)
                  : rng_(that.rng_), it_(std::move(that).it_)
                {}
                bool done() const
                {
                    return it_ == ranges::end(rng_->rng_) ||
                          *it_ == rng_->value_;
                }
                template<bool OtherConst>
                bool equal(basic_impl<OtherConst> const &that) const
                {
                    RANGES_ASSERT(rng_ == that.rng_);
                    return it_ == that.it_;
                }
                auto current() const -> decltype(*it_)
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    return *it_;
                }
                void next()
                {
                    RANGES_ASSERT(it_ != ranges::end(rng_->rng_));
                    ++it_;
                }
            };
            basic_impl<false> begin_impl()
            {
                return {*this};
            }
            basic_impl<true> begin_impl() const
            {
                return {*this};
            }
        public:
            delimit_iterable_view(InputIterable && rng, Value value)
              : rng_(std::forward<InputIterable>(rng)), value_(std::move(value))
            {}
            InputIterable & base()
            {
                return rng_;
            }
            InputIterable const & base() const
            {
                return rng_;
            }
        };

        namespace view
        {
            struct delimiter : bindable<delimiter>
            {
                template<typename InputIterable, typename Value>
                static delimit_iterable_view<InputIterable, Value>
                invoke(delimiter, InputIterable && rng, Value value)
                {
                    return {std::forward<InputIterable>(rng), std::move(value)};
                }

                template<typename Value>
                static auto
                invoke(delimiter delimit, Value value) ->
                    decltype(delimit.move_bind(std::placeholders::_1, std::move(value)))
                {
                    return delimit.move_bind(std::placeholders::_1, std::move(value));
                }
            };

            RANGES_CONSTEXPR delimiter delimit{};
        }
    }
}

#endif
