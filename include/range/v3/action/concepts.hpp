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

#ifndef RANGES_V3_ACTION_CONCEPTS_HPP
#define RANGES_V3_ACTION_CONCEPTS_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        template<typename T>
        struct movable_input_iterator
        {
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T *;
            using reference = T &&;

            movable_input_iterator() = default;
            movable_input_iterator &operator++();
            movable_input_iterator operator++(int);
            bool operator==(movable_input_iterator const &) const;
            bool operator!=(movable_input_iterator const &) const;
            T && operator*() const;
        };
    }
    /// \endcond

    /// \addtogroup group-concepts
    /// @{

    // std::array is a SemiContainer, native arrays are not.
    CPP_def
    (
        template(typename T)
        concept SemiContainer,
            ForwardRange<T> && DefaultConstructible<uncvref_t<T>> &&
            Movable<uncvref_t<T>> &&
            !View<T>
    );

    // std::vector is a Container, std::array is not
    CPP_def
    (
        template(typename T)
        concept Container,
            SemiContainer<T> &&
            Constructible<
                uncvref_t<T>,
                detail::movable_input_iterator<range_value_t<T>>,
                detail::movable_input_iterator<range_value_t<T>>>
    );

    CPP_def
    (
        template(typename C)
        concept Reservable,
            requires (C &c, C const &cc, range_size_t<C> s)
            (
                c.reserve(s),
                cc.capacity(),
                cc.max_size(),
                concepts::requires_<Same<decltype(cc.capacity()), range_size_t<C>>>,
                concepts::requires_<Same<decltype(cc.max_size()), range_size_t<C>>>
            ) &&
            Container<C> && SizedRange<C>
    );

    CPP_def
    (
        template(typename C, typename I)
        concept ReserveAndAssignable,
            requires (C &c, I i)
            (
                c.assign(i, i)
            ) &&
            Reservable<C> && InputIterator<I>
    );

    CPP_def
    (
        template(typename C)
        concept RandomAccessReservable,
            Reservable<C> && RandomAccessRange<C>
    );

    /// \cond
    namespace detail
    {
        template<typename T>
        auto is_lvalue_container_like(T &) noexcept ->
            CPP_ret(std::true_type)(
                requires Container<T>)
        {
            return {};
        }

        template<typename T>
        auto is_lvalue_container_like(reference_wrapper<T>) noexcept ->
          CPP_ret(meta::not_<std::is_rvalue_reference<T>>)(
              requires Container<T>)
        {
            return {};
        }

        template<typename T>
        auto is_lvalue_container_like(std::reference_wrapper<T>) noexcept ->
            CPP_ret(std::true_type)(
                requires Container<T>)
        {
            return {};
        }

        template<typename T>
        auto is_lvalue_container_like(ref_view<T>) noexcept ->
            CPP_ret(std::true_type)(
                requires Container<T>)
        {
            return {};
        }
    }
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept LvalueContainerLike,
            requires (T &&t)
            (
                concepts::implicitly_convertible_to<std::true_type>(
                    detail::is_lvalue_container_like((T &&) t))
            ) &&
            ForwardRange<T>
    );
    /// @}
}

#endif
