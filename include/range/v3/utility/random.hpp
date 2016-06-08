/// \file
// Range v3 library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_UTILITY_RANDOM_HPP
#define RANGES_V3_UTILITY_RANDOM_HPP

#include <random>
#include <meta/meta.hpp>
#include <range/v3/detail/config.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-concepts
        /// @{
        namespace concepts
        {
            struct UniformRandomNumberGenerator
              : refines<Function>
            {
                template<typename Gen>
                auto requires_(Gen&&) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<UnsignedIntegral>(val<Gen>()())
                    ));
            };
        }

        template<typename Gen>
        using UniformRandomNumberGenerator = concepts::models<concepts::UniformRandomNumberGenerator, Gen>;
        /// @}

        /// \cond
        namespace detail
        {
            using default_random_engine = std::mt19937;

            inline default_random_engine& get_random_engine()
            {
                // TODO: seed properly.
                RANGES_STATIC_THREAD_LOCAL default_random_engine engine{std::random_device{}()};
                return engine;
            }
        }
        /// \endcond
    }
}

#endif
