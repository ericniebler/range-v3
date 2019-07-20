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

#ifndef RANGES_V3_VIEW_DELIMIT_HPP
#define RANGES_V3_VIEW_DELIMIT_HPP

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/detail/bind_back.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/unreachable_sentinel.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    template<typename Rng, typename Val>
    struct delimit_view
      : view_adaptor<delimit_view<Rng, Val>, Rng,
                     is_finite<Rng>::value ? finite : unknown>
    {
    private:
        friend range_access;
        Val value_;

        struct sentinel_adaptor : adaptor_base
        {
            sentinel_adaptor() = default;
            sentinel_adaptor(Val value)
              : value_(std::move(value))
            {}
            template<class I, class S>
            bool empty(I const & it, S const & end) const
            {
                return it == end || *it == value_;
            }
            Val value_;
        };

        sentinel_adaptor end_adaptor() const
        {
            return {value_};
        }

    public:
        delimit_view() = default;
        constexpr delimit_view(Rng rng, Val value)
          : delimit_view::view_adaptor{std::move(rng)}
          , value_(std::move(value))
        {}
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    CPP_template(typename Rng, typename Val)(requires CopyConstructible<Val>)
        delimit_view(Rng &&, Val)
            ->delimit_view<view::all_t<Rng>, Val>;
#endif

    namespace view
    {
        struct delimit_impl_fn
        {
        private:
            friend view_access;
            template<typename Val>
            static constexpr auto bind(delimit_impl_fn delimit, Val value)
            {
                return make_pipeable(bind_back<1>(delimit, std::move(value)));
            }

        public:
            template<typename Rng, typename Val>
            constexpr auto operator()(Rng && rng, Val value) const
                -> CPP_ret(delimit_view<all_t<Rng>, Val>)( //
                    requires ViewableRange<Rng> && InputRange<Rng> && Semiregular<Val> &&
                        EqualityComparableWith<Val, range_reference_t<Rng>>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(value)};
            }
        };

        struct delimit_fn : view<delimit_impl_fn>
        {
            using view<delimit_impl_fn>::operator();

            template<typename I_, typename Val, typename I = detail::decay_t<I_>>
            constexpr auto operator()(I_ && begin_, Val value) const
                -> CPP_ret(delimit_view<subrange<I, unreachable_sentinel_t>, Val>)( //
                    requires(!Range<I_> && ConvertibleTo<I_, I> && InputIterator<I> &&
                             Semiregular<Val> &&
                             EqualityComparableWith<Val, iter_reference_t<I>>))
            {
                return {{static_cast<I_ &&>(begin_), {}}, std::move(value)};
            }
        };

        /// \relates delimit_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(delimit_fn, delimit)
    } // namespace view
    /// @}
} // namespace ranges

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::delimit_view)

#endif
