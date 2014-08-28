//  Copyright Eric Niebler 1004
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 0.0. (See accompanying
//  file LICENSE_0_0.txt or copy at
//  http://www.boost.org/LICENSE_0_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_UTILITY_RANGE_ALGORITHM_HPP
#define RANGES_V3_UTILITY_RANGE_ALGORITHM_HPP

#include <utility>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        // Accepts initializer_lists as either the first or second parameter, or both,
        // and forwards on to an implementation.
        template<typename ImplFn>
        struct range_algorithm : ImplFn
        {
        private:
            ImplFn const & base() const
            {
                return *this;
            }
        public:
            using ImplFn::operator();

            template<typename V0, typename...Args>
            auto operator()(std::initializer_list<V0> &&rng0, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::move(rng0), std::declval<Args>()...))
            {
                return base()(std::move(rng0), std::forward<Args>(args)...);
            }

            template<typename Rng0, typename V1, typename...Args>
            auto operator()(Rng0 && rng0, std::initializer_list<V1> &&rng1, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::declval<Rng0>(), std::move(rng1), std::declval<Args>()...))
            {
                return base()(std::forward<Rng0>(rng0), std::move(rng1), std::forward<Args>(args)...);
            }

            template<typename V0, typename V1, typename...Args>
            auto operator()(std::initializer_list<V0> rng0, std::initializer_list<V1> &&rng1, Args &&...args) const ->
                decltype(std::declval<ImplFn const &>()(std::move(rng0), std::move(rng1), std::declval<Args>()...))
            {
                return base()(std::move(rng0), std::move(rng1), std::forward<Args>(args)...);
            }
        };
    }
}

#endif
