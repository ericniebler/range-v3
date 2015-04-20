/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_JOIN_HPP
#define RANGES_V3_VIEW_JOIN_HPP

#include <utility>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/numeric.hpp> // for accumulate
#include <range/v3/begin_end.hpp>
#include <range/v3/empty.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_adaptor.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/single.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{

        // Join a range of ranges
        template<typename Rng>
        struct join_view<Rng, void>
          : range_adaptor<join_view<Rng, void>, Rng,
                is_infinite<Rng>::value || is_infinite<range_value_t<Rng>>::value>
        {
        private:
            CONCEPT_ASSERT(Iterable<Rng>());
            CONCEPT_ASSERT(Iterable<range_value_t<Rng>>());
            using size_t_ = common_type_t<range_size_t<Rng>, range_size_t<range_value_t<Rng>>>;

            friend range_access;
            view::all_t<range_value_t<Rng>> cur_;

            struct adaptor : adaptor_base
            {
            private:
                join_view *rng_;
                range_iterator_t<range_value_t<Rng>> it_;
                RANGES_RELAXED_CONSTEXPR
                void satisfy(range_iterator_t<Rng> &it)
                {
                    auto &cur = rng_->cur_;
                    auto const end = ranges::end(rng_->mutable_base());
                    while(it_ == ranges::end(cur))
                    {
                        if(++it == end)
                        {
#if __cplusplus == 201103L
                            rng_ = nullptr;
#endif
                            it_ = detail::value_init{};
                            break;
                        }
                        cur = view::all(*it);
                        it_ = ranges::begin(cur);
                    }
                }
            public:
                using single_pass = std::true_type;
                RANGES_RELAXED_CONSTEXPR adaptor() = default;
                RANGES_RELAXED_CONSTEXPR
                adaptor(join_view &rng)
                  : rng_(&rng), it_{}
                {}
                RANGES_RELAXED_CONSTEXPR
                range_iterator_t<Rng> begin(join_view &)
                {
                    auto it = ranges::begin(rng_->mutable_base());
                    auto const end = ranges::end(rng_->mutable_base());
                    if(it != end)
                    {
                        rng_->cur_ = view::all(*it);
                        it_ = ranges::begin(rng_->cur_);
                        satisfy(it);
                    }
                    return it;
                }
                RANGES_RELAXED_CONSTEXPR
                bool equal(range_iterator_t<Rng> const &it, range_iterator_t<Rng> const &other_it,
                    adaptor const &other_adapt) const
                {
#ifdef RANGES_CXX_GREATER_THAN_11
                    RANGES_ASSERT(rng_ == other_adapt.rng_);
                    return it == other_it && it_ == other_adapt.it_;
#else
                    return (!rng_ && !other_adapt.rng_) ||
                        (it == other_it && it_ == other_adapt.it_);
#endif
                }
                RANGES_RELAXED_CONSTEXPR
                void next(range_iterator_t<Rng> &it)
                {
                    ++it_;
                    satisfy(it);
                }
                RANGES_RELAXED_CONSTEXPR
                auto current(range_iterator_t<Rng> const &) const ->
                    decltype(*it_)
                {
                    return *it_;
                }
                RANGES_RELAXED_CONSTEXPR
                auto indirect_move(range_iterator_t<Rng> const &) const ->
                    decltype(ranges::indirect_move(it_))
                {
                    return ranges::indirect_move(it_);
                }
            };
            RANGES_RELAXED_CONSTEXPR
            adaptor begin_adaptor()
            {
                return {*this};
            }
            RANGES_RELAXED_CONSTEXPR
            adaptor end_adaptor()
            {
#ifdef RANGES_CXX_GREATER_THAN_11
                return {*this};
#else
                return {};
#endif
            }
            // TODO: could support const iteration if range_reference_t<Rng> is a true reference.
        public:
            RANGES_RELAXED_CONSTEXPR join_view() = default;
            RANGES_RELAXED_CONSTEXPR explicit join_view(Rng rng)
              : range_adaptor_t<join_view>{std::move(rng)}, cur_{}
            {}
            CONCEPT_REQUIRES(!is_infinite<Rng>() && ForwardIterable<Rng>() &&
                             SizedIterable<range_value_t<Rng>>())
            RANGES_RELAXED_CONSTEXPR
            size_t_ size() const
            {
                return accumulate(view::transform(this->base(), ranges::size), size_t_{0});
            }
        };

        // Join a range of ranges, inserting a range of values between them.
        template<typename Rng, typename ValRng>
        struct join_view
          : range_adaptor<join_view<Rng, ValRng>, Rng,
                meta::or_<is_infinite<Rng>, is_infinite<range_value_t<Rng>>, is_infinite<ValRng>>::value>
        {
        private:
            CONCEPT_ASSERT(InputIterable<Rng>());
            CONCEPT_ASSERT(ForwardIterable<ValRng>());
            CONCEPT_ASSERT(InputIterable<range_value_t<Rng>>());
            CONCEPT_ASSERT(Common<range_value_t<range_value_t<Rng>>, range_value_t<ValRng>>());
            CONCEPT_ASSERT(SemiRegular<concepts::Common::value_t<
                range_value_t<range_value_t<Rng>>,
                range_value_t<ValRng>>>());
            using size_t_ = common_type_t<range_size_t<Rng>, range_size_t<range_value_t<Rng>>>;

            friend range_access;
            view::all_t<range_value_t<Rng>> cur_;
            ValRng val_;

            struct adaptor : adaptor_base
            {
            private:
                join_view *rng_;
                bool toggl_;
                range_iterator_t<ValRng> val_it_;
                range_iterator_t<range_value_t<Rng>> it_;
                RANGES_RELAXED_CONSTEXPR
                void satisfy(range_iterator_t<Rng> &it)
                {
                    auto &cur = rng_->cur_;
                    auto &val = rng_->val_;
                    auto const end = ranges::end(rng_->mutable_base());
                    while(toggl_ ? it_ == ranges::end(cur) : val_it_ == ranges::end(val))
                    {
                        if(toggl_)
                        {
                            if(++it == end)
                            {
#if __cplusplus == 201103L
                                rng_ = nullptr;
#endif
                                it_ = detail::value_init{};
                                break;
                            }
                            toggl_ = false;
                            val_it_ = ranges::begin(val);
                        }
                        else
                        {
                            toggl_ = true;
                            cur = view::all(*it);
                            it_ = ranges::begin(cur);
                        }
                    }
                }
            public:
                using single_pass = std::true_type;
                RANGES_RELAXED_CONSTEXPR
                adaptor() = default;
                RANGES_RELAXED_CONSTEXPR
                adaptor(join_view &rng)
                  : rng_(&rng), toggl_(true), val_it_{}, it_{}
                {}
                RANGES_RELAXED_CONSTEXPR
                range_iterator_t<Rng> begin(join_view &)
                {
                    auto it = ranges::begin(rng_->mutable_base());
                    auto const end = ranges::end(rng_->mutable_base());
                    if(it != end)
                    {
                        rng_->cur_ = view::all(*it);
                        it_ = ranges::begin(rng_->cur_);
                        satisfy(it);
                    }
                    return it;
                }
                RANGES_RELAXED_CONSTEXPR
                bool equal(range_iterator_t<Rng> const &it, range_iterator_t<Rng> const &other_it,
                    adaptor const &other_adapt) const
                {
#ifdef RANGES_CXX_GREATER_THAN_11
                    RANGES_ASSERT(rng_ == other_adapt.rng_);
                    return it == other_it && toggl_ == other_adapt.toggl_ &&
                        (toggl_ ? it_ == other_adapt.it_ : val_it_ == other_adapt.val_it_);
#else
                    return (!rng_ && !other_adapt.rng_) ||
                        (it == other_it && toggl_ == other_adapt.toggl_ &&
                            (toggl_ ? it_ == other_adapt.it_ : val_it_ == other_adapt.val_it_));
#endif
                }
                RANGES_RELAXED_CONSTEXPR
                void next(range_iterator_t<Rng> &it)
                {
                    toggl_ ? (void)++it_ : (void)++val_it_;
                    satisfy(it);
                }
                RANGES_RELAXED_CONSTEXPR
                auto current(range_iterator_t<Rng> const &) const ->
                    common_reference_t<
                        range_reference_t<range_value_t<Rng>>,
                        range_reference_t<ValRng>>
                {
                    if(toggl_)
                        return *it_;
                    return *val_it_;
                }
                RANGES_RELAXED_CONSTEXPR
                auto indirect_move(range_iterator_t<Rng> const &) const ->
                    common_reference_t<
                        range_rvalue_reference_t<range_value_t<Rng>>,
                        range_rvalue_reference_t<ValRng>>
                {
                    if(toggl_)
                        return ranges::indirect_move(it_);
                    return ranges::indirect_move(val_it_);
                }
            };
            RANGES_RELAXED_CONSTEXPR
            adaptor begin_adaptor()
            {
                return {*this};
            }
            RANGES_RELAXED_CONSTEXPR
            adaptor end_adaptor()
            {
#ifdef RANGES_CXX_GREATER_THAN_11
                return {*this};
#else
                return {};
#endif
            }
            // TODO: could support const iteration if range_reference_t<Rng> is a true reference.
        public:
            RANGES_RELAXED_CONSTEXPR join_view() = default;
            RANGES_RELAXED_CONSTEXPR join_view(Rng rng, ValRng val)
              : range_adaptor_t<join_view>{std::move(rng)}
              , cur_{}, val_(std::move(val))
            {}
            CONCEPT_REQUIRES(!is_infinite<Rng>() && ForwardIterable<Rng>() &&
                             SizedIterable<range_value_t<Rng>>() && SizedIterable<ValRng>())
            RANGES_RELAXED_CONSTEXPR
            size_t_ size() const
            {
                return accumulate(view::transform(this->mutable_base(), ranges::size), size_t_{0}) +
                    (ranges::empty(this->mutable_base()) ? 0 :
                        ranges::size(val_) * (ranges::size(this->mutable_base()) - 1));
            }
        };

        namespace view
        {
            struct join_fn
            {
                template<typename Rng>
                using JoinableIterable_ = meta::and_<
                    InputIterable<Rng>,
                    // Only evaluate this one if the previous one succeeded
                    meta::lazy::apply<
                        meta::compose<
                            meta::quote<InputIterable>,
                            meta::quote<range_value_t>>,
                        Rng>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(JoinableIterable_<Rng>())>
                RANGES_RELAXED_CONSTEXPR
                join_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return join_view<all_t<Rng>>{all(std::forward<Rng>(rng))};
                }
                template<typename Rng, typename Val = range_value_t<range_value_t<Rng>>,
                    CONCEPT_REQUIRES_(JoinableIterable_<Rng>())>
                RANGES_RELAXED_CONSTEXPR
                join_view<all_t<Rng>, single_view<Val>> operator()(Rng && rng, meta::id_t<Val> v) const
                {
                    CONCEPT_ASSERT_MSG(SemiRegular<Val>(),
                        "To join a range of ranges with a value, the value type must be a model of "
                        "the SemiRegular concept; that is, it must have a default constructor, "
                        "copy and move constructors, and a destructor.");
                    return {all(std::forward<Rng>(rng)), single(std::move(v))};
                }
                template<typename Rng, typename ValRng,
                    CONCEPT_REQUIRES_(JoinableIterable_<Rng>() && ForwardIterable<ValRng>())>
                RANGES_RELAXED_CONSTEXPR
                join_view<all_t<Rng>, all_t<ValRng>> operator()(Rng && rng, ValRng && val) const
                {
                    CONCEPT_ASSERT_MSG(Common<range_value_t<ValRng>,
                        range_value_t<range_value_t<Rng>>>(),
                        "To join a range of ranges with another range, all the ranges must have "
                        "a common value type.");
                    CONCEPT_ASSERT_MSG(SemiRegular<concepts::Common::value_t<
                        range_value_t<ValRng>, range_value_t<range_value_t<Rng>>>>(),
                        "To join a range of ranges with another range, all the ranges must have "
                        "a common value type, and that value type must model the SemiRegular "
                        "concept; that is, it must have a default constructor, copy and move "
                        "constructors, and a destructor.");
                    return {all(std::forward<Rng>(rng)), all(std::forward<ValRng>(val))};
                }
            private:
                template <class T> struct dump;
               friend view_access;
               template<typename T, CONCEPT_REQUIRES_(!JoinableIterable_<T>())>
               static RANGES_RELAXED_CONSTEXPR auto bind(join_fn join, T && t)
               RANGES_DECLTYPE_AUTO_RETURN
               (
                   make_pipeable(binder_1<join_fn, decltype(bind_forward<T>(t))>
                                 (join, bind_forward<T>(t)))
               )
            };

            /// \relates join_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& join = static_const<view<join_fn>>::value;
            }
        }
        /// @}
    }
}

#endif
