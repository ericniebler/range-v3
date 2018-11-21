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

#ifndef RANGES_V3_VIEW_INDIRECT_HPP
#define RANGES_V3_VIEW_INDIRECT_HPP

#include <iterator>
#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct indirect_view
          : view_adaptor<indirect_view<Rng>, Rng>
        {
        private:
            friend range_access;
            struct adaptor
              : adaptor_base
            {
                constexpr auto CPP_auto_fun(read)(iterator_t<Rng> const &it) (const)
                (
                    return **it
                )
                constexpr auto CPP_auto_fun(iter_move)(iterator_t<Rng> const &it) (const)
                (
                    return ranges::iter_move(*it)
                )
            };
            constexpr adaptor begin_adaptor() const noexcept
            {
                return {};
            }
            constexpr adaptor end_adaptor() const noexcept
            {
                return {};
            }
        public:
            indirect_view() = default;
            explicit constexpr indirect_view(Rng rng)
              : indirect_view::view_adaptor{detail::move(rng)}
            {}
            CPP_member
            constexpr auto CPP_fun(size)() (const
                requires SizedRange<Rng const>)
            {
                return ranges::size(this->base());
            }
            CPP_member
            constexpr /*c++14*/ auto CPP_fun(size)() (
                requires SizedRange<Rng>)
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            CPP_def
            (
                template(typename Rng)
                concept ReadableRange,
                    InputRange<Rng> &&
                    Readable<range_value_t<Rng>>
            );

            struct indirect_fn
            {
                CPP_template(typename Rng)(
                    requires ReadableRange<Rng>)
                constexpr auto CPP_auto_fun(operator())(Rng &&rng) (const)
                (
                    return indirect_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))}
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng>
                auto operator()(Rng &&) const ->
                    CPP_ret(void)(
                        requires not ReadableRange<Rng>)
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The argument to view::indirect must be a model of the InputRange "
                        "concept");
                    CPP_assert_msg(Readable<range_value_t<Rng>>,
                        "The value type of the range passed to view::indirect must be a model "
                        "of the Readable concept.");
                }
            #endif
            };

            /// \relates indirect_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<indirect_fn>, indirect)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::indirect_view)

#endif
