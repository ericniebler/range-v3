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
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/utility/variant.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/view/transform.hpp>
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
            template<typename Outer, typename Inner, typename Joiner>
            using join_cardinality_ =
                std::integral_constant<cardinality,
                    Outer::value == infinite || Inner::value == infinite || (Joiner::value == infinite && Outer::value != 0 && Outer::value != 1) ?
                        infinite :
                        Outer::value == unknown || Inner::value == unknown || (Joiner::value == unknown && Outer::value != 0 && Outer::value != 1) ?
                            unknown :
                            Outer::value == finite || Inner::value == finite || (Joiner::value == finite && Outer::value != 0 && Outer::value != 1) ?
                                finite :
                                static_cast<cardinality>(Outer::value * Inner::value + (Outer::value == 0 ? 0 : (Outer::value - 1) * Joiner::value))>;
            template<typename Range, typename JoinRange = void>
            using join_cardinality =
                join_cardinality_<range_cardinality<Range>, range_cardinality<range_reference_t<Range>>,
                    meta::if_<std::is_same<void, JoinRange>,
                        std::integral_constant<cardinality, static_cast<cardinality>(0)>,
                        range_cardinality<JoinRange>>>;
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{

        // Join a range of ranges
        template<typename Rng>
        struct join_view<Rng, void>
          : view_facade<join_view<Rng, void>, detail::join_cardinality<Rng>::value>
        {
            using size_type = common_type_t<range_size_t<Rng>, range_size_t<range_reference_t<Rng>>>;

            join_view() = default;
            explicit join_view(Rng rng)
              : outer_{view::all(std::move(rng))}
              // Delay initialization of the other members until begin() is first called,
              // so that we can create a join_view over e.g. an infinite range of empty ranges,
              // and avoid hanging if begin is never called.
            {}
            CONCEPT_REQUIRES(detail::join_cardinality<Rng>::value >= 0)
            constexpr size_type size() const
            {
                return detail::join_cardinality<Rng>::value;
            }
            CONCEPT_REQUIRES(detail::join_cardinality<Rng>::value < 0 &&
                range_cardinality<Rng>::value >= 0 && ForwardRange<Rng>() &&
                SizedRange<range_reference_t<Rng>>())
            size_type size() const
            {
                return accumulate(view::transform(outer_, ranges::size), size_type{0});
            }
        private:
            friend range_access;
            Rng outer_{};

            class cursor : box<range_sentinel_t<Rng>, Rng>
            {
            private:
                range_iterator_t<Rng> outer_it_{};
                view::all_t<range_reference_t<Rng>> inner_{};
                range_iterator_t<view::all_t<range_reference_t<Rng>>> inner_it_{};
                range_sentinel_t<Rng> & outer_end() { return ranges::get<Rng>(*this); }
                range_sentinel_t<Rng> const& outer_end() const { return ranges::get<Rng>(*this); }
                void satisfy()
                {
                    while (inner_it_ == ranges::end(inner_) && ++outer_it_ != outer_end())
                    {
                        inner_ = view::all(*outer_it_);
                        inner_it_ = ranges::begin(inner_);
                    }
                }
            public:
                using single_pass = std::true_type;
                cursor() = default;
                cursor(join_view &rng)
                  : cursor::box{ranges::end(rng.outer_)}
                  , outer_it_{ranges::begin(rng.outer_)}
                {
                    if (outer_it_ != outer_end())
                    {
                        inner_ = view::all(*outer_it_);
                        inner_it_ = ranges::begin(inner_);
                        satisfy();
                    }
                }
                bool done() const
                {
                    return outer_it_ == outer_end();
                }
                void next()
                {
                    RANGES_ASSERT(inner_it_ != ranges::end(inner_));
                    ++inner_it_;
                    satisfy();
                }
                auto get() const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    *inner_it_
                )
                auto indirect_move() const
                RANGES_DECLTYPE_AUTO_RETURN
                (
                    ranges::indirect_move(inner_it_)
                )
            };
            cursor begin_cursor()
            {
                return {*this};
            }
            // TODO: could support const iteration if range_reference_t<Rng> is a true reference.
        };

        // Join a range of ranges, inserting a range of values between them.
        template<typename Rng, typename ValRng>
        struct join_view
          : view_facade<join_view<Rng, ValRng>, detail::join_cardinality<Rng, ValRng>::value>
        {
            CONCEPT_ASSERT(InputRange<Rng>());
            CONCEPT_ASSERT(InputRange<range_reference_t<Rng>>());
            CONCEPT_ASSERT(ForwardRange<ValRng>());
            CONCEPT_ASSERT(Common<range_value_t<range_reference_t<Rng>>, range_value_t<ValRng>>());
            CONCEPT_ASSERT(SemiRegular<concepts::Common::value_t<
                range_value_t<range_reference_t<Rng>>,
                range_value_t<ValRng>>>());
            using size_type = common_type_t<range_size_t<Rng>, range_size_t<range_value_t<Rng>>>;

            join_view() = default;
            join_view(Rng rng, ValRng val)
              : outer_(std::move(rng))
              , val_(std::move(val))
            {}
            CONCEPT_REQUIRES(detail::join_cardinality<Rng, ValRng>::value >= 0)
            constexpr size_type size() const
            {
                return detail::join_cardinality<Rng, ValRng>::value;
            }
            CONCEPT_REQUIRES(detail::join_cardinality<Rng, ValRng>::value < 0 &&
                range_cardinality<Rng>::value >= 0 && ForwardRange<Rng>() &&
                SizedRange<range_reference_t<Rng>>() && SizedRange<ValRng>())
            size_type size() const
            {
                return accumulate(view::transform(outer_, ranges::size), size_type{0}) +
                        (range_cardinality<Rng>::value == 0 ?
                            0 :
                            ranges::size(val_) * (range_cardinality<Rng>::value - 1));;
            }
        private:
            friend range_access;

            Rng outer_{};
            ValRng val_{};

            class cursor : box<range_sentinel_t<Rng>, Rng>
            {
                using inner_range_t = view::all_t<range_reference_t<Rng>>;
                struct inner_state {
                    inner_range_t rng;
                    range_iterator_t<inner_range_t> it;
                };
                using state = variant<range_iterator_t<ValRng>, inner_state>;
                range_iterator_t<Rng> outer_it_{};
                state cur_{};
                join_view *rng_ = nullptr;

                range_sentinel_t<Rng> & outer_end() { return ranges::get<Rng>(*this); }
                range_sentinel_t<Rng> const& outer_end() const { return ranges::get<Rng>(*this); }

                void satisfy()
                {
                    RANGES_ASSERT(rng_);
                    while (true)
                    {
                        if (cur_.index() == 0)
                        {
                            if (ranges::get<0>(cur_) != ranges::end(rng_->val_))
                                break;
                            ranges::emplace<1>(cur_, inner_state{});
                            inner_state& inner = ranges::get<1>(cur_);
                            inner.rng = view::all(*outer_it_);
                            inner.it = ranges::begin(inner.rng);
                        }
                        else
                        {
                            inner_state& inner = ranges::get<1>(cur_);
                            if (inner.it != ranges::end(inner.rng))
                                break;
                            if (++outer_it_ == outer_end())
                                break;
                            ranges::emplace<0>(cur_, ranges::begin(rng_->val_));
                        }
                    }
                }
            public:
                using value_type = common_type_t<
                    range_value_t<inner_range_t>, range_value_t<ValRng>>;
                using reference = common_reference_t<
                    range_reference_t<inner_range_t>, range_reference_t<ValRng>>;
                using rvalue_reference = common_reference_t<
                    range_rvalue_reference_t<inner_range_t>, range_rvalue_reference_t<ValRng>>;
                using single_pass = std::true_type;
                cursor() = default;
                cursor(join_view &rng)
                  : cursor::box{ranges::end(rng.outer_)}
                  , outer_it_{ranges::begin(rng.outer_)}
                  , rng_{&rng}
                {
                    if (outer_it_ != outer_end())
                    {
                        ranges::emplace<1>(cur_, inner_state{});
                        inner_state& inner = ranges::get<1>(cur_);
                        inner.rng = view::all(*outer_it_);
                        inner.it = ranges::begin(inner.rng);
                        satisfy();
                    }
                }
                bool done() const
                {
                    RANGES_ASSERT(rng_);
                    return outer_it_ == outer_end();
                }
                void next()
                {
                    RANGES_ASSERT(rng_);
                    if (cur_.index() == 0)
                    {
                        ++ranges::get<0>(cur_);
                    }
                    else
                    {
                        ++ranges::get<1>(cur_).it;
                    }
                    satisfy();
                }
                reference get() const
                {
                    if (cur_.index() == 0)
                    {
                        return static_cast<reference>(*ranges::get<0>(cur_));
                    }
                    else
                    {
                        return static_cast<reference>(*ranges::get<1>(cur_).it);
                    }
                }
                rvalue_reference indirect_move() const
                {
                    if (cur_.index() == 0)
                    {
                        return static_cast<rvalue_reference>(ranges::indirect_move(ranges::get<0>(cur_)));
                    }
                    else
                    {
                        return static_cast<rvalue_reference>(ranges::indirect_move(ranges::get<1>(cur_).it));
                    }
                }
            };
            cursor begin_cursor()
            {
                return {*this};
            }
            // TODO: could support const iteration if range_reference_t<Rng> is a true reference.
        };

        namespace view
        {
            struct join_fn
            {
                template<typename Rng>
                using JoinableRange_ = meta::and_<
                    InputRange<Rng>,
                    // Only evaluate this one if the previous one succeeded
                    meta::lazy::invoke<
                        meta::compose<
                            meta::quote<InputRange>,
                            meta::quote<range_reference_t>>,
                        Rng>>;

                template<typename Rng,
                    CONCEPT_REQUIRES_(JoinableRange_<Rng>())>
                join_view<all_t<Rng>> operator()(Rng && rng) const
                {
                    return join_view<all_t<Rng>>{all(std::forward<Rng>(rng))};
                }
                template<typename Rng, typename Val = range_value_t<range_reference_t<Rng>>,
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
                        range_value_t<range_reference_t<Rng>>>(),
                        "To join a range of ranges with another range, all the ranges must have "
                        "a common value type.");
                    CONCEPT_ASSERT_MSG(SemiRegular<concepts::Common::value_t<
                        range_value_t<ValRng>, range_value_t<range_reference_t<Rng>>>>(),
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
