//  (C) Copyright Kevlin Henney and Dave Abrahams 1999.
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP
#define RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP

#include <memory>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/config.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Target, typename Source>
        meta::if_<std::is_pointer<Target>, Target>
        polymorphic_downcast(Source *x) noexcept
        {
            RANGES_ASSERT(dynamic_cast<Target>(x) == x);
            return static_cast<Target>(x);
        }
        template<typename Target, typename Source>
        meta::if_<std::is_reference<Target>, Target>
        polymorphic_downcast(Source &&x) noexcept
        {
            using PTarget = meta::_t<std::add_pointer<Target>>;
            auto ptr = polymorphic_downcast<PTarget>(std::addressof(x));
            return static_cast<Target>(*ptr);
        }
    }
}

#endif  // RANGES_V3_UTILITY_POLYMORPHIC_CAST_HPP
