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

#ifndef RANGES_V3_VIEW_MOVE_HPP
#define RANGES_V3_VIEW_MOVE_HPP

#include <utility>
#include <type_traits>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng>
        struct move_view
          : view_adaptor<move_view<Rng>, Rng>
        {
        private:
            friend range_access;
            template<bool Const>
            struct adaptor
              : adaptor_base
            {
                adaptor() = default;
                template<bool Other>
                constexpr CPP_ctor(adaptor)(adaptor<Other>)(
                    requires Const && !Other)
                {}
                using CRng = meta::const_if_c<Const, Rng>;
                using value_type = range_value_t<Rng>;
                range_rvalue_reference_t<CRng> read(iterator_t<CRng> const &it) const
                {
                    return ranges::iter_move(it);
                }
                range_rvalue_reference_t<CRng> iter_move(iterator_t<CRng> const &it) const
                {
                    return ranges::iter_move(it);
                }
            };
            adaptor<simple_view<Rng>()> begin_adaptor()
            {
                return {};
            }
            adaptor<simple_view<Rng>()> end_adaptor()
            {
                return {};
            }
            CPP_member
            auto begin_adaptor() const -> CPP_ret(adaptor<true>)(
                requires InputRange<Rng const>)
            {
                return {};
            }
            CPP_member
            auto end_adaptor() const -> CPP_ret(adaptor<true>)(
                requires InputRange<Rng const>)
            {
                return {};
            }
        public:
            move_view() = default;
            explicit move_view(Rng rng)
              : move_view::view_adaptor{std::move(rng)}
            {}
            CPP_member
            auto CPP_fun(size)() (const
                requires SizedRange<Rng const>)
            {
                return ranges::size(this->base());
            }
            CPP_member
            auto CPP_fun(size)() (
                requires SizedRange<Rng>)
            {
                return ranges::size(this->base());
            }
        };

        namespace view
        {
            struct move_fn
            {
                template<typename Rng>
                auto operator()(Rng &&rng) const ->
                    CPP_ret(move_view<all_t<Rng>>)(
                        requires InputRange<Rng>)
                {
                    return move_view<all_t<Rng>>{all(static_cast<Rng &&>(rng))};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng>
                auto operator()(Rng &&) const ->
                    CPP_ret(void)(
                        requires not InputRange<Rng>)
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The argument passed to view::move must be a model of the InputRange "
                        "concept.");
                }
            #endif
            };

            /// \relates move_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<move_fn>, move)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::move_view)

#endif
