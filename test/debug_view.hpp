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
#include <memory>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/swap.hpp>

template<typename T, bool Sized = true>
struct debug_input_view
{
    static_assert(std::is_object<T>::value, "");

    using index_t = std::ptrdiff_t;
    using version_t = long;

    struct data
    {
        T *data_ = nullptr;
        T *last_ = nullptr;
        version_t version_ = 0;
        bool valid_ = false;
        bool begin_called_ = false;
    };
    std::shared_ptr<data> data_{};

    debug_input_view() = default;
    debug_input_view(T* p, index_t size)
      : data_(std::make_shared<data>())
    {
        RANGES_ENSURE(p || !size);
        data_->data_ = p;
        data_->last_ = p + size;
        data_->valid_ = true;
    }
    template<index_t N>
    debug_input_view(T (&data)[N])
      : debug_input_view{data, N}
    {}
    debug_input_view(std::shared_ptr<data> data)
      : data_{std::move(data)}
    {
        if(!data_)
            return;
        if(data_->begin_called_)
            data_->valid_ = false;
        else
            ++data_->version_;
    }
    debug_input_view(debug_input_view &&that)
      : debug_input_view{std::move(that.data_)}
    {}
    debug_input_view(debug_input_view const &that)
      : debug_input_view{that.data_}
    {}
    debug_input_view &operator=(debug_input_view that)
    {
        data_.swap(that.data_);
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
          : view_{&view}, version_{view.data_? view.data_->version_ : -1}
        {}

        void check_current() const noexcept
        {
            RANGES_ENSURE(view_),
            RANGES_ENSURE(view_->data_),
            RANGES_ENSURE(view_->data_->version_ == version_);
        }

        void check_dereferenceable() const noexcept
        {
            check_current(), RANGES_ENSURE(view_->data_->data_ < view_->data_->last_);
        }

        reference operator*() const noexcept
        {
            check_dereferenceable();
            return *view_->data_->data_;
        }
        iterator &operator++() noexcept
        {
            check_dereferenceable();
            ++view_->data_->data_;
            version_ = ++view_->data_->version_;
            return *this;
        }
        void operator++(int) noexcept
        {
            ++*this;
        }

        friend bool operator==(iterator const &i, sentinel const &s)
        {
            RANGES_ENSURE(i.view_ == s.view_);
            i.check_current();
            return i.view_->data_->data_ == i.view_->data_->last_;
        }
        friend bool operator==(sentinel const &s, iterator const &i)
        {
            return i == s;
        }
        friend bool operator!=(iterator const &i, sentinel const &s)
        {
            return !(i == s);
        }
        friend bool operator!=(sentinel const &s, iterator const &i)
        {
            return !(i == s);
        }
        CPP_member
        friend auto operator-(sentinel const& s, iterator const& i) ->
            CPP_ret(difference_type)(requires Sized)
        {
            RANGES_ENSURE(i.view_ == s.view_);
            i.check_current();
            return i.view_->data_->last_ - i.view_->data_->data_;
        }
        CPP_member
        friend auto operator-(iterator const& i, sentinel const& s) ->
            CPP_ret(difference_type)(requires Sized)
        {
            return -(s - i);
        }
    };
    iterator begin() noexcept
    {
        RANGES_ENSURE(data_->valid_);
        RANGES_ENSURE(!data_->begin_called_);
        data_->begin_called_ = true;
        return iterator{*this};
    }
    sentinel end() noexcept
    {
        RANGES_ENSURE(data_->valid_);
        return sentinel{*this};
    }
    CPP_member
    auto size() const noexcept -> CPP_ret(std::size_t)(requires Sized)
    {
        RANGES_ENSURE(data_->valid_);
        RANGES_ENSURE(!data_->begin_called_);
        return static_cast<std::size_t>(data_->last_ - data_->data_);
    }
};

#endif
