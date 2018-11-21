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

#ifndef RANGES_V3_VIEW_TRANSFORM_HPP
#define RANGES_V3_VIEW_TRANSFORM_HPP

#include <utility>
#include <iterator>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/detail/satisfy_boost_range.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/size.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            constexpr cardinality transform2_cardinality(cardinality c1, cardinality c2)
            {
                return c1 >= 0 || c2 >= 0 ?
                    (c1 >= 0 && c2 >= 0 ? (c1 < c2 ? c1 : c2) : finite) :
                    c1 == finite || c2 == finite ?
                        finite :
                        c1 == unknown || c2 == unknown ?
                            unknown :
                            infinite;
            }
        }
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Rng, typename Fun>
        struct iter_transform_view
          : view_adaptor<iter_transform_view<Rng, Fun>, Rng>
        {
        private:
            friend range_access;
            semiregular_t<Fun> fun_;
            using use_sentinel_t =
                meta::bool_<!BoundedRange<Rng> || SinglePass<iterator_t<Rng>>>;

            template<bool IsConst>
            struct adaptor : adaptor_base
            {
            private:
                friend struct adaptor<!IsConst>;
                using CRng = meta::const_if_c<IsConst, Rng>;
                using fun_ref_ = semiregular_ref_or_val_t<Fun, IsConst>;
                fun_ref_ fun_;
            public:
                using value_type =
                    detail::decay_t<invoke_result_t<Fun&, copy_tag, iterator_t<CRng>>>;
                adaptor() = default;
                adaptor(fun_ref_ fun)
                  : fun_(std::move(fun))
                {}
                template<bool Other>
                CPP_ctor(adaptor)(adaptor<Other> that)(
                    requires IsConst && !Other)
                  : fun_(std::move(that.fun_))
                {}

                auto CPP_auto_fun(read)(iterator_t<CRng> it) (const)
                (
                    return invoke(fun_, it)
                )
                auto CPP_auto_fun(iter_move)(iterator_t<CRng> it) (const)
                (
                    return invoke(fun_, move_tag{}, it)
                )
            };

            adaptor<false> begin_adaptor()
            {
                return {fun_};
            }
            template<typename CRng = Rng const>
            auto begin_adaptor() const -> CPP_ret(adaptor<true>)(
                requires Range<CRng>() && Invocable<Fun const&, iterator_t<CRng>>)
            {
                return {fun_};
            }
            meta::if_<use_sentinel_t, adaptor_base, adaptor<false>> end_adaptor()
            {
                return {fun_};
            }
            template<typename CRng = Rng const>
            auto end_adaptor() const ->
                CPP_ret(meta::if_<use_sentinel_t, adaptor_base, adaptor<true>>)(
                    requires Range<CRng>() && Invocable<Fun const&, iterator_t<CRng>>)
            {
                return {fun_};
            }
        public:
            iter_transform_view() = default;
            iter_transform_view(Rng rng, Fun fun)
              : iter_transform_view::view_adaptor{std::move(rng)}
              , fun_(std::move(fun))
            {}
            CPP_member
            constexpr /*c++14*/ auto CPP_fun(size)() (
                requires SizedRange<Rng>)
            {
                return ranges::size(this->base());
            }
            CPP_member
            constexpr auto CPP_fun(size)() (const
                requires SizedRange<Rng const>)
            {
                return ranges::size(this->base());
            }
        };

        template<typename Rng, typename Fun>
        struct transform_view
          : iter_transform_view<Rng, indirected<Fun>>
        {
            transform_view() = default;
            transform_view(Rng rng, Fun fun)
              : iter_transform_view<Rng, indirected<Fun>>{std::move(rng),
                    indirect(std::move(fun))}
            {}
        };

        template<typename Rng1, typename Rng2, typename Fun>
        struct iter_transform2_view
          : view_facade<
                iter_transform2_view<Rng1, Rng2, Fun>,
                detail::transform2_cardinality(
                    range_cardinality<Rng1>::value,
                    range_cardinality<Rng2>::value)>
        {
        private:
            friend range_access;
            semiregular_t<Fun> fun_;
            Rng1 rng1_;
            Rng2 rng2_;
            using difference_type_ = common_type_t<range_difference_t<Rng1>, range_difference_t<Rng2>>;
            using size_type_ = meta::_t<std::make_unsigned<difference_type_>>;

            static constexpr cardinality my_cardinality = detail::transform2_cardinality(
                range_cardinality<Rng1>::value,
                range_cardinality<Rng2>::value);

            struct cursor;

            struct sentinel
            {
            private:
                friend struct cursor;
                sentinel_t<Rng1> end1_;
                sentinel_t<Rng2> end2_;
            public:
                sentinel() = default;
                sentinel(detail::any, sentinel_t<Rng1> end1, sentinel_t<Rng2> end2)
                  : end1_(std::move(end1)), end2_(std::move(end2))
                {}
            };

            struct cursor
            {
            private:
                using fun_ref_ = semiregular_ref_or_val_t<Fun, true>;
                fun_ref_ fun_;
                iterator_t<Rng1> it1_;
                iterator_t<Rng2> it2_;

            public:
                using difference_type = difference_type_;
                using single_pass = meta::or_c<
                    (bool) SinglePass<iterator_t<Rng1>>,
                    (bool) SinglePass<iterator_t<Rng2>>>;
                using value_type =
                    detail::decay_t<invoke_result_t<Fun &, copy_tag, iterator_t<Rng1>,
                        iterator_t<Rng2>>>;

                cursor() = default;
                cursor(fun_ref_ fun, iterator_t<Rng1> it1, iterator_t<Rng2> it2)
                  : fun_(std::move(fun)), it1_(std::move(it1)), it2_(std::move(it2))
                {}
                auto CPP_auto_fun(read)() (const)
                (
                    return invoke(fun_, it1_, it2_)
                )
                void next()
                {
                    ++it1_;
                    ++it2_;
                }
                CPP_member
                auto equal(cursor const &that) const ->
                    CPP_ret(bool)(
                        requires ForwardRange<Rng1> && ForwardRange<Rng2>)
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // transformed ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return it1_ == that.it1_ || it2_ == that.it2_;
                }
                bool equal(sentinel const &s) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // transformed ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return it1_ == s.end1_ || it2_ == s.end2_;
                }
                CPP_member
                auto prev() -> CPP_ret(void)(
                    requires BidirectionalRange<Rng1> && BidirectionalRange<Rng2>)
                {
                    --it1_;
                    --it2_;
                }
                CPP_member
                auto advance(difference_type n) -> CPP_ret(void)(
                    requires RandomAccessRange<Rng1> && RandomAccessRange<Rng2>)
                {
                    ranges::advance(it1_, n);
                    ranges::advance(it2_, n);
                }
                CPP_member
                auto distance_to(cursor const &that) const ->
                    CPP_ret(difference_type)(
                    requires SizedSentinel<iterator_t<Rng1>, iterator_t<Rng1>> &&
                        SizedSentinel<iterator_t<Rng2>, iterator_t<Rng2>>)
                {
                    // Return the smallest distance (in magnitude) of any of the iterator
                    // pairs. This is to accommodate zippers of sequences of different length.
                    difference_type d1 = that.it1_ - it1_, d2 = that.it2_ - it2_;
                    return 0 < d1 ? ranges::min(d1, d2) : ranges::max(d1, d2);
                }
                auto CPP_auto_fun(move)() (const)
                (
                    return invoke(fun_, move_tag{}, it1_, it2_)
                )
            };

            using end_cursor_t =
                meta::if_c<
                    (bool)(BoundedRange<Rng1> && BoundedRange<Rng2> &&
                        !SinglePass<iterator_t<Rng1>> &&
                        !SinglePass<iterator_t<Rng2>>),
                    cursor,
                    sentinel>;

            cursor begin_cursor()
            {
                return {fun_, ranges::begin(rng1_), ranges::begin(rng2_)};
            }
            end_cursor_t end_cursor()
            {
                return {fun_, ranges::end(rng1_), ranges::end(rng2_)};
            }
            CPP_member
            auto begin_cursor() const -> CPP_ret(cursor)(
                requires Range<Rng1 const> && Range<Rng2 const>)
            {
                return {fun_, ranges::begin(rng1_), ranges::begin(rng2_)};
            }
            CPP_member
            auto end_cursor() const -> CPP_ret(end_cursor_t)(
                requires Range<Rng1 const> && Range<Rng2 const>)
            {
                return {fun_, ranges::end(rng1_), ranges::end(rng2_)};
            }
            template<typename Self>
            static constexpr size_type_ size_(Self& self)
            {
                return ranges::min(
                    static_cast<size_type_>(ranges::size(self.rng1_)),
                    static_cast<size_type_>(ranges::size(self.rng2_)));
            }
        public:
            iter_transform2_view() = default;
            iter_transform2_view(Rng1 rng1, Rng2 rng2, Fun fun)
              : fun_(std::move(fun))
              , rng1_(std::move(rng1))
              , rng2_(std::move(rng2))
            {}
            CPP_member
            constexpr auto size() const -> CPP_ret(size_type_)(
                requires my_cardinality >= 0)
            {
                return static_cast<size_type_>(my_cardinality);
            }
            CPP_member
            constexpr auto size() const -> CPP_ret(size_type_)(
                requires my_cardinality < 0 &&
                    SizedRange<Rng1 const> && SizedRange<Rng2 const>)
            {
                return size_(*this);
            }
            CPP_member
            constexpr /*c++14*/ auto size() -> CPP_ret(size_type_)(
                requires my_cardinality < 0 &&
                    SizedRange<Rng1> && SizedRange<Rng2>)
            {
                return size_(*this);
            }
        };

        template<typename Rng1, typename Rng2, typename Fun>
        struct transform2_view
          : iter_transform2_view<Rng1, Rng2, indirected<Fun>>
        {
            transform2_view() = default;
            transform2_view(Rng1 rng1, Rng2 rng2, Fun fun)
              : iter_transform2_view<Rng1, Rng2, indirected<Fun>>{std::move(rng1),
                    std::move(rng2), indirect(std::move(fun))}
            {}
        };

        CPP_def
        (
            template(typename Rng, typename Fun)
            concept IterTansformableRange,
                InputRange<Rng> &&
                CopyConstructible<Fun> &&
                Invocable<Fun&, iterator_t<Rng>> &&
                Invocable<Fun&, copy_tag, iterator_t<Rng>> &&
                Invocable<Fun&, move_tag, iterator_t<Rng>>
        );
        CPP_def
        (
            template(typename Rng1, typename Rng2, typename Fun)
            concept IterTansformableRanges,
                InputRange<Rng1> &&
                InputRange<Rng2> &&
                CopyConstructible<Fun> &&
                Invocable<Fun&, iterator_t<Rng1>, iterator_t<Rng2>> &&
                Invocable<Fun&, copy_tag, iterator_t<Rng1>, iterator_t<Rng2>> &&
                Invocable<Fun&, move_tag, iterator_t<Rng1>, iterator_t<Rng2>>
        );

        namespace view
        {
            struct iter_transform_fn
            {
            private:
                friend view_access;
                template<typename Fun>
                static auto bind(iter_transform_fn iter_transform, Fun fun)
                {
                    return make_pipeable(std::bind(iter_transform, std::placeholders::_1,
                        protect(std::move(fun))));
                }
            public:
                template<typename Rng, typename Fun>
                auto operator()(Rng &&rng, Fun fun) const ->
                    CPP_ret(iter_transform_view<all_t<Rng>, Fun>)(
                        requires IterTansformableRange<Rng, Fun>)
                {
                    return {all(static_cast<Rng &&>(rng)), std::move(fun)};
                }

                template<typename Rng1, typename Rng2, typename Fun>
                auto operator()(Rng1 &&rng1, Rng2 &&rng2, Fun fun) const ->
                    CPP_ret(iter_transform2_view<all_t<Rng1>, all_t<Rng2>, Fun>)(
                        requires IterTansformableRanges<Rng1, Rng2, Fun>)
                {
                    return {all(static_cast<Rng1 &&>(rng1)),
                            all(static_cast<Rng2 &&>(rng2)),
                            std::move(fun)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun>
                auto operator()(Rng &&, Fun) const ->
                    CPP_ret(void)(
                        requires not IterTansformableRange<Rng, Fun>)
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The object on which view::iter_transform operates must be a model of the "
                        "InputRange concept.");
                    CPP_assert_msg(
                        CopyConstructible<Fun>,
                        "The function passed to view::iter_transform must be CopyConstructible.");
                    CPP_assert_msg(
                        Invocable<Fun&, iterator_t<Rng>>,
                        "The function passed to view::iter_transform must be callable with an argument "
                        "of the range's iterator type.");
                    CPP_assert_msg(
                        Invocable<Fun&, copy_tag, iterator_t<Rng>>,
                        "The function passed to view::iter_transform must be callable with "
                        "copy_tag and an argument of the range's iterator type.");
                    CPP_assert_msg(
                        Invocable<Fun&, move_tag, iterator_t<Rng>>,
                        "The function passed to view::iter_transform must be callable with "
                        "move_tag and an argument of the range's iterator type.");
                }

                template<typename Rng1, typename Rng2, typename Fun>
                auto operator()(Rng1 &&, Rng2 &&, Fun) const ->
                    CPP_ret(void)(
                        requires not IterTansformableRanges<Rng1, Rng2, Fun>)
                {
                    CPP_assert_msg(InputRange<Rng1>,
                        "The first object on which view::iter_transform operates must be a model of the "
                        "InputRange concept.");
                    CPP_assert_msg(InputRange<Rng2>,
                        "The second object on which view::iter_transform operates must be a model of the "
                        "InputRange concept.");
                    CPP_assert_msg(
                        CopyConstructible<Fun>,
                        "The function passed to view::iter_transform must be CopyConstructible.");
                    CPP_assert_msg(
                        Invocable<Fun&, iterator_t<Rng1>, iterator_t<Rng2>>,
                        "The function passed to view::iter_transform must be callable with argument "
                        "of the ranges' iterator types.");
                    CPP_assert_msg(
                        Invocable<Fun&, copy_tag, iterator_t<Rng1>, iterator_t<Rng2>>,
                        "The function passed to view::iter_transform must be callable with "
                        "copy_tag and arguments of the ranges' iterator types.");
                    CPP_assert_msg(
                        Invocable<Fun&, move_tag, iterator_t<Rng1>, iterator_t<Rng2>>,
                        "The function passed to view::iter_transform must be callable with "
                        "move_tag and arguments of the rangess iterator types.");
                }
            #endif
            };

            /// \relates iter_transform_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<iter_transform_fn>, iter_transform)

            template<typename U>
            void non_void(U &&)
            {}

            // Don't forget to update view::for_each whenever this set
            // of concepts changes
            CPP_def
            (
                template(typename Rng, typename Fun)
                concept TransformableRange,
                    requires (Fun &fun, iterator_t<Rng> it)
                    (
                        non_void(ranges::invoke(fun, *it))
                    ) &&
                    InputRange<Rng> &&
                    CopyConstructible<Fun>
            );

            CPP_def
            (
                template(typename Rng1, typename Rng2, typename Fun)
                concept TransformableRanges,
                    requires (Fun &fun, iterator_t<Rng1> it1, iterator_t<Rng2> it2)
                    (
                        non_void(ranges::invoke(fun, *it1, *it2))
                    ) &&
                    InputRange<Rng1> &&
                    InputRange<Rng2> &&
                    CopyConstructible<Fun>
            );

            struct transform_fn
            {
            private:
                friend view_access;
                template<typename Fun>
                static auto bind(transform_fn transform, Fun fun)
                {
                    return make_pipeable(std::bind(transform, std::placeholders::_1,
                        protect(std::move(fun))));
                }
            public:
                template<typename Rng, typename Fun>
                auto operator()(Rng &&rng, Fun fun) const ->
                    CPP_ret(transform_view<all_t<Rng>, Fun>)(
                        requires TransformableRange<Rng, Fun>)
                {
                    return {all(static_cast<Rng &&>(rng)), std::move(fun)};
                }

                template<typename Rng1, typename Rng2, typename Fun>
                auto operator()(Rng1 &&rng1, Rng2 &&rng2, Fun fun) const ->
                    CPP_ret(transform2_view<all_t<Rng1>, all_t<Rng2>, Fun>)(
                        requires TransformableRanges<Rng1, Rng2, Fun>)
                {
                    return {all(static_cast<Rng1 &&>(rng1)), all(static_cast<Rng2 &&>(rng2)),
                        std::move(fun)};
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Rng, typename Fun>
                auto operator()(Rng &&, Fun) const ->
                    CPP_ret(void)(
                        requires not TransformableRange<Rng, Fun>)
                {
                    CPP_assert_msg(InputRange<Rng>,
                        "The object on which view::transform operates must be a model of the "
                        "InputRange concept.");
                    CPP_assert_msg(
                        CopyConstructible<Fun>,
                        "The function passed to view::transform must be CopyConstructible.");
                    CPP_assert_msg(
                        Invocable<Fun&, range_reference_t<Rng>>,
                        "The function passed to view::transform must be callable with an argument "
                        "of the range's reference type.");
                    CPP_assert_msg(
                        !Same<void, invoke_result_t<Fun&, range_reference_t<Rng>>>,
                        "The function passed to view::transform must return non-void when called "
                        "with an argument of the range's reference type.");
                }

                template<typename Rng1, typename Rng2, typename Fun>
                auto operator()(Rng1 &&, Rng2 &&, Fun) const ->
                    CPP_ret(void)(
                        requires not TransformableRanges<Rng1, Rng2, Fun>)
                {
                    CPP_assert_msg(InputRange<Rng1>,
                        "The first object on which view::transform operates must be a model of the "
                        "InputRange concept.");
                    CPP_assert_msg(InputRange<Rng2>,
                        "The second object on which view::transform operates must be a model of the "
                        "InputRange concept.");
                    CPP_assert_msg(
                        CopyConstructible<Fun>,
                        "The function passed to view::transform must be CopyConstructible.");
                    CPP_assert_msg(
                        Invocable<Fun&, range_reference_t<Rng1>, range_reference_t<Rng2>>,
                        "The function passed to view::transform must be callable with arguments "
                        "of the ranges' reference types.");
                    CPP_assert_msg(
                        !Same<void, invoke_result_t<
                            Fun&, range_reference_t<Rng1>, range_reference_t<Rng2>>>,
                        "The function passed to view::transform must return non-void when called "
                        "with arguments of the ranges' reference types.");
                }
            #endif
            };

            /// \relates transform_fn
            /// \ingroup group-views
            RANGES_INLINE_VARIABLE(view<transform_fn>, transform)
        }
        /// @}
    }
}

RANGES_SATISFY_BOOST_RANGE(::ranges::v3::iter_transform_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::v3::transform_view)

#endif
