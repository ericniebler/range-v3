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

#ifndef RANGES_V3_VIEW_TO_CONTAINER_HPP
#define RANGES_V3_VIEW_TO_CONTAINER_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/common_iterator.hpp>

#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
// Non-portable forward declarations of standard containers
RANGES_BEGIN_NAMESPACE_STD
    template<class Value, class Alloc /*= allocator<Value>*/>
    class vector;
RANGES_END_NAMESPACE_STD
#else
#include <vector>
#endif

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename Rng, typename Cont, typename I = range_common_iterator_t<Rng>>
            using ConvertibleToContainer = fast_logical_and<
                Iterable<Cont>,
                logical_not<Range<Cont>>,
                Movable<Cont>,
                Convertible<range_value_t<Rng>, range_value_t<Cont>>,
                Constructible<Cont, I, I>>;

            template<typename Container, typename Rng,
                CONCEPT_REQUIRES_(detail::ConvertibleToContainer<Rng, Container>())>
            Container to_container(Rng && rng)
            {
                static_assert(!is_infinite<Rng>::value,
                    "Attempt to convert an infinite range to a container.");
                using I = range_common_iterator_t<Rng>;
                return Container{I{begin(rng)}, I{end(rng)}};
            }
        }

        namespace view
        {
            template<typename ContainerMetafunctionClass>
            struct to_container_fn
              : pipeable<to_container_fn<ContainerMetafunctionClass>>
            {
                template<typename Rng,
                    typename Cont = meta_apply<ContainerMetafunctionClass, range_value_t<Rng>>,
                    CONCEPT_REQUIRES_(Iterable<Rng>() && detail::ConvertibleToContainer<Rng, Cont>())>
                Cont operator()(Rng && rng) const
                {
                    return detail::to_container<Cont>(std::forward<Rng>(rng));
                }
            };

            RANGES_CONSTEXPR to_container_fn<meta_quote<std::vector>> to_vector {};

            // BUGBUG switch to variable template if it's available
            template<template<typename...> class Cont>
            view::to_container_fn<meta_quote<Cont>> to_()
            {
                return {};
            }

            template<typename Cont>
            view::to_container_fn<meta_always<Cont>> to_()
            {
                return {};
            }
        }
    }
}

#endif
