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

#include <utility>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
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
              : refines<ForwardIterable>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        concepts::model_of<DefaultConstructible>(std::move(t)),
                        concepts::model_of<Movable>(std::move(t)),
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
                        concepts::model_of<Constructible>(std::move(t), begin(t), end(t))
                    ));
            };
        }

        template<typename T>
        using SemiContainer = concepts::models<concepts::SemiContainer, T>;

        template<typename T>
        using Container = concepts::models<concepts::Container, T>;

        namespace detail
        {
            template<typename T, CONCEPT_REQUIRES_(Container<T>())>
            std::true_type is_lvalue_container_like(T &);

            template<typename T, CONCEPT_REQUIRES_(Container<T>())>
            std::true_type is_lvalue_container_like(reference_wrapper<T>);

            template<typename T, CONCEPT_REQUIRES_(Container<T>())>
            std::true_type is_lvalue_container_like(std::reference_wrapper<T>);
        }

        namespace concepts
        {
            struct LvalueContainerLike
              : refines<ForwardIterable>
            {
                template<typename T>
                auto requires_(T && t) -> decltype(
                    concepts::valid_expr(
                        detail::is_lvalue_container_like(std::forward<T>(t))
                    ));
            };
        }

        template<typename T>
        using LvalueContainerLike = concepts::models<concepts::LvalueContainerLike, T>;
    }
}

#endif
