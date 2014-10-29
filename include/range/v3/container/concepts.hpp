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

#ifndef RANGES_V3_CONTAINER_CONCEPTS_HPP
#define RANGES_V3_CONTAINER_CONCEPTS_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/logical_ops.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace concepts
        {
            // std::array is a SemiContainer, native arrays are not.
            struct SemiContainer
              : refines<Iterable>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<SemiRegular>(aux::copy(t)),
                        concepts::is_false(is_range<T>())
                    ));
            };

            // std::vector is a Container, std::array is not
            struct Container
              : refines<SemiContainer>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<Constructible>(aux::copy(t), begin(t), end(t))
                    ));
            };
        }

        template<typename T>
        using SemiContainer = concepts::models<concepts::SemiContainer, T>;

        template<typename T>
        using Container = concepts::models<concepts::Container, T>;

        template<typename T>
        struct RvalueContainerLike
          : logical_or<
                fast_logical_and<
                    Container<T>,
                    logical_not<std::is_reference<T>>
                >,
                logical_and<
                    is_reference_wrapper_t<T>,
                    // like Container<referent_of_t<T>>, but lazy:
                    meta_defer<meta_compose<Container, meta_quote<referent_of>::apply>::apply, T>
                >
            >
        {};

        template<typename T>
        struct LvalueContainerLike
          : logical_or<
                fast_logical_and<
                    Container<T>,
                    std::is_lvalue_reference<T>
                >,
                logical_and<
                    is_reference_wrapper_t<T>,
                    // like Container<referent_of_t<T>>, but lazy:
                    meta_defer<meta_compose<Container, meta_quote<referent_of>::apply>::apply, T>
                >
            >
        {};
    }
}

#endif
