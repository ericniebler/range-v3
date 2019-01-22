/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_ISTREAM_RANGE_HPP
#define RANGES_V3_ISTREAM_RANGE_HPP

#include <istream>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_access.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/iterator/default_sentinel.hpp>

namespace ranges
{
    /// \addtogroup group-core
    /// @{
    template<typename Val>
    struct istream_range
      : view_facade<istream_range<Val>, unknown>
    {
    private:
        friend range_access;
        std::istream *sin_;
        movesemiregular_t<Val> obj_;
        struct cursor
        {
        private:
            friend range_access;
            using single_pass = std::true_type;
            istream_range *rng_ = nullptr;
        public:
            cursor() = default;
            explicit cursor(istream_range &rng)
              : rng_(&rng)
            {}
            void next()
            {
                rng_->next();
            }
            Val &read() const noexcept
            {
                return rng_->cached();
            }
            bool equal(default_sentinel_t) const
            {
                return !rng_->sin_;
            }
            bool equal(cursor that) const
            {
                return !rng_->sin_ == !that.rng_->sin_;
            }
        };
        void next()
        {
            if(!(*sin_ >> cached()))
                sin_ = nullptr;
        }
        cursor begin_cursor()
        {
            return cursor{*this};
        }
    public:
        istream_range() = default;
        explicit istream_range(std::istream &sin)
          : sin_(&sin), obj_{}
        {
            next(); // prime the pump
        }
        Val & cached() noexcept
        {
            return obj_;
        }
    };

    #if !RANGES_CXX_VARIABLE_TEMPLATES
    template<typename Val>
    istream_range<Val> istream(std::istream & sin)
    {
        CPP_assert_msg(DefaultConstructible<Val>,
           "Only DefaultConstructible types are extractable from streams.");
        return istream_range<Val>{sin};
    }
    #else
    CPP_template(typename Val)(
        requires DefaultConstructible<Val>)
    struct istream_fn
    {
        istream_range<Val> operator()(std::istream & sin) const
        {
            return istream_range<Val>{sin};
        }
    };

    #if RANGES_CXX_INLINE_VARIABLES < RANGES_CXX_INLINE_VARIABLES_17
    inline namespace
    {
        template<typename Val>
        constexpr auto& istream = static_const<istream_fn<Val>>::value;
    }
    #else  // RANGES_CXX_INLINE_VARIABLES >= RANGES_CXX_INLINE_VARIABLES_17
    template<typename Val>
    inline constexpr istream_fn<Val> istream{};
    #endif  // RANGES_CXX_INLINE_VARIABLES

    #endif  // RANGES_CXX_VARIABLE_TEMPLATES
    /// @}
}

#endif
