/// \file
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

#ifndef RANGES_V3_GETLINES_HPP
#define RANGES_V3_GETLINES_HPP

#include <string>
#include <istream>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        struct getlines_range
          : view_facade<getlines_range, unknown>
        {
        private:
            friend range_access;
            std::istream *sin_;
            std::string str_;
            char delim_;
            struct cursor
            {
            private:
                getlines_range *rng_;
            public:
                cursor() = default;
                explicit cursor(getlines_range &rng)
                  : rng_(&rng)
                {}
                void next()
                {
                    rng_->next();
                }
                std::string &read() const noexcept
                {
                    return rng_->str_;
                }
                bool equal(default_sentinel) const
                {
                    return !*rng_->sin_;
                }
            };
            void next()
            {
                std::getline(*sin_, str_, delim_);
            }
            cursor begin_cursor()
            {
                return cursor{*this};
            }
        public:
            getlines_range() = default;
            getlines_range(std::istream &sin, char delim = '\n')
              : sin_(&sin), str_{}, delim_(delim)
            {
                this->next(); // prime the pump
            }
            std::string & cached() noexcept
            {
                return str_;
            }
        };

        struct getlines_fn
        {
            getlines_range operator()(std::istream & sin, char delim = '\n') const
            {
                return getlines_range{sin, delim};
            }
        };

        RANGES_INLINE_VARIABLE(getlines_fn, getlines)
        /// @}
    }
}

#endif
