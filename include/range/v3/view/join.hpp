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
#include <range/v3/view_adaptor.hpp>
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
        /// \cond
        namespace detail
        {
            // Compute the cardinality of a joined range
            template<typename Outer, typename Inner, typename Joiner = std::integral_constant<cardinality, static_cast<cardinality>(0)>>
            using join_cardinality =
                std::integral_constant<cardinality,
                    Outer::value == infinite || Inner::value == infinite || (Joiner::value == infinite && Outer::value != 0 && Outer::value != 1) ?
                        infinite :
                        Outer::value == unknown || Inner::value == unknown || (Joiner::value == unknown && Outer::value != 0 && Outer::value != 1) ?
                            unknown :
                            Outer::value == finite || Inner::value == finite || (Joiner::value == finite && Outer::value != 0 && Outer::value != 1) ?
                                finite :
                                static_cast<cardinality>(Outer::value * Inner::value + (Outer::value == 0 ? 0 : (Outer::value - 1) * Joiner::value))>;
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{

        // Join a range of ranges
        template<typename Rng>
        struct join_view<Rng, void>
          : view_adaptor<join_view<Rng, void>, Rng,
                detail::join_cardinality<
                    range_cardinality<Rng>,
                    range_cardinality<range_value_t<Rng>>>::value>
        {
        private:
            CONCEPT_ASSERT(Range<Rng>());
            CONCEPT_ASSERT(Range<range_value_t<Rng>>());
            using size_t_ = common_type_t<range_size_t<Rng>, range_size_t<range_value_t<Rng>>>;

            friend range_access;
            view::all_t<range_value_t<Rng>> cur_;

            struct adaptor : adaptor_base
            {
            private:
                join_view *rng_;
                range_iterator_t<range_value_t<Rng>> it_;
                void satisfy(range_iterator_t<Rng> &it)
                {
                    auto &cur = rng_->cur_;
                    auto const end = ranges::end(rng_->mutable_base());
                    while(it_ == ranges::end(cur))
                    {
                        if(++it == end)
                        {
#ifndef RANGES_CXX_GREATER_THAN_11
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
                adaptor() = default;
                adaptor(join_view &rng)
                  : rng_(&rng), it_{}
                {}
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
                void next(range_iterator_t<Rng> &it)
                {
                    ++it_;
                    satisfy(it);
                }
                auto get(range_iterator_t<Rng> const &) const ->
                    decltype(*it_)
                {
                    return *it_;
                }
                auto indirect_move(range_iterator_t<Rng> const &) const ->
                    decltype(ranges::indirect_move(it_))
                {
                    return ranges::indirect_move(it_);
                }
                void distance_to() = delete;
            };
            adaptor begin_adaptor()
            {
                return {*this};
            }
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
            join_view() = default;
            explicit join_view(Rng rng)
              : view_adaptor_t<join_view>{std::move(rng)}, cur_{}
            {}
            CONCEPT_REQUIRES(range_cardinality<Rng>::value >= 0 && SizedRange<range_value_t<Rng>>())
            constexpr size_t_ size() const
            {
                return range_cardinality<join_view>::value >= 0 ?
                    (size_t_)range_cardinality<join_view>::value :
                    accumulate(view::transform(this->base(), ranges::size), size_t_{0});
            }
        };

        // Join a range of ranges, inserting a range of values between them.
        template<typename Rng, typename ValRng>
        struct join_view
          : view_adaptor<join_view<Rng, ValRng>, Rng,
                detail::join_cardinality<
                    range_cardinality<Rng>,
                    range_cardinality<range_value_t<Rng>>,
                    range_cardinality<ValRng>>::value>
        {
        private:
            CONCEPT_ASSERT(InputRange<Rng>());
            CONCEPT_ASSERT(ForwardRange<ValRng>());
            CONCEPT_ASSERT(InputRange<range_value_t<Rng>>());
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
#ifndef RANGES_CXX_GREATER_THAN_11
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
                adaptor() = default;
                adaptor(join_view &rng)
                  : rng_(&rng), toggl_(true), val_it_{}, it_{}
                {}
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
                void next(range_iterator_t<Rng> &it)
                {
                    toggl_ ? (void)++it_ : (void)++val_it_;
                    satisfy(it);
                }
                auto get(range_iterator_t<Rng> const &) const ->
                    common_reference_t<
                        range_reference_t<range_value_t<Rng>>,
                        range_reference_t<ValRng>>
                {
                    if(toggl_)
                        return *it_;
                    return *val_it_;
                }
                auto indirect_move(range_iterator_t<Rng> const &) const ->
                    common_reference_t<
                        range_rvalue_reference_t<range_value_t<Rng>>,
                        range_rvalue_reference_t<ValRng>>
                {
                    if(toggl_)
                        return ranges::indirect_move(it_);
                    return ranges::indirect_move(val_it_);
                }
                void distance_to() = delete;
            };
            adaptor begin_adaptor()
            {
                return {*this};
            }
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
            join_view() = default;
            join_view(Rng rng, ValRng val)
              : view_adaptor_t<join_view>{std::move(rng)}
              , cur_{}, val_(std::move(val))
            {}
            CONCEPT_REQUIRES(range_cardinality<Rng>::value >= 0 &&
                SizedRange<range_value_t<Rng>>() && SizedRange<ValRng>())
            constexpr size_t_ size() const
            {
                return range_cardinality<join_view>::value >= 0 ?
                    (size_t_)range_cardinality<join_view>::value :
                    accumulate(view::transform(this->mutable_base(), ranges::size), size_t_{0}) +
                        (range_cardinality<Rng>::value == 0 ?
                            0 :
                            ranges::size(val_) * (range_cardinality<Rng>::value - 1));;
            }
        };

        namespace view
        {
            struct join_fn
            {
                template<typename Rng>
                using JoinableRange_ = meta::and_<
                    InputRange<Rng>,
                    // Only evaluate this one if the previous one succeeded
                    meta::lazy::apply<
                        meta::compose<
                            meta::quote<InputRange>,
                            meta::quote<range_value_t>>,
                        Rng>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(JoinableRange_<Rng>())>
                join_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return join_view<all_t<Rng>>{all(std::forward<Rng>(rng))};
                }
                template<typename Rng, typename Val = range_value_t<range_value_t<Rng>>,
                    CONCEPT_REQUIRES_(JoinableRange_<Rng>())>
                join_view<all_t<Rng>, single_view<Val>> operator()(Rng && rng, meta::id_t<Val> v) const
                {
                    CONCEPT_ASSERT_MSG(SemiRegular<Val>(),
                        "To join a range of ranges with a value, the value type must be a model of "
                        "the SemiRegular concept; that is, it must have a default constructor, "
                        "copy and move constructors, and a destructor.");
                    return {all(std::forward<Rng>(rng)), single(std::move(v))};
                }
                template<typename Rng, typename ValRng,
                    CONCEPT_REQUIRES_(JoinableRange_<Rng>() && ForwardRange<ValRng>())>
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
               friend view_access;
               template<typename T, CONCEPT_REQUIRES_(!JoinableRange_<T>())>
               static auto bind(join_fn join, T && t)
               RANGES_DECLTYPE_AUTO_RETURN
               (
                   make_pipeable(std::bind(join, std::placeholders::_1, bind_forward<T>(t)))
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
