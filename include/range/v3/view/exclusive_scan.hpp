/// \file
// Range v3 library
//
//  Copyright Mitsutaka Takeda 2018-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGE_V3_VIEW_EXCLUSIVE_SCAN_HPP
#define RANGE_V3_VIEW_EXCLUSIVE_SCAN_HPP

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    // clang-format off
    CPP_def
    (
        template(typename Rng, typename T, typename Fun)
        concept exclusive_scan_constraints,
            viewable_range<Rng> && input_range<Rng> &&
            copy_constructible<T> &&
            invocable<Fun &, T, range_reference_t<Rng>> &&
            assignable_from<T &, invoke_result_t<Fun &, T, range_reference_t<Rng>>>
    );
    // clang-format on

    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename T, typename Fun>
    struct exclusive_scan_view : view_adaptor<exclusive_scan_view<Rng, T, Fun>, Rng>
    {
    private:
        friend range_access;
        CPP_assert(exclusive_scan_constraints<Rng, T, Fun>);

        semiregular_box_t<T> init_;
        semiregular_box_t<Fun> fun_;
        using single_pass = meta::bool_<single_pass_iterator_<iterator_t<Rng>>>;
        using use_sentinel_t = meta::bool_<!common_range<Rng> || single_pass{}>;

        template<bool IsConst>
        struct adaptor : adaptor_base
        {
        private:
            friend struct adaptor<!IsConst>;
            using exclusive_scan_view_t = meta::const_if_c<IsConst, exclusive_scan_view>;
            using CRng = meta::const_if_c<IsConst, Rng>;
            semiregular_box_t<T> sum_;
            exclusive_scan_view_t * rng_;

            // clang-format off
            auto CPP_auto_fun(move_or_copy_init)(std::false_type)
            (
                return (rng_->init_)
            )

            // If the base range is single-pass, we can move the init value.
            auto CPP_auto_fun(move_or_copy_init)(std::true_type)
            (
                return std::move(rng_->init_)
            )
                // clang-format on
                public : using single_pass = exclusive_scan_view::single_pass;
            adaptor() = default;
            adaptor(exclusive_scan_view_t * rng)
              : rng_(rng)
            {}
            CPP_template(bool Other)( //
                requires IsConst && (!Other)) adaptor(adaptor<Other> that)
              : rng_(that.rng_)
            {}
            iterator_t<CRng> begin(exclusive_scan_view_t &)
            {
                sum_ = move_or_copy_init(single_pass{});
                return ranges::begin(rng_->base());
            }
            T read(iterator_t<CRng> const &) const
            {
                return sum_;
            }
            void next(iterator_t<CRng> & it)
            {
                RANGES_EXPECT(it != ranges::end(rng_->base()));
                sum_ = invoke(rng_->fun_, static_cast<T &&>(std::move(sum_)), *it);
                ++it;
            }
            void prev() = delete;
        };

        adaptor<false> begin_adaptor()
        {
            return {this};
        }
        meta::if_<use_sentinel_t, adaptor_base, adaptor<false>> end_adaptor()
        {
            return {this};
        }
        CPP_member
        auto begin_adaptor() const -> CPP_ret(adaptor<true>)( //
            requires exclusive_scan_constraints<Rng const, T, Fun const>)
        {
            return {this};
        }
        CPP_member
        auto end_adaptor() const
            -> CPP_ret(meta::if_<use_sentinel_t, adaptor_base, adaptor<true>>)( //
                requires exclusive_scan_constraints<Rng const, T, Fun const>)
        {
            return {this};
        }

    public:
        exclusive_scan_view() = default;
        constexpr exclusive_scan_view(Rng rng, T init, Fun fun)
          : exclusive_scan_view::view_adaptor{std::move(rng)}
          , init_(std::move(init))
          , fun_(std::move(fun))
        {}
        CPP_member
        auto CPP_fun(size)()(const requires sized_range<Rng const>)
        {
            return ranges::size(this->base());
        }
        CPP_member
        auto CPP_fun(size)()(requires sized_range<Rng>)
        {
            return ranges::size(this->base());
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng, typename T,
                 typename Fun)(requires copy_constructible<T> && copy_constructible<Fun>)
        exclusive_scan_view(Rng &&, T, Fun)
            ->exclusive_scan_view<views::all_t<Rng>, T, Fun>;
#endif

    namespace views
    {
        struct exclusive_scan_fn
        {
        private:
            friend view_access;
            template<typename T, typename Fun = plus>
            static constexpr auto bind(exclusive_scan_fn exclusive_scan, T init,
                                       Fun fun = {})
            {
                return make_pipeable(
                    bind_back(exclusive_scan, std::move(init), std::move(fun)));
            }

        public:
            template<typename Rng, typename T, typename Fun = plus>
            constexpr auto operator()(Rng && rng, T init, Fun fun = Fun{}) const
                -> CPP_ret(exclusive_scan_view<all_t<Rng>, T, Fun>)( //
                    requires exclusive_scan_constraints<Rng, T, Fun>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(init), std::move(fun)};
            }
        };

        /// \relates exclusive_scan_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<exclusive_scan_fn>, exclusive_scan)
    } // namespace views
    /// @}
} // namespace ranges
#endif // RANGE_V3_VIEW_EXCLUSIVE_SCAN_HPP
