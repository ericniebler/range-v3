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

#ifndef RANGES_V3_VIEW_ADJACENT_REMOVE_IF_HPP
#define RANGES_V3_VIEW_ADJACENT_REMOVE_IF_HPP

#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct RANGES_EMPTY_BASES adjacent_remove_if_view
          : view_adaptor<
                adjacent_remove_if_view<Rng, Pred>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
          , private box<semiregular_t<Pred>, adjacent_remove_if_view<Rng, Pred>>
        {
            adjacent_remove_if_view() = default;
            constexpr adjacent_remove_if_view(Rng rng, Pred pred)
              : adjacent_remove_if_view::view_adaptor{detail::move(rng)}
              , adjacent_remove_if_view::box(detail::move(pred))
            {}
        private:
            friend range_access;

            struct adaptor
              : adaptor_base
            {
            private:
                adjacent_remove_if_view *rng_;
            public:
                adaptor() = default;
                constexpr adaptor(adjacent_remove_if_view &rng) noexcept
                  : rng_(&rng)
                {}
                RANGES_CXX14_CONSTEXPR static iterator_t<Rng> begin(adjacent_remove_if_view &rng)
                {
                    return *rng.begin_;
                }
                RANGES_CXX14_CONSTEXPR void next(iterator_t<Rng> &it) const
                {
                    RANGES_ASSERT(it != ranges::end(rng_->base()));
                    rng_->satisfy_forward(++it);
                }
                CONCEPT_REQUIRES(BidirectionalRange<Rng>())
                RANGES_CXX14_CONSTEXPR void prev(iterator_t<Rng> &it) const
                {
                    rng_->satisfy_reverse(it);
                }
                void advance() = delete;
                void distance_to() = delete;
            };
            RANGES_CXX14_CONSTEXPR adaptor begin_adaptor()
            {
                cache_begin();
                return {*this};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng>())
            RANGES_CXX14_CONSTEXPR adaptor end_adaptor()
            {
                if(BidirectionalRange<Rng>()) cache_begin();
                return {*this};
            }
            CONCEPT_REQUIRES(!BoundedRange<Rng>())
            RANGES_CXX14_CONSTEXPR adaptor_base end_adaptor() noexcept
            {
                return {};
            }

            RANGES_CXX14_CONSTEXPR void satisfy_forward(iterator_t<Rng> &it)
            {
                auto const end = ranges::end(this->base());
                if(it == end) return;
                auto &pred = this->adjacent_remove_if_view::box::get();
                for(auto next = it; ++next != end && invoke(pred, *it, *next); it = next)
                    ;
            }
            RANGES_CXX14_CONSTEXPR void satisfy_reverse(iterator_t<Rng> &it)
            {
                auto const &first = *begin_;
                RANGES_ASSERT(it != first); (void)first;
                auto prev = it;
                --it;
                if(prev == ranges::end(this->base()))
                {
                    return;
                }
                auto &pred = this->adjacent_remove_if_view::box::get();
                for(; invoke(pred, *it, *prev); prev = it, --it)
                    RANGES_ASSERT(it != first);
            }

            void cache_begin()
            {
                if(begin_) return;
                auto it = ranges::begin(this->base());
                satisfy_forward(it);
                begin_.emplace(std::move(it));
            }

            detail::non_propagating_cache<iterator_t<Rng>> begin_;
        };

        namespace view
        {
            struct adjacent_remove_if_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(adjacent_remove_if_fn adjacent_remove_if, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(std::bind(adjacent_remove_if, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred>
                using Constraint = meta::and_<
                    ForwardRange<Rng>,
                    IndirectPredicate<Pred, iterator_t<Rng>, iterator_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Constraint<Rng, Pred>())>
                RANGES_CXX14_CONSTEXPR auto operator()(Rng &&rng, Pred pred) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    adjacent_remove_if_view<all_t<Rng>, Pred>{
                        all(static_cast<Rng &&>(rng)), std::move(pred)}
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Constraint<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(ForwardRange<Rng>(),
                        "Rng must model the ForwardRange concept");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, iterator_t<Rng>,
                        iterator_t<Rng>>(),
                        "Pred must be callable with two arguments of the range's common "
                        "reference type, and it must return something convertible to bool.");
                }
            #endif
            };

            /// \relates adjacent_remove_if_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<adjacent_remove_if_fn>, adjacent_remove_if)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::adjacent_remove_if_view)

#endif
