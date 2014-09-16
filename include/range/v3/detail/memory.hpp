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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef RANGES_V3_DETAIL_MEMORY_HPP
#define RANGES_V3_DETAIL_MEMORY_HPP

#include <memory>
#include <type_traits>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename T, bool B = std::is_trivially_destructible<T>::value>
            struct destroy_n
            {
                destroy_n const &operator++() const noexcept
                {
                    return *this;
                }
                void operator()(T const *) const noexcept
                {}
            };

            template<typename T>
            struct destroy_n<T, false>
            {
            private:
                std::size_t n_ = 0;
            public:
                destroy_n &operator++() noexcept
                {
                    ++n_;
                    return *this;
                }
                void operator()(T const *p) noexcept
                {
                    for(std::size_t n = 0; n < n_; ++n, ++p)
                        p->~T();
                }
            };

            struct return_temporary_buffer
            {
                template<typename T>
                void operator()(T *p) const
                {
                    std::return_temporary_buffer(p);
                }
            };
        }
    }
}

#endif
