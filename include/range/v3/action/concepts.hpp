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
            movable_input_iterator & operator++();
            movable_input_iterator operator++(int);
            bool operator==(movable_input_iterator const &) const;
            bool operator!=(movable_input_iterator const &) const;
            T && operator*() const;
        };
    } // namespace detail
      /// \endcond

    /// \addtogroup group-range
    /// @{

    // std::array is a semi_container, native arrays are not.
    // clang-format off
    CPP_def
    (
        template(typename T)
        concept semi_container,
            forward_range<T> && default_constructible<uncvref_t<T>> &&
            movable<uncvref_t<T>> &&
            !view_<T>
    );

    // std::vector is a container, std::array is not
    CPP_def
    (
        template(typename T)
        concept container,
            semi_container<T> &&
            constructible_from<
                uncvref_t<T>,
                detail::movable_input_iterator<range_value_t<T>>,
                detail::movable_input_iterator<range_value_t<T>>>
    );

    CPP_def
    (
        template(typename C)
        concept reservable,
            requires (C &c, C const &cc, range_size_t<C> s)
            (
                c.reserve(s),
                cc.capacity(),
                cc.max_size(),
                concepts::requires_<same_as<decltype(cc.capacity()), range_size_t<C>>>,
                concepts::requires_<same_as<decltype(cc.max_size()), range_size_t<C>>>
            ) &&
            container<C> && sized_range<C>
    );

    CPP_def
    (
        template(typename C, typename I)
        concept reservable_with_assign,
            requires (C &c, I i)
            (
                c.assign(i, i)
            ) &&
            reservable<C> && input_iterator<I>
    );

    CPP_def
    (
        template(typename C)
        concept random_access_reservable,
            reservable<C> && random_access_range<C>
    );
    // clang-format on

    /// \cond
    namespace detail
    {
        template<typename T>
        auto is_lvalue_container_like(T &) noexcept -> CPP_ret(std::true_type)( //
            requires container<T>)
        {
            return {};
        }

        template<typename T>
        auto is_lvalue_container_like(reference_wrapper<T>) noexcept
            -> CPP_ret(meta::not_<std::is_rvalue_reference<T>>)( //
                requires container<T>)
        {
            return {};
        }

        template<typename T>
        auto is_lvalue_container_like(std::reference_wrapper<T>) noexcept
            -> CPP_ret(std::true_type)( //
                requires container<T>)
        {
            return {};
        }

        template<typename T>
        auto is_lvalue_container_like(ref_view<T>) noexcept -> CPP_ret(std::true_type)( //
            requires container<T>)
        {
            return {};
        }

        template<typename T>
        using is_lvalue_container_like_t =
            decltype(detail::is_lvalue_container_like(std::declval<T>()));

    } // namespace detail
    /// \endcond

    // clang-format off
    CPP_def
    (
        template(typename T)
        concept lvalue_container_like,
            implicitly_convertible_to<detail::is_lvalue_container_like_t<T>,
                                      std::true_type> &&
            forward_range<T>
    );
    // clang-format on
    /// @}
} // namespace ranges

#endif
