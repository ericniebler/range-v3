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

#ifndef RANGES_V3_VIEW_PARTIAL_SUM_HPP
#define RANGES_V3_VIEW_PARTIAL_SUM_HPP

#include <utility>
#include <iterator>
#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Fun>
    struct partial_sum_view
      : view_adaptor<partial_sum_view<Rng, Fun>, Rng>
    {
    private:
        friend range_access;
        semiregular_t<Fun> fun_;
        template<bool Const>
        using single_pass =
            meta::bool_<SinglePass<iterator_t<meta::const_if_c<Const, Rng>>>>;
        template<bool Const>
        using use_sentinel_t =
            meta::bool_<!CommonRange<meta::const_if_c<Const, Rng>> || single_pass<Const>::value>;

        template<bool Const>
        struct adaptor : adaptor_base
        {
        private:
            friend struct adaptor<!Const>;
            using CRng = meta::const_if_c<Const, Rng>;
            using partial_sum_view_t = meta::const_if_c<Const, partial_sum_view>;
            semiregular_t<range_value_t<Rng>> sum_;
            partial_sum_view_t *rng_;
        public:
            using single_pass = partial_sum_view::single_pass<Const>;
            adaptor() = default;
            adaptor(partial_sum_view_t &rng)
              : rng_(&rng)
            {}
            template<bool Other>
            constexpr CPP_ctor(adaptor)(adaptor<Other> that)(
                requires Const && (!Other))
              : sum_(std::move(that.sum_))
              , rng_(that.rng_)
            {}
            iterator_t<CRng> begin(partial_sum_view_t &)
            {
                auto &base = rng_->base();
                auto it = ranges::begin(base);
                if (it != ranges::end(base))
                    sum_ = *it;
                return it;
            }
            range_value_t<Rng> read(iterator_t<CRng>) const
            {
                return sum_;
            }
            void next(iterator_t<CRng> &it)
            {
                if (++it != ranges::end(rng_->base()))
                {
                    auto &current = static_cast<range_value_t<Rng> &>(sum_);
                    sum_ = invoke(rng_->fun_, current, *it);
                }
            }
            void prev() = delete;
        };

        adaptor<false> begin_adaptor()
        {
            return {*this};
        }
        meta::if_<use_sentinel_t<false>, adaptor_base, adaptor<false>> end_adaptor()
        {
            return {*this};
        }
        template<bool Const = true>
        auto begin_adaptor() const ->
            CPP_ret(adaptor<Const>)(
                requires Const && Range<meta::const_if_c<Const, Rng>> &&
                    IndirectBinaryInvocable_<
                        Fun const &,
                        iterator_t<meta::const_if_c<Const, Rng>>,
                        iterator_t<meta::const_if_c<Const, Rng>>>)
        {
            return {*this};
        }
        template<bool Const = true>
        auto end_adaptor() const ->
            CPP_ret(meta::if_<use_sentinel_t<Const>, adaptor_base, adaptor<Const>>)(
                requires Const && Range<meta::const_if_c<Const, Rng>> &&
                    IndirectBinaryInvocable_<
                        Fun const &,
                        iterator_t<meta::const_if_c<Const, Rng>>,
                        iterator_t<meta::const_if_c<Const, Rng>>>)
        {
            return {*this};
        }
    public:
        partial_sum_view() = default;
        partial_sum_view(Rng rng, Fun fun)
          : partial_sum_view::view_adaptor{std::move(rng)}
          , fun_(std::move(fun))
        {}
        CPP_member
        auto CPP_fun(size)() (const
            requires SizedRange<Rng>)
        {
            return ranges::size(this->base());
        }
    };

    namespace view
    {
        struct partial_sum_fn
        {
        private:
            friend view_access;
            template<typename Fun>
            static auto bind(partial_sum_fn partial_sum, Fun fun)
            {
                return make_pipeable(std::bind(partial_sum, std::placeholders::_1,
                    protect(std::move(fun))));
            }
            template<typename Fun = plus>
            RANGES_DEPRECATED("Use \"ranges::view::partial_sum\" instead of \"ranges::view::partial_sum()\".")
            static auto bind(partial_sum_fn partial_sum)
            {
                return make_pipeable(std::bind(partial_sum, std::placeholders::_1,
                    Fun{}));
            }
        public:
            template<typename Rng, typename Fun = plus>
            auto operator()(Rng &&rng, Fun fun = {}) const ->
                CPP_ret(partial_sum_view<all_t<Rng>, Fun>)(
                    requires InputRange<Rng> &&
                        IndirectBinaryInvocable_<Fun, iterator_t<Rng>, iterator_t<Rng>> &&
                        ConvertibleTo<
                            indirect_result_t<Fun &, iterator_t<Rng>, iterator_t<Rng>>,
                            range_value_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(fun)};
            }
        };

        /// \relates partial_sum_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<partial_sum_fn>, partial_sum)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::partial_sum_view)

#endif
