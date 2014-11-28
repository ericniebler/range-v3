// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_SEMIREGULAR_HPP
#define RANGES_V3_UTILITY_SEMIREGULAR_HPP

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/optional.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T>
        struct semiregular
        {
        private:
            optional<T> t_;
        public:
            semiregular() = default;
            semiregular(T f)
              : t_(std::move(f))
            {}
            T & get()
            {
                RANGES_ASSERT(!!t_);
                return *t_;
            }
            T const & get() const
            {
                RANGES_ASSERT(!!t_);
                return *t_;
            }
            operator T &()
            {
                return get();
            }
            operator T const &() const
            {
                return get();
            }
            template<typename...Args>
            auto operator()(Args &&...args) ->
                decltype(std::declval<T &>()(std::forward<Args>(args)...))
            {
                return get()(std::forward<Args>(args)...);
            }
            template<typename...Args>
            auto operator()(Args &&...args) const ->
                decltype(std::declval<T const &>()(std::forward<Args>(args)...))
            {
                return get()(std::forward<Args>(args)...);
            }
        };

        template<typename T>
        using semiregular_t = meta::if_<SemiRegular<T>, T, semiregular<T>>;
    }
}

#endif
