/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_INDICES_HPP
#define RANGES_V3_VIEW_INDICES_HPP

#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/iota.hpp>

namespace ranges
{
    inline namespace v3
    {

        namespace view
        {
            /// Half-open range of indices: [from, to).
            struct indices_fn
              : iota_view<std::ptrdiff_t>
            {
                indices_fn() = default;

                CPP_template(typename Val)(
                    requires Integral<Val>)
                auto operator()(Val from, Val to) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    detail::take_exactly_view_<iota_view<Val>, true>
                        {iota_view<Val>{from}, detail::ints_open_distance_(from, to)}
                )

                CPP_template(typename Val, typename Self = indices_fn)(
                    requires Integral<Val>)
                auto operator()(Val to) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Self{}(Val(), to)
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename Val)(
                    requires not Integral<Val>)
                void operator()(Val) const
                {
                    CPP_assert_msg(Integral<Val>,
                        "The object passed to view::indices must be Integral");
                }
                CPP_template(typename Val)(
                    requires not Integral<Val>)
                void operator()(Val, Val) const
                {
                    CPP_assert_msg(Integral<Val>,
                        "The object passed to view::indices must be Integral");
                }
            #endif
            };

            /// Inclusive range of indices: [from, to].
            struct closed_indices_fn
              : iota_view<std::ptrdiff_t>
            {
                CPP_template(typename Val)(
                    requires Integral<Val>)
                auto operator()(Val from, Val to) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    detail::take_exactly_view_<iota_view<Val>, true>
                        {iota_view<Val>{from}, detail::ints_closed_distance_(from, to)}
                )

                CPP_template(typename Val, typename Self = closed_indices_fn)(
                    requires Integral<Val>)
                auto operator()(Val to) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Self{}(Val(), to)
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                CPP_template(typename Val)(
                    requires not Integral<Val>)
                void operator()(Val) const
                {
                    CPP_assert_msg(Integral<Val>,
                        "The object passed to view::closed_indices must be Integral");
                }
                CPP_template(typename Val)(
                    requires not Integral<Val>)
                void operator()(Val, Val) const
                {
                    CPP_assert_msg(Integral<Val>,
                        "The object passed to view::closed_indices must be Integral");
                }
            #endif
            };

            /// \relates indices_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(indices_fn, indices)

            /// \relates closed_indices_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(closed_indices_fn, closed_indices)

        }  // namespace view
    }
}

#endif  // RANGES_V3_VIEW_INDICES_HPP
