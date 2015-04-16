/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_OPTIONAL_HPP
#define RANGES_V3_UTILITY_OPTIONAL_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/variant.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-utility
        template<typename T>
        struct optional
        {
        private:
            tagged_variant<T, meta::nil_> data_;
        public:
            RANGES_RELAXED_CONSTEXPR optional() = default;
            RANGES_RELAXED_CONSTEXPR optional(optional const&) = default;
            RANGES_RELAXED_CONSTEXPR optional(optional && o) = default;
            RANGES_RELAXED_CONSTEXPR optional& operator=(optional const&) = default;
            RANGES_RELAXED_CONSTEXPR optional& operator=(optional &&) = default;

            RANGES_RELAXED_CONSTEXPR optional(T t)
              : data_(meta::size_t<0>{}, std::move(t))
            {}
            RANGES_RELAXED_CONSTEXPR explicit operator bool() const
            {
                return data_.which() == 0;
            }
            RANGES_RELAXED_CONSTEXPR bool operator!() const
            {
                return data_.which() != 0;
            }
            RANGES_RELAXED_CONSTEXPR T & operator*()
            {
                RANGES_ASSERT(!!*this);
                return ranges::get<0>(data_);
            }
            RANGES_RELAXED_CONSTEXPR T const & operator*() const
            {
                RANGES_ASSERT(!!*this);
                return ranges::get<0>(data_);
            }
            RANGES_RELAXED_CONSTEXPR optional &operator=(T const &t)
            {
                ranges::set<0>(data_, t);
                return *this;
            }
            RANGES_RELAXED_CONSTEXPRoptional &operator=(T &&t)
            {
                ranges::set<0>(data_, std::move(t));
                return *this;
            }
            RANGES_RELAXED_CONSTEXPR void reset()
            {
                ranges::set<1>(data_, meta::nil_{});
            }
        };
    }
}

#endif
