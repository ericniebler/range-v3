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
#ifndef RANGES_V3_DETAIL_WITH_BRACED_INIT_ARGS_HPP
#define RANGES_V3_DETAIL_WITH_BRACED_INIT_ARGS_HPP

#include <initializer_list>
#include <range/v3/functional/invoke.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        // Accepts initializer_lists as either the first or second parameter, or both,
        // and forwards on to an implementation.
        template<typename ImplFn>
        struct with_braced_init_args
          : ImplFn
        {
        private:
            constexpr ImplFn const & base() const
            {
                return *this;
            }
        public:
            using ImplFn::operator();

            template<typename V0, typename...Args>
            constexpr auto operator()(std::initializer_list<V0> &&rng0, Args &&...args) const ->
                invoke_result_t<ImplFn const &, std::initializer_list<V0>, Args...>
            {
                return base()(static_cast<std::initializer_list<V0> &&>(rng0), static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename Rng0, typename V1, typename...Args>
            constexpr auto operator()(Rng0 &&rng0, std::initializer_list<V1> &&rng1, Args &&...args) const ->
                invoke_result_t<ImplFn const &, Rng0, std::initializer_list<V1>, Args...>
            {
                return base()(
                    static_cast<Rng0 &&>(rng0),
                    static_cast<std::initializer_list<V1> &&>(rng1),
                    static_cast<Args &&>(args)...);
            }
            /// \overload
            template<typename V0, typename V1, typename...Args>
            constexpr auto operator()(std::initializer_list<V0> &&rng0, std::initializer_list<V1> &&rng1, Args &&...args) const ->
                invoke_result_t<ImplFn const &, std::initializer_list<V0>, std::initializer_list<V1>, Args...>
            {
                return base()(
                    static_cast<std::initializer_list<V0> &&>(rng0),
                    static_cast<std::initializer_list<V1> &&>(rng1),
                    static_cast<Args &&>(args)...);
            }
        };
    }
    /// \endcond
}

#endif
