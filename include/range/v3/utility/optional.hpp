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

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/variant.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T>
        struct optional
        {
        private:
            tagged_variant<T, detail::empty> data_;
        public:
            optional() = default;
            optional(T t)
              : data_(meta::size_t<0>{}, std::move(t))
            {}
            explicit operator bool() const
            {
                return data_.which() == 0;
            }
            bool operator!() const
            {
                return data_.which() != 0;
            }
            T & operator*()
            {
                RANGES_ASSERT(!!*this);
                return ranges::get<0>(data_);
            }
            T const & operator*() const
            {
                RANGES_ASSERT(!!*this);
                return ranges::get<0>(data_);
            }
            optional &operator=(T t)
            {
                ranges::set<0>(data_, std::move(t));
                return *this;
            }
            void reset()
            {
                ranges::set<1>(data_, detail::empty{});
            }
        };
    }
}

#endif
