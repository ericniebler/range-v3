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

#ifndef RANGES_V3_VIEW_ADJACENT_FILTER_HPP
#define RANGES_V3_VIEW_ADJACENT_FILTER_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/algorithm/adjacent_find.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename Rng, typename Pred>
            using AdjacentFilterConstraint2 =
                IndirectPredicate<Pred, iterator_t<Rng>, iterator_t<Rng>>;

            template<typename Rng, typename Pred>
            using AdjacentFilterConstraint = meta::and_<
                ForwardRange<Rng>,
                meta::defer<AdjacentFilterConstraint2, Rng, Pred>>;
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct adjacent_filter_view
          : view_adaptor<
                adjacent_filter_view<Rng, Pred>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
          , private box<semiregular_t<Pred>, adjacent_filter_view<Rng, Pred>>
        {
        private:
            friend range_access;

            static constexpr bool const_iterable =
                detail::AdjacentFilterConstraint<Rng const, Pred const>();

            struct adaptor : adaptor_base
            {
            private:
                using Base = meta::const_if_c<const_iterable, Rng>;
                using Parent = meta::const_if_c<const_iterable, adjacent_filter_view>;
                Parent *rng_;
            public:
                adaptor() = default;
                constexpr adaptor(Parent &rng) noexcept
                  : rng_(&rng)
                {}
                RANGES_CXX14_CONSTEXPR void next(iterator_t<Base> &it) const
                {
                    auto const last = ranges::end(rng_->base());
                    auto &pred = rng_->adjacent_filter_view::box::get();
                    RANGES_EXPECT(it != last);
                    for(auto prev = it; ++it != last; prev = it)
                        if(invoke(pred, *prev, *it))
                            break;
                }
                CONCEPT_REQUIRES(BidirectionalRange<Base>())
                RANGES_CXX14_CONSTEXPR void prev(iterator_t<Base> &it) const
                {
                    auto const first = ranges::begin(rng_->base());
                    auto &pred = rng_->adjacent_filter_view::box::get();
                    RANGES_EXPECT(it != first);
                    --it;
                    while(it != first)
                    {
                        auto prev = it;
                        if(invoke(pred, *--prev, *it))
                            break;
                        it = prev;
                    }
                }
                void distance_to() = delete;
            };
            CONCEPT_REQUIRES(const_iterable)
            constexpr adaptor begin_adaptor() const noexcept
            {
                return {*this};
            }
            CONCEPT_REQUIRES(const_iterable)
            constexpr adaptor end_adaptor() const noexcept
            {
                return {*this};
            }
            CONCEPT_REQUIRES(!const_iterable)
            RANGES_CXX14_CONSTEXPR adaptor begin_adaptor() noexcept
            {
                return {*this};
            }
            CONCEPT_REQUIRES(!const_iterable)
            RANGES_CXX14_CONSTEXPR adaptor end_adaptor() noexcept
            {
                return {*this};
            }
        public:
            adjacent_filter_view() = default;
            constexpr adjacent_filter_view(Rng rng, Pred pred)
                noexcept(std::is_nothrow_constructible<
                    typename adjacent_filter_view::view_adaptor, Rng>::value &&
                    std::is_nothrow_constructible<semiregular_t<Pred>, Pred>::value)
              : adjacent_filter_view::view_adaptor{detail::move(rng)}
              , adjacent_filter_view::box(detail::move(pred))
            {}
       };

        namespace view
        {
            struct adjacent_filter_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                RANGES_CXX14_CONSTEXPR
                static auto bind(adjacent_filter_fn adjacent_filter, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(std::bind(adjacent_filter, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(detail::AdjacentFilterConstraint<Rng, Pred>())>
                RANGES_CXX14_CONSTEXPR auto operator()(Rng && rng, Pred pred) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    adjacent_filter_view<all_t<Rng>, Pred>{
                        all(static_cast<Rng &&>(rng)), std::move(pred)}
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!detail::AdjacentFilterConstraint<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "Rng must model the ForwardRange concept");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, iterator_t<Rng>, iterator_t<Rng>>(),
                        "Pred must be callable with two arguments of the range's common "
                        "reference type, and it must return something convertible to bool.");
                }
            #endif
            };

            /// \relates adjacent_filter_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<adjacent_filter_fn>, adjacent_filter)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::adjacent_filter_view)

#endif
