// Range v3 library
//
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_TEST_DEBUG_VIEW_HPP
#define RANGES_TEST_DEBUG_VIEW_HPP

#include <cstddef>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/swap.hpp>

template<typename T, bool Sized = true>
struct debug_input_view
{
    static_assert(std::is_object<T>::value, "");

    using index_t = std::ptrdiff_t;
    using version_t = unsigned long;

    mutable T *data_ = nullptr;
    mutable T *last_ = nullptr;
    mutable version_t version_ = 0;
    mutable bool valid_ = false;
    bool begin_called_ = false;

    debug_input_view() = default;
    constexpr debug_input_view(T* data, index_t size) noexcept
      : data_{data}
      , last_{(RANGES_ENSURE(data || !size), data + size)}
      , valid_{true}
    {}
    template<index_t N>
    constexpr debug_input_view(T (&data)[N]) noexcept
      : debug_input_view{data, N}
    {}
    constexpr /*c++14*/
    debug_input_view(debug_input_view const &that) noexcept
      : data_{ranges::exchange(that.data_, nullptr)}
      , last_{ranges::exchange(that.last_, nullptr)}
      , valid_{ranges::exchange(that.valid_, false)}
    {
        ++that.version_;
        if (that.begin_called_)
        {
            data_ = last_ = nullptr;
            valid_ = false;
        }
    }
    constexpr /*c++14*/
    debug_input_view &operator=(debug_input_view const &that) noexcept
    {
        ++that.version_;
        data_ = ranges::exchange(that.data_, nullptr);
        last_ = ranges::exchange(that.last_, nullptr);
        valid_ = ranges::exchange(that.valid_, false);
        begin_called_ = false;
        if (that.begin_called_)
        {
            data_ = last_ = nullptr;
            valid_ = false;
        }
        ++version_;
        return *this;
    }

    struct sentinel
    {
        debug_input_view *view_ = nullptr;

        sentinel() = default;
        explicit constexpr sentinel(debug_input_view &view) noexcept
          : view_{&view}
        {}
    };
    struct iterator
    {
        using iterator_category = std::input_iterator_tag;
        using value_type = meta::_t<std::remove_cv<T>>;
        using difference_type = index_t;
        using reference = T &;
        using pointer = T *;

        debug_input_view *view_ = nullptr;
        version_t version_ = 0;

        iterator() = default;
        explicit constexpr iterator(debug_input_view &view) noexcept
          : view_{&view}, version_{view.version_}
        {}

        constexpr /*c++14*/ void check_current() const noexcept
        {
            RANGES_ENSURE(view_), RANGES_ENSURE(view_->version_ == version_);
        }

        constexpr /*c++14*/ void check_dereferenceable() const noexcept
        {
            check_current(), RANGES_ENSURE(view_->data_ < view_->last_);
        }

        constexpr /*c++14*/
        reference operator*() const noexcept
        {
            check_dereferenceable();
            return *view_->data_;
        }
        constexpr /*c++14*/
        iterator &operator++() noexcept
        {
            check_dereferenceable();
            ++view_->data_;
            version_ = ++view_->version_;
            return *this;
        }
        constexpr /*c++14*/
        void operator++(int) noexcept
        {
            ++*this;
        }

        constexpr /*c++14*/
        friend bool operator==(iterator const &i, sentinel const &s)
        {
            RANGES_ENSURE(i.view_ == s.view_);
            i.check_current();
            return i.view_->data_ == i.view_->last_;
        }
        constexpr /*c++14*/
        friend bool operator==(sentinel const &s, iterator const &i)
        {
            return i == s;
        }
        constexpr /*c++14*/
        friend bool operator!=(iterator const &i, sentinel const &s)
        {
            return !(i == s);
        }
        constexpr /*c++14*/
        friend bool operator!=(sentinel const &s, iterator const &i)
        {
            return !(i == s);
        }
        CPP_member
        constexpr /*c++14*/
        friend auto operator-(sentinel const& s, iterator const& i) ->
            CPP_ret(difference_type)(requires Sized)
        {
            RANGES_ENSURE(i.view_ == s.view_);
            i.check_current();
            return i.view_->last_ - i.view_->data_;
        }
        CPP_member
        constexpr /*c++14*/
        friend auto operator-(iterator const& i, sentinel const& s) ->
            CPP_ret(difference_type)(requires Sized)
        {
            return -(s - i);
        }
    };
    constexpr /*c++14*/ iterator begin() noexcept
    {
        RANGES_ENSURE(valid_);
        RANGES_ENSURE(!begin_called_);
        begin_called_ = true;
        return iterator{*this};
    }
    constexpr /*c++14*/ sentinel end() noexcept
    {
        RANGES_ENSURE(valid_);
        return sentinel{*this};
    }
    CPP_member
    constexpr /*c++14*/
    auto size() const noexcept -> CPP_ret(std::size_t)(requires Sized)
    {
        RANGES_ENSURE(valid_);
        RANGES_ENSURE(!begin_called_);
        return static_cast<std::size_t>(last_ - data_);
    }
};

#endif
