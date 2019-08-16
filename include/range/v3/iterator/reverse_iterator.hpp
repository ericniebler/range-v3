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

#include <range/v3/iterator/basic_iterator.hpp>
#include <range/v3/iterator/concepts.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{

    /// \cond
    namespace detail
    {
        template<typename I>
        struct reverse_cursor
        {
        private:
            CPP_assert(bidirectional_iterator<I>);
            friend range_access;
            template<typename OtherI>
            friend struct reverse_cursor;
            struct mixin : basic_mixin<reverse_cursor>
            {
                mixin() = default;
                using basic_mixin<reverse_cursor>::basic_mixin;
                constexpr mixin(I it)
                  : mixin{reverse_cursor{it}}
                {}
                constexpr I base() const
                {
                    return this->get().base();
                }
            };

            I it_;

            constexpr reverse_cursor(I it)
              : it_(std::move(it))
            {}
            constexpr auto read() const -> iter_reference_t<I>
            {
                return *arrow();
            }
            constexpr I arrow() const
            {
                auto tmp = it_;
                --tmp;
                return tmp;
            }
            constexpr I base() const
            {
                return it_;
            }
            template<typename J>
            constexpr auto equal(reverse_cursor<J> const & that) const
                -> CPP_ret(bool)( //
                    requires sentinel_for<J, I>)
            {
                return it_ == that.it_;
            }
            constexpr void next()
            {
                --it_;
            }
            constexpr void prev()
            {
                ++it_;
            }
            CPP_member
            constexpr auto advance(iter_difference_t<I> n) -> CPP_ret(void)( //
                requires random_access_iterator<I>)
            {
                it_ -= n;
            }
            template<typename J>
            constexpr auto distance_to(reverse_cursor<J> const & that) const
                -> CPP_ret(iter_difference_t<I>)( //
                    requires sized_sentinel_for<J, I>)
            {
                return it_ - that.base();
            }
            constexpr iter_rvalue_reference_t<I> move() const
                noexcept(noexcept((void)I(I(it_)), (void)--const_cast<I &>(it_),
                                  iter_move(it_)))
            {
                auto tmp = it_;
                --tmp;
                return iter_move(tmp);
            }

        public:
            reverse_cursor() = default;
            template<typename U>
            constexpr CPP_ctor(reverse_cursor)(reverse_cursor<U> const & u)( //
                requires convertible_to<U, I>)
              : it_(u.base())
            {}
        };
    } // namespace detail
    /// \endcond

    struct make_reverse_iterator_fn
    {
        template<typename I>
        constexpr auto operator()(I i) const -> CPP_ret(reverse_iterator<I>)( //
            requires bidirectional_iterator<I>)
        {
            return reverse_iterator<I>(i);
        }
    };

    RANGES_INLINE_VARIABLE(make_reverse_iterator_fn, make_reverse_iterator)

    namespace cpp20
    {
        using ranges::make_reverse_iterator;
        using ranges::reverse_iterator;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif // RANGES_V3_ITERATOR_REVERSE_ITERATOR_HPP
