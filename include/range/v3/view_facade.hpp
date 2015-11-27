/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
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
                decltype(range_access::begin_cursor(std::declval<Derived &>(), 42));

            template<typename Derived>
            using end_cursor_t =
                decltype(range_access::end_cursor(std::declval<Derived &>(), 42));

            template<typename Derived>
            using facade_iterator_t =
                basic_iterator<begin_cursor_t<Derived>, end_cursor_t<Derived>>;

            template<typename Derived>
            using facade_sentinel_t =
                meta::if_<
                    Same<begin_cursor_t<Derived>, end_cursor_t<Derived>>,
                    basic_iterator<begin_cursor_t<Derived>, end_cursor_t<Derived>>,
                    basic_sentinel<end_cursor_t<Derived>>>;
        }
        /// \endcond

        /// \addtogroup group-core
        /// @{
        template<typename Derived, cardinality Cardinality>
        struct view_facade
          : view_interface<Derived, Cardinality>
        {
        protected:
            friend range_access;
            using view_facade_t = view_facade;
            using view_interface<Derived, Cardinality>::derived;
            // Default implementations
            Derived begin_cursor() const
            {
                return derived();
            }
            default_end_cursor end_cursor() const
            {
                return {};
            }
        public:
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D> begin()
            {
                return {range_access::begin_cursor(derived(), 42)};
            }
            /// \overload
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_iterator_t<D const> begin() const
            {
                return {range_access::begin_cursor(derived(), 42)};
            }
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D> end()
            {
                return {range_access::end_cursor(derived(), 42)};
            }
            /// \overload
            template<typename D = Derived, CONCEPT_REQUIRES_(Same<D, Derived>())>
            detail::facade_sentinel_t<D const> end() const
            {
                return {range_access::end_cursor(derived(), 42)};
            }
        };

        template<typename RangeFacade>
        using view_facade_t = meta::_t<range_access::view_facade<RangeFacade>>;

        /// @}
    }
}

#endif
