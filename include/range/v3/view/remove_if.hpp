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
        struct remove_if_view
          : view_adaptor<
                remove_if_view<Rng, Pred>,
                Rng,
                is_finite<Rng>::value ? finite : range_cardinality<Rng>::value>
          , private box<semiregular_t<Pred>>
        {
            remove_if_view() = default;
            constexpr remove_if_view(Rng rng, Pred pred)
                noexcept(
                    std::is_nothrow_constructible<
                        typename remove_if_view::view_adaptor, Rng>::value &&
                    std::is_nothrow_constructible<
                        typename remove_if_view::box, Pred>::value)
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
                    noexcept(std::is_nothrow_copy_constructible<iterator_t<Rng>>::value)
                {
                    return *rng.begin_;
                }
                RANGES_CXX14_CONSTEXPR void next(iterator_t<Rng> &it) const
                    noexcept(noexcept(std::declval<remove_if_view &>().satisfy_forward(++it)))
                {
                    RANGES_ASSERT(it != ranges::end(rng_->base()));
                    rng_->satisfy_forward(++it);
                }
                CONCEPT_REQUIRES(BidirectionalRange<Rng>())
                RANGES_CXX14_CONSTEXPR void prev(iterator_t<Rng> &it) const
                    noexcept(noexcept(std::declval<remove_if_view &>().satisfy_reverse(it)))
                {
                    rng_->satisfy_reverse(it);
                }
                void advance() = delete;
                void distance_to() = delete;
            private:
                remove_if_view *rng_;
            };
            RANGES_CXX14_CONSTEXPR adaptor begin_adaptor()
                noexcept(noexcept(std::declval<remove_if_view &>().cache_begin()))
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
                noexcept(noexcept(std::declval<remove_if_view &>().cache_begin()))
            {
                if(BidirectionalRange<Rng>()) cache_begin();
                return {*this};
            }

            RANGES_CXX14_CONSTEXPR void satisfy_forward(iterator_t<Rng> &it)
                noexcept(noexcept((void)(++it != ranges::end(std::declval<Rng &>())),
                    invoke(std::declval<Pred &>(), *it)))
            {
                auto const last = ranges::end(this->base());
                auto &pred = this->remove_if_view::box::get();
                while (it != last && invoke(pred, *it))
                    ++it;
            }
            RANGES_CXX14_CONSTEXPR void satisfy_reverse(iterator_t<Rng> &it)
                noexcept(noexcept(invoke(std::declval<Pred &>(), *--it)))
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
                noexcept(noexcept(ranges::begin(std::declval<Rng &>()),
                    std::declval<remove_if_view &>().
                        satisfy_forward(std::declval<iterator_t<Rng> &>())) &&
                    std::is_nothrow_move_constructible<iterator_t<Rng>>::value)
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
            struct remove_if_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(remove_if_fn remove_if, Pred pred)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    make_pipeable(std::bind(remove_if, std::placeholders::_1,
                        protect(std::move(pred))))
                )
            public:
                template<typename Rng, typename Pred>
                using Constraint = meta::and_<
                    InputRange<Rng>,
                    IndirectPredicate<Pred, iterator_t<Rng>>>;

                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(Constraint<Rng, Pred>())>
                RANGES_CXX14_CONSTEXPR auto operator()(Rng &&rng, Pred pred) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    remove_if_view<all_t<Rng>, Pred>{
                        all(static_cast<Rng &&>(rng)), std::move(pred)}
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred,
                    CONCEPT_REQUIRES_(!Constraint<Rng, Pred>())>
                void operator()(Rng &&, Pred) const
                {
                    CONCEPT_ASSERT_MSG(InputRange<Rng>(),
                        "The first argument to view::remove_if must be a model of the "
                        "InputRange concept");
                    CONCEPT_ASSERT_MSG(IndirectPredicate<Pred, iterator_t<Rng>>(),
                        "The second argument to view::remove_if must be callable with "
                        "a value of the range, and the return type must be convertible "
                        "to bool");
                }
            #endif
            };

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
