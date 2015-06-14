/// \file
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

#ifndef RANGES_V3_ISTREAM_RANGE_HPP
#define RANGES_V3_ISTREAM_RANGE_HPP

#include <istream>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/semiregular.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-core
        /// @{
        template<typename Val>
        struct istream_range
          : view_facade<istream_range<Val>, unknown>
        {
        private:
            friend range_access;
            std::istream *sin_;
            semiregular_t<Val> obj_;
            struct cursor
            {
            private:
                istream_range *rng_;
            public:
                cursor() = default;
                explicit cursor(istream_range &rng)
                  : rng_(&rng)
                {}
                void next()
                {
                    rng_->next();
                }
                Val const &current() const
                {
                    return rng_->cached();
                }
                bool done() const
                {
                    return !*rng_->sin_;
                }
            };
            void next()
            {
                *sin_ >> cached();
            }
            cursor begin_cursor()
            {
                return cursor{*this};
            }
        public:
            istream_range() = default;
            istream_range(std::istream &sin)
              : sin_(&sin), obj_{}
            {
                next(); // prime the pump
            }
            Val & cached()
            {
                return obj_;
            }
        };

        /// TODO use a variable template here when they're available
        template<typename Val>
        istream_range<Val> istream(std::istream & sin)
        {
            return istream_range<Val>{sin};
        }

        /// @}
    }
}

#endif
