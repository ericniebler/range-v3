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

#ifndef RANGES_V3_TO_CONTAINER_HPP
#define RANGES_V3_TO_CONTAINER_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/action/concepts.hpp>

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
        /// \cond
        namespace detail
        {
            template<typename Rng, typename Cont, typename I = range_common_iterator_t<Rng>>
            using ConvertibleToContainer = meta::strict_and<
                Range<Cont>,
                meta::not_<View<Cont>>,
                MoveConstructible<Cont>,
                ConvertibleTo<range_value_type_t<Rng>, range_value_type_t<Cont>>,
                Constructible<Cont, I, I>>;

            template<typename ContainerMetafunctionClass>
            struct to_container_fn
              : pipeable<to_container_fn<ContainerMetafunctionClass>>
            {
            private:
                template<typename C, typename R>
                using ReserveConcept =
                    meta::strict_and<
                        ReserveAndAssignable<C, range_common_iterator_t<R>>,
                        SizedRange<R>>;

                template<typename Rng,
                    typename Cont = meta::invoke<ContainerMetafunctionClass, range_value_type_t<Rng>>,
                    CONCEPT_REQUIRES_(InputRange<Rng>() && detail::ConvertibleToContainer<Rng, Cont>())>
                Cont impl(Rng &&rng, std::false_type) const
                {
                    using I = range_common_iterator_t<Rng>;
                    return Cont(I{ranges::begin(rng)}, I{ranges::end(rng)});
                }

                template<typename Rng,
                    typename Cont = meta::invoke<ContainerMetafunctionClass, range_value_type_t<Rng>>,
                    CONCEPT_REQUIRES_(InputRange<Rng>() && detail::ConvertibleToContainer<Rng, Cont>() &&
                                      ReserveConcept<Cont, Rng>())>
                Cont impl(Rng &&rng, std::true_type) const
                {
                    Cont c;
                    auto const size = ranges::size(rng);
                    using size_type = decltype(c.max_size());
                    using C = common_type_t<
                        meta::_t<std::make_unsigned<decltype(size)>>,
                        meta::_t<std::make_unsigned<size_type>>>;
                    RANGES_EXPECT(static_cast<C>(size) <= static_cast<C>(c.max_size()));
                    c.reserve(static_cast<size_type>(size));
                    using I = range_common_iterator_t<Rng>;
                    c.assign(I{ranges::begin(rng)}, I{ranges::end(rng)});
                    return c;
                }

            public:
                template<typename Rng,
                    typename Cont = meta::invoke<ContainerMetafunctionClass, range_value_type_t<Rng>>,
                    CONCEPT_REQUIRES_(InputRange<Rng>() && detail::ConvertibleToContainer<Rng, Cont>())>
                Cont operator()(Rng &&rng) const
                {
                    static_assert(!is_infinite<Rng>::value,
                        "Attempt to convert an infinite range to a container.");
                    return impl(static_cast<Rng &&>(rng), ReserveConcept<Cont, Rng>());
                }
            };
        }
        /// \endcond

        /// \addtogroup group-core
        /// @{

        /// \ingroup group-core
        RANGES_INLINE_VARIABLE(detail::to_container_fn<meta::quote<std::vector>>,
                               to_vector)


        /// \brief For initializing a container of the specified type with the elements of an Range
        template<template<typename...> class ContT>
        detail::to_container_fn<meta::quote<ContT>> to_()
        {
            return {};
        }

        /// \overload
        template<template<typename...> class ContT, typename Rng,
            typename Cont = meta::invoke<meta::quote<ContT>, range_value_type_t<Rng>>,
            CONCEPT_REQUIRES_(Range<Rng>() && detail::ConvertibleToContainer<Rng, Cont>())>
        Cont to_(Rng && rng)
        {
            return static_cast<Rng&&>(rng) | ranges::to_<ContT>();
        }

        /// \overload
        template<template<typename...> class ContT, typename T,
            typename Cont = meta::invoke<meta::quote<ContT>, T>,
            CONCEPT_REQUIRES_(detail::ConvertibleToContainer<std::initializer_list<T>, Cont>())>
        Cont to_(std::initializer_list<T> list)
        {
            return list | ranges::to_<ContT>();
        }

        /// \overload
        template<typename Cont>
        detail::to_container_fn<meta::id<Cont>> to_()
        {
            return {};
        }

        /// \overload
        template<typename Cont, typename Rng,
            CONCEPT_REQUIRES_(Range<Rng>() && detail::ConvertibleToContainer<Rng, Cont>())>
        Cont to_(Rng && rng)
        {
            return static_cast<Rng&&>(rng) | ranges::to_<Cont>();
        }

        /// \overload
        template<typename Cont, typename T,
            CONCEPT_REQUIRES_(detail::ConvertibleToContainer<std::initializer_list<T>, Cont>())>
        Cont to_(std::initializer_list<T> list)
        {
            return list | ranges::to_<Cont>();
        }

        /// @}
    }
}

#endif
