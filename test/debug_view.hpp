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
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/swap.hpp>

template<typename T, bool Sized = true>
struct debug_input_view
{
    CONCEPT_ASSERT(std::is_object<T>::value);

    using index_t = std::ptrdiff_t;
    using version_t = unsigned long;

    T *data_ = nullptr;
    T *last_ = nullptr;
    version_t version_ = 0;
    bool valid_ = false;
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
    RANGES_CXX14_CONSTEXPR
    debug_input_view(debug_input_view &&that) noexcept
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
    RANGES_CXX14_CONSTEXPR
    debug_input_view &operator=(debug_input_view &&that) noexcept
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

        RANGES_CXX14_CONSTEXPR void check_current() const noexcept
        {
            RANGES_ENSURE(view_), RANGES_ENSURE(view_->version_ == version_);
        }

        RANGES_CXX14_CONSTEXPR void check_dereferenceable() const noexcept
        {
            check_current(), RANGES_ENSURE(view_->data_ < view_->last_);
        }

        RANGES_CXX14_CONSTEXPR
        reference operator*() const noexcept
        {
            check_dereferenceable();
            return *view_->data_;
        }
        RANGES_CXX14_CONSTEXPR
        iterator &operator++() noexcept
        {
            check_dereferenceable();
            ++view_->data_;
            version_ = ++view_->version_;
            return *this;
        }
        RANGES_CXX14_CONSTEXPR
        void operator++(int) noexcept
        {
            ++*this;
        }

        RANGES_CXX14_CONSTEXPR
        friend bool operator==(iterator const &i, sentinel const &s)
        {
            RANGES_ENSURE(i.view_ == s.view_);
            i.check_current();
            return i.view_->data_ == i.view_->last_;
        }
        RANGES_CXX14_CONSTEXPR
        friend bool operator==(sentinel const &s, iterator const &i)
        {
            return i == s;
        }
        RANGES_CXX14_CONSTEXPR
        friend bool operator!=(iterator const &i, sentinel const &s)
        {
            return !(i == s);
        }
        RANGES_CXX14_CONSTEXPR
        friend bool operator!=(sentinel const &s, iterator const &i)
        {
            return !(i == s);
        }
        CONCEPT_REQUIRES(Sized)
        RANGES_CXX14_CONSTEXPR
        friend difference_type operator-(sentinel const& s, iterator const& i)
        {
            RANGES_ENSURE(i.view_ == s.view_);
            i.check_current();
            return i.view_->last_ - i.view_->data_;
        }
        CONCEPT_REQUIRES(Sized)
        RANGES_CXX14_CONSTEXPR
        friend difference_type operator-(iterator const& i, sentinel const& s)
        {
            return -(s - i);
        }
    };
    RANGES_CXX14_CONSTEXPR iterator begin() noexcept
    {
        RANGES_ENSURE(valid_);
        RANGES_ENSURE(!begin_called_);
        begin_called_ = true;
        return iterator{*this};
    }
    RANGES_CXX14_CONSTEXPR sentinel end() noexcept
    {
        RANGES_ENSURE(valid_);
        return sentinel{*this};
    }
    CONCEPT_REQUIRES(Sized)
    RANGES_CXX14_CONSTEXPR
    std::size_t size() const noexcept
    {
        RANGES_ENSURE(valid_);
        RANGES_ENSURE(!begin_called_);
        return static_cast<std::size_t>(last_ - data_);
    }
};

#endif
