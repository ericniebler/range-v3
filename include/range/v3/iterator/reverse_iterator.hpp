/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ITERATOR_REVERSE_ITERATOR_HPP
#define RANGES_V3_ITERATOR_REVERSE_ITERATOR_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/basic_iterator.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename I>
        struct reverse_cursor
        {
        private:
            CPP_assert(BidirectionalIterator<I>);
            friend range_access;
            template<typename OtherI>
            friend struct reverse_cursor;
            struct mixin
              : basic_mixin<reverse_cursor>
            {
                mixin() = default;
                using basic_mixin<reverse_cursor>::basic_mixin;
                constexpr /*c++14*/
                mixin(I it)
                  : mixin{reverse_cursor{it}}
                {}
                constexpr /*c++14*/
                I base() const
                {
                    return this->get().base();
                }
            };

            I it_;

            constexpr /*c++14*/
            reverse_cursor(I it)
              : it_(std::move(it))
            {}
            constexpr /*c++14*/
            auto read() const -> iter_reference_t<I>
            {
                return *arrow();
            }
            constexpr /*c++14*/
            I arrow() const
            {
                auto tmp = it_;
                --tmp;
                return tmp;
            }
            constexpr /*c++14*/
            I base() const
            {
                return it_;
            }
            CPP_template(typename J)(
                requires Sentinel<J, I>)
            constexpr /*c++14*/ bool equal(reverse_cursor<J> const& that) const
            {
                return it_ == that.it_;
            }
            constexpr /*c++14*/
            void next()
            {
                --it_;
            }
            constexpr /*c++14*/
            void prev()
            {
                ++it_;
            }
            CPP_member
            constexpr /*c++14*/
            auto advance(iter_difference_t<I> n) ->
                CPP_ret(void)(
                    requires RandomAccessIterator<I>)
            {
                it_ -= n;
            }
            CPP_template(typename J)(
                requires SizedSentinel<J, I>)
            constexpr /*c++14*/ iter_difference_t<I>
            distance_to(reverse_cursor<J> const &that) const
            {
                return it_ - that.base();
            }
            constexpr /*c++14*/
            iter_rvalue_reference_t<I> move() const
                noexcept(noexcept((void)I(I(it_)), (void)--const_cast<I &>(it_), iter_move(it_)))
            {
                auto tmp = it_;
                --tmp;
                return iter_move(tmp);
            }
        public:
            reverse_cursor() = default;
            template<typename U>
            constexpr /*c++14*/
            CPP_ctor(reverse_cursor)(reverse_cursor<U> const &u)(
                requires ConvertibleTo<U, I>)
              : it_(u.base())
            {}
        };
    }  // namespace detail
    /// \endcond

    struct make_reverse_iterator_fn
    {
        CPP_template(typename I)(
            requires BidirectionalIterator<I>)
        constexpr /*c++14*/ reverse_iterator<I> operator()(I i) const
        {
            return reverse_iterator<I>(i);
        }
    };

    RANGES_INLINE_VARIABLE(make_reverse_iterator_fn, make_reverse_iterator)
} // namespace ranges

#endif // RANGES_V3_ITERATOR_REVERSE_ITERATOR_HPP
