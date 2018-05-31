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

#ifndef RANGES_V3_ISTREAM_RANGE_HPP
#define RANGES_V3_ISTREAM_RANGE_HPP

#include <istream>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>

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
            movesemiregular_t<Val> obj_;
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
                Val &read() const noexcept
                {
                    return rng_->cached();
                }
                bool equal(default_sentinel) const
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
            explicit istream_range(std::istream &sin)
              : sin_(&sin), obj_{}
            {
                next(); // prime the pump
            }
            Val & cached() noexcept
            {
                return obj_;
            }
        };

    #if !RANGES_CXX_VARIABLE_TEMPLATES
        template<typename Val>
        istream_range<Val> istream(std::istream & sin)
        {
            CONCEPT_ASSERT_MSG(DefaultConstructible<Val>(),
               "Only DefaultConstructible types are extractable from streams.");
            return istream_range<Val>{sin};
        }
    #else
        template<typename Val, CONCEPT_REQUIRES_(DefaultConstructible<Val>())>
        struct istream_fn
        {
            istream_range<Val> operator()(std::istream & sin) const
            {
                return istream_range<Val>{sin};
            }
        };

    #if RANGES_CXX_INLINE_VARIABLES < RANGES_CXX_INLINE_VARIABLES_17
        inline namespace
        {
            template<typename Val>
            constexpr auto& istream = static_const<istream_fn<Val>>::value;
        }
    #else  // RANGES_CXX_INLINE_VARIABLES >= RANGES_CXX_INLINE_VARIABLES_17
        template<typename Val>
        inline constexpr istream_fn<Val> istream{};
    #endif  // RANGES_CXX_INLINE_VARIABLES

    #endif  // RANGES_CXX_VARIABLE_TEMPLATES
        /// @}
    }
}

#endif
