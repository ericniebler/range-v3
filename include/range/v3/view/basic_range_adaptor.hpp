/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGE_V3_VIEW_BASIC_RANGE_ADAPTOR_HPP
#define RANGE_V3_VIEW_BASIC_RANGE_ADAPTOR_HPP

#include <range/v3/detail/config.hpp>

#if RANGES_CXX_VER >= RANGES_CXX_STD_17

#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            /// \cond
            namespace details
            {
                template<template<class...> class V>
                struct basic_range_adaptor_fn
                {
                    using Self = basic_range_adaptor_fn<V>;

                    template<class ...Args>
                    static auto bind(Self self, Args &&...args)
                    {
                        return make_pipeable(
                            std::bind(self, std::placeholders::_1, std::forward<Args>(args)...)
                        );
                    }

                    template<typename Rng, class ...Args>
                    auto operator()(Rng &&rng, Args &&...args) const
                    {
                        // >=C++17 Class template argument deduction (CTAD)
                        // could be just:
                        // return V(std::forward<Rng>(rng), std::forward<Args>(args)...);
                        // But this will work even without CTAD deduction guides for most cases:
                        return V(ranges::view::all(std::forward<Rng>(rng)), std::forward<Args>(args)...);
                    }
                };
            }
            /// \endcond

            template<template<class...> class V>
            using basic_range_adaptor_t = ranges::view::view<details::basic_range_adaptor_fn<V>>;

            template<template<class...> class V>
            constexpr const basic_range_adaptor_t<V> basic_range_adaptor{};
        }
    }
}

#endif // RANGES_CXX_VER >= RANGES_CXX_STD_17

#endif //RANGE_V3_VIEW_BASIC_RANGE_ADAPTOR_HPP
