/// \file
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

#ifndef RANGES_V3_VIEW_INTS_HPP
#define RANGES_V3_VIEW_INTS_HPP

#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace view
        {
            struct ints_fn
              : iota_view<int>
            {
                ints_fn() = default;

                template<typename Val,
                    CONCEPT_REQUIRES_(Integral<Val>())>
                iota_view<Val> operator()(Val value) const
                {
                    return iota_view<Val>{value};
                }
                template<typename Val>
                meta::if_c<
                    (bool)Integral<Val>(),
                    detail::take_exactly_view_<iota_view<Val>, true>>
                operator()(Val from, Val to) const;

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Val,
                    CONCEPT_REQUIRES_(!Integral<Val>())>
                void operator()(Val) const
                {
                    CONCEPT_ASSERT_MSG(Integral<Val>(),
                        "The object passed to view::ints must be Integral");
                }
                template<typename Val,
                    CONCEPT_REQUIRES_(!Integral<Val>())>
                void operator()(Val, Val) const
                {
                    CONCEPT_ASSERT_MSG(Integral<Val>(),
                        "The object passed to view::ints must be Integral");
                }
            #endif
            };

            template<typename Val>
            meta::if_c<
                (bool)Integral<Val>(),
                detail::take_exactly_view_<iota_view<Val>, true>>
            ints_fn::operator()(Val from, Val to) const
            {
                return {iota_view<Val>{from}, detail::iota_minus(to, from)};
            }

            struct closed_ints_fn
            {
                template<typename Val,
                    CONCEPT_REQUIRES_(Integral<Val>())>
                detail::take_exactly_view_<iota_view<Val>, true> operator()(Val from, Val to) const
                {
                    return {iota_view<Val>{from}, detail::iota_minus(to, from) + 1};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Val,
                    CONCEPT_REQUIRES_(!Integral<Val>())>
                void operator()(Val, Val) const
                {
                    CONCEPT_ASSERT_MSG(Integral<Val>(),
                        "The object passed to view::closed_ints must be Integral");
                }
            #endif
            };

            /// \relates ints_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& ints = static_const<ints_fn>::value;
            }

            /// \relates closed_ints_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& closed_ints = static_const<closed_ints_fn>::value;
            }
        }
        /// @}
    }
}

#endif
