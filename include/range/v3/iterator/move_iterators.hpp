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
#ifndef RANGES_V3_ITERATOR_MOVE_ITERATORS_HPP
#define RANGES_V3_ITERATOR_MOVE_ITERATORS_HPP

#include <cstddef>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/basic_iterator.hpp>
#include <range/v3/iterator/traits.hpp>

namespace ranges
{
    /// \addtogroup group-iterator
    /// @{
    template<typename I>
    struct move_iterator
    {
    private:
        CPP_assert(InputIterator<I>);
        I current_ = I{};
    public:
        using iterator_type = I;
        using difference_type = iter_difference_t<I>;
        using value_type = iter_value_t<I>;
        using iterator_category = std::input_iterator_tag;
        using reference = iter_rvalue_reference_t<I>;

        constexpr move_iterator() = default;
        explicit move_iterator(I i)
          : current_(i)
        {}
        template<typename O>
        CPP_ctor(move_iterator)(move_iterator<O> const &i)(
            requires ConvertibleTo<O, I>)
          : current_(i.base())
        {}
        template<typename O>
        auto operator=(move_iterator<O> const & i) ->
            CPP_ret(move_iterator &)(
                requires ConvertibleTo<O, I>)
        {
            current_ = i.base();
            return *this;
        }
        I base() const
        {
            return current_;
        }
        auto CPP_auto_fun(operator*)() (const)
        (
            return iter_move(current_)
        )
        move_iterator &operator++()
        {
            ++current_;
            return *this;
        }
        CPP_member
        auto operator++(int) ->
            CPP_ret(void)(
                requires (not ForwardIterator<I>))
        {
            ++current_;
        }
        CPP_member
        auto operator++(int) ->
            CPP_ret(move_iterator)(
                requires ForwardIterator<I>)
        {
            return move_iterator(current_++);
        }
        CPP_member
        auto operator--() ->
            CPP_ret(move_iterator &)(
                requires ForwardIterator<I>)
        {
            --current_;
            return *this;
        }
        CPP_member
        auto operator--(int) ->
            CPP_ret(move_iterator)(
                requires BidirectionalIterator<I>)
        {
            return move_iterator(current_--);
        }
        CPP_member
        auto operator+(difference_type n) const ->
            CPP_ret(move_iterator)(
                requires RandomAccessIterator<I>)
        {
            return move_iterator(current_ + n);
        }
        CPP_member
        auto operator+=(difference_type n) ->
            CPP_ret(move_iterator &)(
                requires RandomAccessIterator<I>)
        {
            current_ += n;
            return *this;
        }
        CPP_member
        auto operator-(difference_type n) const ->
            CPP_ret(move_iterator)(
                requires RandomAccessIterator<I>)
        {
            return move_iterator(current_ - n);
        }
        CPP_member
        auto operator-=(difference_type n) ->
            CPP_ret(move_iterator &)(
                requires RandomAccessIterator<I>)
        {
            current_ -= n;
            return *this;
        }
        CPP_member
        auto operator[](difference_type n) const ->
            CPP_ret(reference)(
                requires RandomAccessIterator<I>)
        {
            return iter_move(current_ + n);
        }

        template<typename I2>
        friend auto operator==(move_iterator const &x, move_iterator<I2> const &y) ->
            CPP_broken_friend_ret(bool)(
                requires EqualityComparableWith<I, I2>)
        {
            return x.base() == y.base();
        }
        template<typename I2>
        friend auto operator!=(move_iterator const &x, move_iterator<I2> const &y) ->
            CPP_broken_friend_ret(bool)(
                requires EqualityComparableWith<I, I2>)
        {
            return !(x == y);
        }
        template<typename I2>
        friend auto operator<(move_iterator const &x, move_iterator<I2> const &y) ->
            CPP_broken_friend_ret(bool)(
                requires StrictTotallyOrderedWith<I, I2>)
        {
            return x.base() < y.base();
        }
        template<typename I2>
        friend auto operator<=(move_iterator const &x, move_iterator<I2> const &y) ->
            CPP_broken_friend_ret(bool)(
                requires StrictTotallyOrderedWith<I, I2>)
        {
            return !(y < x);
        }
        template<typename I2>
        friend auto operator>(move_iterator const &x, move_iterator<I2> const &y) ->
            CPP_broken_friend_ret(bool)(
                requires StrictTotallyOrderedWith<I, I2>)
        {
            return y < x;
        }
        template<typename I2>
        friend auto operator>=(move_iterator const &x, move_iterator<I2> const &y) ->
            CPP_broken_friend_ret(bool)(
                requires StrictTotallyOrderedWith<I, I2>)
        {
            return !(x < y);
        }

