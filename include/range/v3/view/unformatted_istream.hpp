/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Google LLC 2020-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_UNFORMATTED_ISTREAM_HPP
#define RANGES_V3_VIEW_UNFORMATTED_ISTREAM_HPP

#include <istream>

#include <range/v3/range_fwd.hpp>

#include <range/v3/iterator/default_sentinel.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/facade.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Val, typename CharT = char, typename Traits = std::char_traits<CharT>>
    struct basic_unformatted_istream_view
    : view_facade<basic_unformatted_istream_view<Val, CharT, Traits>, unknown>
    {
    private:
        friend range_access;
        std::basic_istream<CharT, Traits> * sin_;
        semiregular_box_t<Val> obj_;
        struct cursor
        {
        private:
            friend range_access;
            using single_pass = std::true_type;
            basic_unformatted_istream_view * rng_ = nullptr;

        public:
            cursor() = default;
            explicit cursor(basic_unformatted_istream_view * rng) noexcept
              : rng_(rng)
            {}
            void next()
            {
                rng_->next();
            }
            Val & read() const noexcept
            {
                return rng_->cached();
            }
            bool equal(default_sentinel_t) const noexcept
            {
                return !rng_->sin_;
            }
            bool equal(cursor that) const noexcept
            {
                return !rng_->sin_ == !that.rng_->sin_;
            }
        };
        void next()
        {
            if(!sin_->read(reinterpret_cast<CharT*>(std::addressof(cached())), sizeof(Val))) {
                sin_ = nullptr;
            }
        }
        cursor begin_cursor()
        {
            return cursor{this};
        }

    public:
        basic_unformatted_istream_view() = default;
        explicit basic_unformatted_istream_view(std::istream & sin)
        noexcept(std::is_nothrow_default_constructible<Val>::value)
          : sin_(&sin)
          , obj_{}
        {
            // The current `value_` is considered stale, so we'd better update it (otherwise the
            // first value in the range will *always* be `Val()`.)
            next();
        }
        Val & cached() noexcept
        {
            return obj_;
        }
    };

    /// \cond
    namespace _unformatted_istream_
    {
        /// \endcond
        template(typename Val, typename CharT, typename Traits)(
            /// \pre
            requires copy_constructible<Val> AND default_constructible<Val>)
        inline basic_unformatted_istream_view<Val, CharT, Traits>
        unformatted_istream_view(std::basic_istream<CharT, Traits> & sin)
        {
            return basic_unformatted_istream_view<Val>{sin};
        }
        /// \cond
    } // namespace _unformatted_istream_
    using namespace _unformatted_istream_;
    /// \endcond
    /// @}
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif
