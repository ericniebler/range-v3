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

#ifndef RANGES_V3_UTILITY_SEMIREGULAR_WRAPPERS_HPP
#define RANGES_V3_UTILITY_SEMIREGULAR_WRAPPERS_HPP

#include <memory>
#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/optional.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename T, bool IsSemiRegular = SemiRegular<T>()>
        struct value_wrapper
        {
        private:
            optional<T> t_;
        public:
            value_wrapper() = default;
            value_wrapper(T f)
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
        struct reference_wrapper
        {
        private:
            T *t_;
        public:
            reference_wrapper() = default;
            reference_wrapper(T &t)
              : t_(std::addressof(t))
            {}
            T & get() const
            {
                RANGES_ASSERT(nullptr != t_);
                return *t_;
            }
            operator T &() const
            {
                return get();
            }
            template<typename ...Args>
            auto operator()(Args &&...args) const ->
                decltype((*t_)(std::forward<Args>(args)...))
            {
                return (*t_)(std::forward<Args>(args)...);
            }
        };

        struct ref_fn
        {
            template<typename T>
            reference_wrapper<T> operator()(T & t) const
            {
                return {t};
            }
        };

        RANGES_CONSTEXPR ref_fn ref {};
    }
}

#endif
