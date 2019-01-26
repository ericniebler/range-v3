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

#ifndef RANGES_V3_RANGE_CONVERSION_HPP
#define RANGES_V3_RANGE_CONVERSION_HPP

#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/functional/pipeable.hpp>

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
                Range<Cont> && (!View<Cont>) && MoveConstructible<Cont> &&
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

        struct to_container
        {
            template<typename ToContainer>
            struct fn
              : pipeable<fn<ToContainer>>
            {
            private:
                template<typename Cont, typename Rng>
                static auto impl(Rng &&rng) -> CPP_ret(Cont)(
                    requires (!ToContainerReserve<Cont, Rng>))
                {
                    using I = range_common_iterator_t<Rng>;
                    return Cont(I{ranges::begin(rng)}, I{ranges::end(rng)});
                }

                template<typename Cont, typename Rng>
                static auto impl(Rng &&rng) -> CPP_ret(Cont)(
                    requires ToContainerReserve<Cont, Rng>)
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

                template<typename Rng>
                using container_t = meta::invoke<ToContainer, range_value_t<Rng>>;

            public:
                template<typename Rng>
                auto operator()(Rng &&rng) const ->
                    CPP_ret(container_t<Rng>)(
                        requires InputRange<Rng> &&
                            ConvertibleToContainer<Rng, container_t<Rng>>)
                {
                    static_assert(!is_infinite<Rng>::value,
                        "Attempt to convert an infinite range to a container.");
                    return impl<container_t<Rng>>(static_cast<Rng &&>(rng));
                }
            };

            template<typename ToContainer, typename Rng>
            using container_t = meta::invoke<ToContainer, range_value_t<Rng>>;

            template<typename Rng, typename ToContainer>
            friend auto operator|(Rng &&rng, fn<ToContainer>(*)(to_container)) ->
                CPP_broken_friend_ret(container_t<ToContainer, Rng>)(
                    requires InputRange<Rng> &&
                        ConvertibleToContainer<Rng, container_t<ToContainer, Rng>>)
            {
                return fn<ToContainer>{}(static_cast<Rng &&>(rng));
            }
        };

        template<typename ToContainer>
        using to_container_fn = to_container::fn<ToContainer>;
    }
    /// \endcond

    /// \addtogroup group-core
    /// @{

    /// \ingroup group-core
    RANGES_INLINE_VARIABLE(
        detail::to_container_fn<meta::quote<std::vector>>,
        to_vector)

    /// \cond
    namespace _to_
    {
    /// \endond

        /// \brief For initializing a container of the specified type with the elements of an Range
        template<template<typename...> class ContT>
        auto to(RANGES_HIDDEN_DETAIL(detail::to_container = {})) ->
            detail::to_container_fn<meta::quote<ContT>>
        {
            return {};
        }

        /// \overload
        template<template<typename...> class ContT, typename Rng>
        auto to(Rng &&rng) ->
            CPP_ret(ContT<range_value_t<Rng>>)(
                requires Range<Rng> &&
                    detail::ConvertibleToContainer<Rng, ContT<range_value_t<Rng>>>)
        {
            return detail::to_container_fn<meta::quote<ContT>>{}(static_cast<Rng &&>(rng));
        }

        /// \overload
        template<typename Cont>
        auto to(RANGES_HIDDEN_DETAIL(detail::to_container = {})) ->
            detail::to_container_fn<meta::id<Cont>>
        {
            return {};
        }

        /// \overload
        template<typename Cont, typename Rng>
        auto to(Rng &&rng) ->
            CPP_ret(Cont)(
                requires Range<Rng> && detail::ConvertibleToContainer<Rng, Cont>)
        {
            return detail::to_container_fn<meta::id<Cont>>{}(static_cast<Rng &&>(rng));
        }

        /// \cond
        // Slightly odd initializer_list overloads, undocumented for now.
        template<template<typename...> class ContT, typename T>
        auto to(std::initializer_list<T> il) ->
            CPP_ret(ContT<T>)(
                requires detail::ConvertibleToContainer<std::initializer_list<T>, ContT<T>>)
        {
            return detail::to_container_fn<meta::quote<ContT>>{}(il);
        }
        template<typename Cont, typename T>
        auto to(std::initializer_list<T> il) ->
            CPP_ret(Cont)(
                requires detail::ConvertibleToContainer<std::initializer_list<T>, Cont>)
        {
            return detail::to_container_fn<meta::id<Cont>>{}(il);
        }
        /// \endcond

    /// \cond
    } // namespace _to_
    using namespace _to_;
    /// \endcond
    /// @}

    ////////////////////////////////////////////////////////////////////////////
    /// \cond
    namespace _to_
    {
        // The old name "ranges::to_" is now deprecated:
        template<template<typename...> class ContT>
        RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        auto to_(detail::to_container = {}) ->
            detail::to_container_fn<meta::quote<ContT>>
        {
            return {};
        }
        template<template<typename...> class ContT, typename Rng>
        RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        auto to_(Rng &&rng) ->
            CPP_ret(ContT<range_value_t<Rng>>)(
                requires Range<Rng> &&
                    detail::ConvertibleToContainer<Rng, ContT<range_value_t<Rng>>>)
        {
            return static_cast<Rng &&>(rng) | ranges::to_<ContT>();
        }
        template<template<typename...> class ContT, typename T>
        RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        auto to_(std::initializer_list<T> il) ->
            CPP_ret(ContT<T>)(
                requires detail::ConvertibleToContainer<std::initializer_list<T>, ContT<T>>)
        {
            return il | ranges::to_<ContT>();
        }
        template<typename Cont>
        RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        auto to_(detail::to_container = {}) ->
            detail::to_container_fn<meta::id<Cont>>
        {
            return {};
        }
        template<typename Cont, typename Rng>
        RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        auto to_(Rng &&rng) ->
            CPP_ret(Cont)(
                requires Range<Rng> && detail::ConvertibleToContainer<Rng, Cont>)
        {
            return static_cast<Rng &&>(rng) | ranges::to_<Cont>();
        }
        template<typename Cont, typename T>
        RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        auto to_(std::initializer_list<T> list) ->
            CPP_ret(Cont)(
                requires detail::ConvertibleToContainer<std::initializer_list<T>, Cont>)
        {
            return list | ranges::to_<Cont>();
        }
    } // namespace _to_
    /// \endcond
}

#endif