        template<typename I2>
        friend auto operator-(move_iterator const &x, move_iterator<I2> const &y) ->
            CPP_broken_friend_ret(iter_difference_t<I2>)(
                requires SizedSentinel<I, I2>)
        {
            return x.base() - y.base();
        }
        CPP_broken_friend_member
        friend auto operator+(iter_difference_t<I> n, move_iterator const &x) ->
            CPP_broken_friend_ret(move_iterator)(
                requires RandomAccessIterator<I>)
        {
            return x + n;
        }
    };

    struct make_move_iterator_fn
    {
        template<typename I>
        constexpr auto operator()(I it) const ->
            CPP_ret(move_iterator<I>)(
                requires InputIterator<I>)
        {
            return move_iterator<I>{detail::move(it)};
        }
    };

    RANGES_INLINE_VARIABLE(make_move_iterator_fn, make_move_iterator)

    template<typename S>
    struct move_sentinel
    {
    private:
        S sent_;
    public:
        constexpr move_sentinel()
          : sent_{}
        {}
        constexpr explicit move_sentinel(S s)
          : sent_(detail::move(s))
        {}
        template<typename OS>
        constexpr explicit CPP_ctor(move_sentinel)(move_sentinel<OS> const &that)(
            requires ConvertibleTo<OS, S>)
          : sent_(that.base())
        {}
        template<typename OS>
        auto operator=(move_sentinel<OS> const &that) ->
            CPP_ret(move_sentinel &)(
                requires ConvertibleTo<OS, S>)
        {
            sent_ = that.base();
            return *this;
        }
        S base() const
        {
            return sent_;
        }

        template<typename I>
        friend auto operator==(move_iterator<I> const &i, move_sentinel const &s) ->
            CPP_broken_friend_ret(bool)(
                requires Sentinel<S, I>)
        {
            return i.base() == s.base();
        }
        template<typename I>
        friend auto operator==(move_sentinel const &s, move_iterator<I> const &i) ->
            CPP_broken_friend_ret(bool)(
                requires Sentinel<S, I>)
        {
            return s.base() == i.base();
        }
        template<typename I>
        friend auto operator!=(move_iterator<I> const &i, move_sentinel const &s) ->
            CPP_broken_friend_ret(bool)(
                requires Sentinel<S, I>)
        {
            return i.base() != s.base();
        }
        template<typename I>
        friend auto operator!=(move_sentinel const &s, move_iterator<I> const &i) ->
            CPP_broken_friend_ret(bool)(
                requires Sentinel<S, I>)
        {
            return s.base() != i.base();
        }
    };

    struct make_move_sentinel_fn
    {
        template<typename I>
        constexpr auto operator()(I i) const ->
            CPP_ret(move_iterator<I>)(
                requires InputIterator<I>)
        {
            return move_iterator<I>{detail::move(i)};
        }

        template<typename S>
        constexpr auto operator()(S s) const ->
            CPP_ret(move_sentinel<S>)(
                requires Semiregular<S> && (!InputIterator<S>))
        {
            return move_sentinel<S>{detail::move(s)};
        }
    };

    RANGES_INLINE_VARIABLE(make_move_sentinel_fn, make_move_sentinel)

