/// \file
// Range v3 library
//
//  Copyright Barry Revzin 2019-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_REMOVE_WHEN_HPP
#define RANGES_V3_VIEW_REMOVE_WHEN_HPP

#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/compose.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/detail/delimiter_specifier.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Fun>
    struct RANGES_EMPTY_BASES remove_when_view
      : view_adaptor<remove_when_view<Rng, Fun>, Rng,
                     is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
      , private box<semiregular_t<Fun>>
    {
        remove_when_view() = default;
        constexpr remove_when_view(Rng rng, Fun fun)
          : remove_when_view::view_adaptor{detail::move(rng)}
          , remove_when_view::box(detail::move(fun))
        {}

    private:
        friend range_access;
        bool zero_;

        struct adaptor : adaptor_base
        {
            adaptor() = default;
            constexpr adaptor(remove_when_view & rng) noexcept
              : rng_(&rng)
            {}
            static constexpr iterator_t<Rng> begin(remove_when_view & rng)
            {
                return *rng.begin_;
            }
            constexpr void next(iterator_t<Rng> & it) const
            {
                RANGES_ASSERT(it != ranges::end(rng_->base()));
                rng_->satisfy_forward(++it);
            }
            void advance() = delete;
            void distance_to() = delete;

        private:
            remove_when_view * rng_;
        };
        constexpr adaptor begin_adaptor()
        {
            cache_begin();
            return {*this};
        }
        constexpr adaptor end_adaptor()
        {
            return {*this};
        }

        constexpr void satisfy_forward(iterator_t<Rng> & it)
        {
            if(zero_)
            {
                // if the last match consumed zero elements, we already bumped the
                // position so can stop here.
                zero_ = false;
                return;
            }

            auto const last = ranges::end(this->base());
            auto & fun = this->remove_when_view::box::get();
            if(it != last)
            {
                auto p = invoke(fun, it, last);
                if(p.first)
                {
                    zero_ = (it == p.second);
                    it = p.second;
                }
            }
        }

        constexpr void cache_begin()
        {
            if(begin_)
                return;
            auto it = ranges::begin(this->base());
            zero_ = false;
            satisfy_forward(it);
            begin_.emplace(std::move(it));
        }

        detail::non_propagating_cache<iterator_t<Rng>> begin_;
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng, typename Fun)(requires CopyConstructible<Fun>)
        remove_when_view(Rng &&, Fun)
            ->remove_when_view<view::all_t<Rng>, Fun>;
#endif

    namespace view
    {
        struct remove_when_fn
        {
        private:
            friend view_access;
            template<typename Fun>
            static auto bind(remove_when_fn remove_when, Fun && fun)
            {
                return make_pipeable(std::bind(
                    remove_when, std::placeholders::_1, static_cast<Fun &&>(fun)));
            }

        public:
            template<typename Rng, typename Fun>
            constexpr auto operator()(Rng && rng, Fun fun) const
                -> CPP_ret(remove_when_view<all_t<Rng>, detail::delimiter_specifier<Fun>>)( //
                    requires ViewableRange<Rng> && InputRange<Rng> && Predicate<
                        Fun const &, range_reference_t<Rng>> && CopyConstructible<Fun>)
            {
                return {all(static_cast<Rng &&>(rng)),
                        detail::delimiter_specifier<Fun>{std::move(fun)}};
            }
            template<typename Rng, typename Fun>
            auto operator()(Rng && rng, Fun fun) const -> CPP_ret(
                remove_when_view<all_t<Rng>, Fun>)( //
                requires ViewableRange<Rng> && ForwardRange<Rng> &&
                    Invocable<Fun &, iterator_t<Rng>, sentinel_t<Rng>> &&
                        Invocable<Fun &, iterator_t<Rng>, iterator_t<Rng>> &&
                            CopyConstructible<Fun> && ConvertibleTo<
                                invoke_result_t<Fun &, iterator_t<Rng>, sentinel_t<Rng>>,
                                std::pair<bool, iterator_t<Rng>>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(fun)};
            }
        };

        /// \relates remove_when_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<remove_when_fn>, remove_when)
    } // namespace view
    /// @}
} // namespace ranges

RANGES_RE_ENABLE_WARNINGS

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::remove_when_view)

#endif
