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
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
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
            using single_pass = meta::bool_<SinglePass<iterator_t<Rng>>>;
            using use_sentinel_t = meta::bool_<!CommonRange<Rng> || single_pass{}>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                friend struct adaptor<!IsConst>;
                using CRng = meta::const_if_c<IsConst, Rng>;
                using partial_sum_view_t = meta::const_if_c<IsConst, partial_sum_view>;
                semiregular_t<range_value_t<Rng>> sum_;
                partial_sum_view_t *rng_;
            public:
                using single_pass = partial_sum_view::single_pass;
                adaptor() = default;
                adaptor(partial_sum_view_t &rng)
                  : rng_(&rng)
                {}
                template<bool Other>
                constexpr CPP_ctor(adaptor)(adaptor<Other> that)(
                    requires IsConst && (!Other))
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
            meta::if_<use_sentinel_t, adaptor_base, adaptor<false>> end_adaptor()
            {
                return {*this};
            }
            template<typename CRng = Rng const>
            auto begin_adaptor() const -> CPP_ret(adaptor<true>)(
                requires Range<CRng> &&
                    IndirectInvocable<Fun const &, iterator_t<CRng>, iterator_t<CRng>>)
            {
                return {*this};
            }
            template<typename CRng = Rng const>
            auto end_adaptor() const ->
                CPP_ret(meta::if_<use_sentinel_t, adaptor_base, adaptor<true>>)(
                    requires Range<CRng> &&
                        IndirectInvocable<Fun const &, iterator_t<CRng>, iterator_t<CRng>>)
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
                            IndirectInvocable<Fun, iterator_t<Rng>, iterator_t<Rng>> &&
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
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::partial_sum_view)

#endif