    /// \cond
    namespace detail
    {
        template<typename I, bool IsReadable>
        struct move_into_cursor_types_
        {};

        template<typename I>
        struct move_into_cursor_types_<I, true>
        {
            using value_type = iter_value_t<I>;
            using single_pass = meta::bool_<(bool)SinglePass<I>>;
        };

        template<typename I>
        using move_into_cursor_types = move_into_cursor_types_<I, (bool) Readable<I>>;

        template<typename I>
        struct move_into_cursor
          : move_into_cursor_types<I>
        {
        private:
            friend range_access;
            struct mixin
              : basic_mixin<move_into_cursor>
            {
                mixin() = default;
                using basic_mixin<move_into_cursor>::basic_mixin;
                explicit mixin(I it)
                  : mixin{move_into_cursor{std::move(it)}}
                {}
                I base() const
                {
                    return this->get().it_;
                }
            };

            I it_ = I();

            explicit move_into_cursor(I it)
              : it_(std::move(it))
            {}
            void next()
            {
                ++it_;
            }
            template<typename T>
            auto write(T &&t) noexcept(noexcept(*it_ = std::move(t))) ->
                CPP_ret(void)(
                    requires Writable<I, aux::move_t<T>>)
            {
                *it_ = std::move(t);
            }
            template<typename T>
            auto write(T &&t) const noexcept(noexcept(*it_ = std::move(t))) ->
                CPP_ret(void)(
                    requires Writable<I, aux::move_t<T>>)
            {
                *it_ = std::move(t);
            }
            CPP_member
            auto read() const
                noexcept(noexcept(*std::declval<I const&>())) ->
                CPP_ret(iter_reference_t<I>)(
                    requires Readable<I>)
            {
                return *it_;
            }
            CPP_member
            auto equal(move_into_cursor const &that) const ->
                CPP_ret(bool)(
                    requires InputIterator<I>)
            {
                return it_ == that.it_;
            }
            CPP_member
            auto prev() ->
                CPP_ret(void)(
                    requires BidirectionalIterator<I>)
            {
                --it_;
            }
            CPP_member
            auto advance(iter_difference_t<I> n) ->
                CPP_ret(void)(
                    requires RandomAccessIterator<I>)
            {
                it_ += n;
            }
            CPP_member
            auto distance_to(move_into_cursor const &that) const ->
                CPP_ret(iter_difference_t<I>)(
                    requires SizedSentinel<I, I>)
            {
                return that.it_ - it_;
            }
            template<typename II = I const>
            constexpr /*c++14*/
            auto move() const noexcept(has_nothrow_iter_move_v<II>) ->
                CPP_ret(iter_rvalue_reference_t<II>)(
                    requires Same<I const, II> && Readable<II>)
            {
                return iter_move(it_);
            }
        public:
            constexpr move_into_cursor() = default;
        };
    }
    /// \endcond

    struct move_into_fn
    {
        template<typename I>
        constexpr move_into_iterator<I> operator()(I it) const
        {
            return move_into_iterator<I>{std::move(it)};
        }
    };

    /// \sa `move_into_fn`
    RANGES_INLINE_VARIABLE(move_into_fn, move_into)

    namespace cpp20
    {
        using ranges::move_iterator;
        using ranges::make_move_iterator;
        using ranges::move_sentinel;
    }
    /// @}
} // namespace ranges

/// \cond
namespace std
{
    template<typename I>
    struct iterator_traits< ::ranges::move_iterator<I>>
    {
        using iterator_category = std::input_iterator_tag;
        using difference_type = typename ::ranges::move_iterator<I>::difference_type;
        using value_type = typename ::ranges::move_iterator<I>::value_type;
        using reference = typename ::ranges::move_iterator<I>::reference;
        using pointer = meta::_t<std::add_pointer<reference>>;
    };
}
/// \endcond

#endif // RANGES_V3_ITERATOR_MOVE_ITERATORS_HPP
