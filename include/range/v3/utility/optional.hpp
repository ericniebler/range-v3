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
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/variant.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \ingroup group-utility
        struct in_place_t {};
        namespace
        {
            constexpr auto &in_place = static_const<in_place_t>::value;
        }

        template<typename T>
        struct optional
        {
        private:
            tagged_variant<meta::nil_, T> data_;
        public:
            optional() = default;
            optional(T t)
              : data_(meta::size_t<1>{}, std::move(t))
            {}
            template <typename...Args, CONCEPT_REQUIRES_(Constructible<T, Args...>())>
            explicit optional(in_place_t, Args &&...args)
              : data_(meta::size_t<1>{}, std::forward<Args>(args)...)
            {}
            explicit operator bool() const
            {
                return data_.which() != 0;
            }
            T & operator*()
            {
                RANGES_ASSERT(*this);
                return ranges::get<1>(data_);
            }
            T const & operator*() const
            {
                RANGES_ASSERT(*this);
                return ranges::get<1>(data_);
            }
            optional &operator=(T const &t)
            {
                ranges::set<1>(data_, t);
                return *this;
            }
            optional &operator=(T &&t)
            {
                ranges::set<1>(data_, std::move(t));
                return *this;
            }
            void reset()
            {
                ranges::set<0>(data_);
            }
        };
    }
}

#endif
