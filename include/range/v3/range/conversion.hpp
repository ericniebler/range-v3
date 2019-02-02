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
                CPP_template(typename Cont, typename Rng)(
                requires (!ToContainerReserve<Cont, Rng>))
                static Cont impl(Rng &&rng)
                {
                    using I = range_common_iterator_t<Rng>;
                    return Cont(I{ranges::begin(rng)}, I{ranges::end(rng)});
                }

                CPP_template(typename Cont, typename Rng)(
                requires ToContainerReserve<Cont, Rng>)
                static Cont impl(Rng &&rng)
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
                CPP_template(typename Rng)(
                    requires InputRange<Rng> &&
                        ConvertibleToContainer<Rng, container_t<Rng>>)
                container_t<Rng> operator()(Rng &&rng) const
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
    /// \endcond

        /// \brief For initializing a container of the specified type with the elements of an Range
        template<template<typename...> class ContT>
        auto to(RANGES_HIDDEN_DETAIL(detail::to_container = {})) ->
            detail::to_container_fn<meta::quote<ContT>>
        {
            return {};
        }

        /// \overload
        CPP_template(template<typename...> class ContT, typename Rng)(
            requires Range<Rng> &&
                detail::ConvertibleToContainer<Rng, ContT<range_value_t<Rng>>>)
        ContT<range_value_t<Rng>> to(Rng &&rng)
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
        CPP_template(typename Cont, typename Rng)(
            requires Range<Rng> && detail::ConvertibleToContainer<Rng, Cont>)
        Cont to(Rng &&rng)
        {
            return detail::to_container_fn<meta::id<Cont>>{}(static_cast<Rng &&>(rng));
        }

        /// \cond
        // Slightly odd initializer_list overloads, undocumented for now.
        CPP_template(template<typename...> class ContT, typename T)(
            requires detail::ConvertibleToContainer<std::initializer_list<T>, ContT<T>>)
        ContT<T> to(std::initializer_list<T> il)
        {
            return detail::to_container_fn<meta::quote<ContT>>{}(il);
        }
        CPP_template(typename Cont, typename T)(
            requires detail::ConvertibleToContainer<std::initializer_list<T>, Cont>)
        Cont to(std::initializer_list<T> il)
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
        CPP_template(template<typename...> class ContT, typename Rng)(
            requires Range<Rng> &&
                detail::ConvertibleToContainer<Rng, ContT<range_value_t<Rng>>>)
        RANGES_EMPTY_ATTR RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        ContT<range_value_t<Rng>> to_(Rng &&rng)
        {
            return static_cast<Rng &&>(rng) | ranges::to_<ContT>();
        }
        CPP_template(template<typename...> class ContT, typename T)(
            requires detail::ConvertibleToContainer<std::initializer_list<T>, ContT<T>>)
        RANGES_EMPTY_ATTR RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        ContT<T> to_(std::initializer_list<T> il)
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
        CPP_template(typename Cont, typename Rng)(
            requires Range<Rng> && detail::ConvertibleToContainer<Rng, Cont>)
        RANGES_EMPTY_ATTR RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        Cont to_(Rng &&rng)
        {
            return static_cast<Rng &&>(rng) | ranges::to_<Cont>();
        }
        CPP_template(typename Cont, typename T)(
            requires detail::ConvertibleToContainer<std::initializer_list<T>, Cont>)
        RANGES_EMPTY_ATTR RANGES_DEPRECATED("Please use ranges::to (no underscore) instead.")
        Cont to_(std::initializer_list<T> list)
        {
            return list | ranges::to_<Cont>();
        }
    } // namespace _to_
    /// \endcond
}

#endif
