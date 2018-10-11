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
                static constexpr /*c++14*/ iterator_t<Rng> begin(remove_if_view &rng)
                    noexcept(std::is_nothrow_copy_constructible<iterator_t<Rng>>::value)
                {
                    return *rng.begin_;
                }
                constexpr /*c++14*/ void next(iterator_t<Rng> &it) const
                    noexcept(noexcept(std::declval<remove_if_view &>().satisfy_forward(++it)))
                {
                    RANGES_ASSERT(it != ranges::end(rng_->base()));
                    rng_->satisfy_forward(++it);
                }
                CPP_member
                constexpr /*c++14*/ auto prev(iterator_t<Rng> &it) const
                    noexcept(noexcept(std::declval<remove_if_view &>().satisfy_reverse(it))) ->
                    CPP_ret(void)(
                        requires BidirectionalRange<Rng>)
                {
                    rng_->satisfy_reverse(it);
                }
                void advance() = delete;
                void distance_to() = delete;
            private:
                remove_if_view *rng_;
            };
            constexpr /*c++14*/ adaptor begin_adaptor()
                noexcept(noexcept(std::declval<remove_if_view &>().cache_begin()))
            {
                cache_begin();
                return {*this};
            }
            CPP_member
            constexpr auto end_adaptor() const noexcept ->
                CPP_ret(adaptor_base)(
                    requires not BoundedRange<Rng>)
            {
                return {};
            }
            CPP_member
            constexpr /*c++14*/ auto end_adaptor()
                noexcept(noexcept(std::declval<remove_if_view &>().cache_begin())) ->
                CPP_ret(adaptor)(
                    requires BoundedRange<Rng>)
            {
                if(BidirectionalRange<Rng>) cache_begin();
                return {*this};
            }

            constexpr /*c++14*/ void satisfy_forward(iterator_t<Rng> &it)
                noexcept(noexcept((void)(++it != ranges::end(std::declval<Rng &>())),
                    invoke(std::declval<Pred &>(), *it)))
            {
                auto const last = ranges::end(this->base());
                auto &pred = this->remove_if_view::box::get();
                while (it != last && invoke(pred, *it))
                    ++it;
            }
            constexpr /*c++14*/ void satisfy_reverse(iterator_t<Rng> &it)
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

            constexpr /*c++14*/ void cache_begin()
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
            CPP_def
            (
                template(typename Rng, typename Pred)
                concept SearchableRange,
                    InputRange<Rng> &&
                    IndirectPredicate<Pred, iterator_t<Rng>>
            );

            struct remove_if_fn
            {
            private:
                friend view_access;
                template<typename Pred>
                static auto bind(remove_if_fn remove_if, Pred pred)
                {
                    return make_pipeable(std::bind(remove_if, std::placeholders::_1,
                        protect(std::move(pred))));
                }
            public:
                CPP_template(typename Rng, typename Pred)(
                    requires SearchableRange<Rng, Pred>)
                constexpr /*c++14*/ auto CPP_auto_fun(operator())(Rng &&rng, Pred pred) (const)
                (
                    return remove_if_view<all_t<Rng>, Pred>{
                        all(static_cast<Rng &&>(rng)), std::move(pred)}
                )
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Pred>
                auto operator()(Rng &&, Pred) const ->
                    CPP_ret(void)(
                        requires not SearchableRange<Rng, Pred>)
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The first argument to view::remove_if must be a model of the "
                        "InputRange concept");
                    CPP_assert_msg(IndirectPredicate<Pred, iterator_t<Rng>>,
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
