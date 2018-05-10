/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_VIEW_FACADE_HPP
#define RANGES_V3_VIEW_FACADE_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_access.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/basic_iterator.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Derived>
            using begin_cursor_t =
                detail::decay_t<decltype(range_access::begin_cursor(std::declval<Derived &>(), 42))>;

            template<typename Derived>
            using end_cursor_t =
                detail::decay_t<decltype(range_access::end_cursor(std::declval<Derived &>(), 42))>;

            template<typename Derived>
            using facade_iterator_t = basic_iterator<begin_cursor_t<Derived>>;

            template<typename Derived>
            using facade_sentinel_t =
                meta::if_<
                    Same<begin_cursor_t<Derived>, end_cursor_t<Derived>>,
                    facade_iterator_t<Derived>,
                    end_cursor_t<Derived>>;
        }
        /// \endcond

        /// \addtogroup group-core
        /// @{

        /// \brief A utility for constructing a view from a (derived) type that
        /// implements begin and end cursors.
        /// \tparam Derived A type that derives from `view_facade` and implements
        /// begin and end cursors. This type is permitted to be incomplete.
        /// \tparam Cardinality The cardinality of this view: `finite`, `infinite`,
        /// or `unknown`. See `ranges::v3::cardinality`.
        template<typename Derived, cardinality Cardinality>
        struct view_facade
          : view_interface<Derived, Cardinality>
        {
        protected:
            friend range_access;
            using view_interface<Derived, Cardinality>::derived;
            // Default implementations
            Derived begin_cursor() const
            {
                return derived();
            }
            constexpr default_sentinel end_cursor() const
            {
                return {};
            }
        public:
            /// Let `d` be `static_cast<Derived &>(*this)`. Let `b` be
            /// `std::as_const(d).begin_cursor()` if that expression is well-formed;
            /// otherwise, let `b` be `d.begin_cursor()`. Let `B` be the type of
            /// `b`.
            /// \return `ranges::v3::basic_iterator<B>(b)`
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D> begin()
            {
                return detail::facade_iterator_t<D>{
                    range_access::begin_cursor(derived(), 42)};
            }
            /// \overload
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D const> begin() const
            {
                return detail::facade_iterator_t<D const>{
                    range_access::begin_cursor(derived(), 42)};
            }
            /// Let `d` be `static_cast<Derived &>(*this)`. Let `e` be
            /// `std::as_const(d).end_cursor()` if that expression is well-formed;
            /// otherwise, let `e` be `d.end_cursor()`. Let `E` be the type of
            /// `e`.
            /// \return `ranges::v3::basic_iterator<E>(e)` if `E` is the same
            /// as `B` computed above for `begin()`; otherwise, return `e`.
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D> end()
            {
                return static_cast<detail::facade_sentinel_t<D>>(
                    range_access::end_cursor(derived(), 42));
            }
            /// \overload
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D const> end() const
            {
                return static_cast<detail::facade_sentinel_t<D const>>(
                    range_access::end_cursor(derived(), 42));
            }
        };

        /// @}
    }
}

#endif
