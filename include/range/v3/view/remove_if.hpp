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

#ifndef RANGES_V3_VIEW_REMOVE_IF_HPP
#define RANGES_V3_VIEW_REMOVE_IF_HPP

#include <type_traits>
#include <utility>
#include <meta/meta.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/utility/box.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>

RANGES_DISABLE_WARNINGS

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Pred>
        struct RANGES_EMPTY_BASES remove_if_view
          : view_adaptor<
                remove_if_view<Rng, Pred>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
          , private box<semiregular_t<Pred>>
        {
            remove_if_view() = default;
            constexpr remove_if_view(Rng rng, Pred pred)
              : remove_if_view::view_adaptor{detail::move(rng)}
              , remove_if_view::box(detail::move(pred))
            {}
        private:
            friend range_access;

            struct adaptor : adaptor_base
            {
                adaptor() = default;
                constexpr adaptor(remove_if_view &rng) noexcept
                  : rng_(&rng)
                {}
                static RANGES_CXX14_CONSTEXPR iterator_t<Rng> begin(remove_if_view &rng)
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
            private:
                remove_if_view *rng_;
            };
            RANGES_CXX14_CONSTEXPR adaptor begin_adaptor()
            {
                cache_begin();
                return {*this};
            }
            CONCEPT_REQUIRES(!BoundedRange<Rng>())
            constexpr adaptor_base end_adaptor() const noexcept
            {
                return {};
            }
            CONCEPT_REQUIRES(BoundedRange<Rng>())
            RANGES_CXX14_CONSTEXPR adaptor end_adaptor()
            {
                if(BidirectionalRange<Rng>()) cache_begin();
                return {*this};
            }

            RANGES_CXX14_CONSTEXPR void satisfy_forward(iterator_t<Rng> &it)
            {
                auto const last = ranges::end(this->base());
                auto &pred = this->remove_if_view::box::get();
                while (it != last && invoke(pred, *it))
                    ++it;
            }
            RANGES_CXX14_CONSTEXPR void satisfy_reverse(iterator_t<Rng> &it)
            {
                RANGES_ASSERT(begin_);
                auto const &first = *begin_;
                auto &pred = this->remove_if_view::box::get();
                do
                {
                    RANGES_ASSERT(it != first); (void)first;
                    --it;
                } while(invoke(pred, *it));
            }

            RANGES_CXX14_CONSTEXPR void cache_begin()
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
            /// \cond
            template<typename Modifier>
            struct remove_if_fn_
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(remove_if_fn_ remove_if, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(std::bind(remove_if, std::placeholders::_1,
                        protect(std::move(pred))))
                )

                template<typename Pred, typename Proj>
                static auto bind(remove_if_fn_ remove_if, Pred pred, Proj proj)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(std::bind(remove_if, std::placeholders::_1,
                        protect(std::move(pred)), protect(std::move(proj))))
                )
            public:
                template<typename Rng, typename Pred, typename Proj = ident>
                using Constraint = meta::and_<
                    InputRange<Rng>,
                    IndirectPredicate<Pred, projected<iterator_t<Rng>, Proj>>>;

                template<typename Rng, typename Pred,
                    typename M = detail::decay_t<invoke_result_t<Modifier, Pred>>,
                    CONCEPT_REQUIRES_(Constraint<Rng, Pred>())>
                RANGES_CXX14_CONSTEXPR
                auto operator()(Rng &&rng, Pred&& pred) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    remove_if_view<all_t<Rng>, M>{
                        all(static_cast<Rng &&>(rng)),
                        Modifier{}(std::move(pred))
                    }
                )

                template<typename Rng, typename Pred, typename Proj,
                    typename M = detail::decay_t<invoke_result_t<Modifier, Pred>>,
                    CONCEPT_REQUIRES_(Constraint<Rng, Pred, Proj>())>
                RANGES_CXX14_CONSTEXPR
                auto operator()(Rng &&rng, Pred pred, Proj proj) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    remove_if_view<all_t<Rng>, composed<M, Proj>>{
                        all(static_cast<Rng &&>(rng)),
                        compose(Modifier{}(std::move(pred)), std::move(proj))
                    }
                )

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Constraint<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::remove_if/filter must "
                        "be a model of the InputRange concept");
                    using Itr = iterator_t<Rng>;
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, Itr>(),
                        "The second argument to view::remove_if/filter must "
                        "accept arguments of the range's value type.");
                }

                template<typename Rng, typename Pred, typename Proj,
                    CONCEPT_REQUIRES_(!Constraint<Rng, Pred, Proj>())>
                void operator()(Rng &&, Pred, Proj) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::remove_if/filter must "
                        "be a model of the InputRange concept");
                    using Itr = iterator_t<Rng>;
                    CONCEPT_ASSERT_MSG(IndirectInvocable<Proj, Itr>(),
                        "The projection function must accept arguments of the iterator's "
                        "value type, reference type, and common reference type.");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, projected<Itr, Proj>>(),
                        "The second argument to view::remove_if/filter must accept values "
                        "returned by the projection function.");
                }
            #endif
            };
            /// \endcond

            /// Given a source range, unary predicate, and optional projection,
            /// present a view of the elements that do not satisfy the predicate.
            using remove_if_fn = remove_if_fn_<ident>;

            /// \relates remove_if_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<remove_if_fn>, remove_if)
        }
        /// @}
    }
}

RANGES_RE_ENABLE_WARNINGS

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::remove_if_view)

#endif
