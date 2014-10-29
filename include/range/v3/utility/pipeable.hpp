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

#ifndef RANGES_V3_UTILITY_PIPEABLE_HPP
#define RANGES_V3_UTILITY_PIPEABLE_HPP

#include <utility>

namespace ranges
{
    inline namespace v3
    {
        template<typename Derived>
        struct pipeable
        {
        private:
            friend Derived;
            Derived const & derived() const &
            {
                return static_cast<Derived const &>(*this);
            }
            Derived && derived() &&
            {
                return static_cast<Derived &&>(const_cast<pipeable &&>(*this));
            }

            // Default Pipe behavior just passes the argument to the pipe's function call
            // operator
            template<typename Arg, typename Pipe>
            static auto pipe(Arg && arg, Pipe && pipe) ->
                decltype(std::declval<Pipe>()(std::declval<Arg>()))
            {
                return std::forward<Pipe>(pipe)(std::forward<Arg>(arg));
            }
        public:
            template<typename Arg, typename D = Derived>
            friend auto operator|(Arg && arg, pipeable const & pipe) ->
                decltype(D::pipe(std::declval<Arg>(), std::declval<Derived const &>()))
            {
                return D::pipe(std::forward<Arg>(arg), pipe.derived());
            }
            template<typename Arg, typename D = Derived>
            friend auto operator|(Arg && arg, pipeable && pipe) ->
                decltype(D::pipe(std::declval<Arg>(), std::declval<Derived>()))
            {
                return D::pipe(std::forward<Arg>(arg), std::move(pipe).derived());
            }
        };
    }
}

#endif // RANGES_V3_UTILITY_PIPEABLE_HPP
