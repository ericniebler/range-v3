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
    template<typename Value, typename Alloc /*= allocator<Value>*/>
    class vector;
RANGES_END_NAMESPACE_STD
#else
#include <vector>
#endif

namespace ranges
{
    /// \cond
    namespace detail
    {
        CPP_def
        (
            template(typename Cont)
            concept HasAllocatorType,
                Type<typename Cont::allocator_type>
        );

        CPP_def
        (
            template(typename Rng, typename Cont)
            concept ConvertibleToContainerImpl,
                Range<Cont> && !View<Cont> && MoveConstructible<Cont> &&
                ConvertibleTo<range_value_t<Rng>, range_value_t<Cont>> &&
                Constructible<Cont, range_common_iterator_t<Rng>, range_common_iterator_t<Rng>>
        );

        CPP_def
        (
            template(typename Rng, typename Cont)
            concept ConvertibleToContainer,
                defer::HasAllocatorType<Cont> && // HACKHACK
                defer::ConvertibleToContainerImpl<Rng, Cont>
        );

        CPP_def
        (
            template(typename C, typename R)
            concept ToContainerReserve,
                ReserveAndAssignable<C, range_common_iterator_t<R>> &&
                SizedRange<R>
        );

        template<typename ToContainer>
        struct to_container_fn
          : pipeable<to_container_fn<ToContainer>>
        {
        private:
            CPP_template(typename Cont, typename Rng)(
                requires not ToContainerReserve<Cont, Rng>)
            Cont impl(Rng &&rng) const
            {
                using I = range_common_iterator_t<Rng>;
                return Cont(I{ranges::begin(rng)}, I{ranges::end(rng)});
            }

            CPP_template(typename Cont, typename Rng)(
                requires ToContainerReserve<Cont, Rng>)
            Cont impl(Rng &&rng) const
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
            CPP_template(typename Rng,
                typename Cont = meta::invoke<ToContainer, range_value_t<Rng>>)(
                requires InputRange<Rng> && detail::ConvertibleToContainer<Rng, Cont>)
            Cont operator()(Rng &&rng) const
            {
                static_assert(!is_infinite<Rng>::value,
                    "Attempt to convert an infinite range to a container.");
                return impl<Cont>(static_cast<Rng &&>(rng));
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
    auto to_() ->
        detail::to_container_fn<meta::quote<ContT>>
    {
        return {};
    }

    /// \overload
    template<template<typename...> class ContT, typename Rng>
    auto to_(Rng &&rng) ->
        CPP_ret(ContT<range_value_t<Rng>>)(
            requires Range<Rng> &&
                detail::ConvertibleToContainer<Rng, ContT<range_value_t<Rng>>>)
    {
        return static_cast<Rng &&>(rng) | ranges::to_<ContT>();
    }

    /// \overload
    template<template<typename...> class ContT, typename T>
    auto to_(std::initializer_list<T> il) ->
        CPP_ret(ContT<T>)(
            requires detail::ConvertibleToContainer<std::initializer_list<T>, ContT<T>>)
    {
        return il | ranges::to_<ContT>();
    }

    /// \overload
    template<typename Cont>
    auto to_() ->
        detail::to_container_fn<meta::id<Cont>>
    {
        return {};
    }

    /// \overload
    template<typename Cont, typename Rng>
    auto to_(Rng &&rng) ->
        CPP_ret(Cont)(
            requires Range<Rng> && detail::ConvertibleToContainer<Rng, Cont>)
    {
        return static_cast<Rng &&>(rng) | ranges::to_<Cont>();
    }

    /// \overload
    template<typename Cont, typename T>
    auto to_(std::initializer_list<T> list) ->
        CPP_ret(Cont)(
            requires detail::ConvertibleToContainer<std::initializer_list<T>, Cont>)
    {
        return list | ranges::to_<Cont>();
    }

    /// @}
}

#endif
