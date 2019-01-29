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
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/view/adaptor.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/algorithm/min.hpp>
#include <range/v3/functional/indirect.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/view.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
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

        CPP_def
        (
            template(typename Fun, typename Rng)
            concept IterTransform1Readable,
                RegularInvocable<Fun &, iterator_t<Rng>> &&
                RegularInvocable<Fun &, copy_tag, iterator_t<Rng>> &&
                RegularInvocable<Fun &, move_tag, iterator_t<Rng>> &&
                CommonReference<
                    invoke_result_t<Fun &, iterator_t<Rng>> &&,
                    invoke_result_t<Fun &, copy_tag, iterator_t<Rng>> &> &&
                CommonReference<
                    invoke_result_t<Fun &, iterator_t<Rng>> &&,
                    invoke_result_t<Fun &, move_tag, iterator_t<Rng>> &&> &&
                CommonReference<
                    invoke_result_t<Fun &, move_tag, iterator_t<Rng>> &&,
                    invoke_result_t<Fun &, copy_tag, iterator_t<Rng>> const &>
        );

        CPP_def
        (
            template(typename Fun, typename Rng1, typename Rng2)
            concept IterTransform2Readable,
                RegularInvocable<Fun &, iterator_t<Rng1>, iterator_t<Rng2>> &&
                RegularInvocable<Fun &, copy_tag, iterator_t<Rng1>, iterator_t<Rng2>> &&
                RegularInvocable<Fun &, move_tag, iterator_t<Rng1>, iterator_t<Rng2>> &&
                CommonReference<
                    invoke_result_t<Fun &, iterator_t<Rng1>, iterator_t<Rng2>> &&,
                    invoke_result_t<Fun &, copy_tag, iterator_t<Rng1>, iterator_t<Rng2>> &> &&
                CommonReference<
                    invoke_result_t<Fun &, iterator_t<Rng1>, iterator_t<Rng2>> &&,
                    invoke_result_t<Fun &, move_tag, iterator_t<Rng1>, iterator_t<Rng2>> &&> &&
                CommonReference<
                    invoke_result_t<Fun &, move_tag, iterator_t<Rng1>, iterator_t<Rng2>> &&,
                    invoke_result_t<Fun &, copy_tag, iterator_t<Rng1>, iterator_t<Rng2>> const &>
        );
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
        template<bool Const>
        using use_sentinel_t =
            meta::bool_<!CommonRange<meta::const_if_c<Const, Rng>> ||
                SinglePass<iterator_t<meta::const_if_c<Const, Rng>>>>;

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
                requires IsConst && (!Other))
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
        template<bool Const = true>
        auto begin_adaptor() const -> CPP_ret(adaptor<Const>)(
            requires Const && Range<meta::const_if_c<Const, Rng>> &&
                detail::IterTransform1Readable<Fun const, meta::const_if_c<Const, Rng>>)
        {
            return {fun_};
        }
        meta::if_<use_sentinel_t<false>, adaptor_base, adaptor<false>> end_adaptor()
        {
            return {fun_};
        }
        template<bool Const = true>
        auto end_adaptor() const ->
            CPP_ret(meta::if_<use_sentinel_t<Const>, adaptor_base, adaptor<Const>>)(
                requires Const && Range<meta::const_if_c<Const, Rng>> &&
                    detail::IterTransform1Readable<Fun const, meta::const_if_c<Const, Rng>>)
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
                return static_cast<range_size_type_t<Rng>>(ranges::size(this->base()));
        }
        CPP_member
        constexpr auto CPP_fun(size)() (const
            requires SizedRange<Rng const>)
        {
                return static_cast<range_size_type_t<Rng>>(ranges::size(this->base()));
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

        template<bool>
        struct cursor;

        template<bool Const>
        struct sentinel
        {
        private:
            friend struct cursor<Const>;
            sentinel_t<meta::const_if_c<Const, Rng1>> end1_;
            sentinel_t<meta::const_if_c<Const, Rng2>> end2_;
        public:
            sentinel() = default;
            sentinel(meta::const_if_c<Const, iter_transform2_view> &parent, decltype(end))
              : end1_(end(parent.rng1_)), end2_(end(parent.rng2_))
            {}
            template<bool Other>
            CPP_ctor(sentinel)(sentinel<Other> that)(
                requires Const && (!Other))
              : end1_(std::move(that.end1_))
              , end2_(std::move(that.end2_))
            {}
        };

        template<bool Const>
        struct cursor
        {
        private:
            using fun_ref_ = semiregular_ref_or_val_t<Fun, Const>;
            using R1 = meta::const_if_c<Const, Rng1>;
            using R2 = meta::const_if_c<Const, Rng2>;
            fun_ref_ fun_;
            iterator_t<R1> it1_;
            iterator_t<R2> it2_;

        public:
            using difference_type = difference_type_;
            using single_pass = meta::or_c<
                (bool) SinglePass<iterator_t<R1>>,
                (bool) SinglePass<iterator_t<R2>>>;
            using value_type =
                detail::decay_t<
                    invoke_result_t<
                        meta::const_if_c<Const, Fun> &,
                        copy_tag,
                        iterator_t<R1>,
                        iterator_t<R2>>>;

            cursor() = default;
            template<typename BeginEndFn>
            cursor(meta::const_if_c<Const, iter_transform2_view> &parent, BeginEndFn begin_end)
              : fun_(parent.fun_), it1_(begin_end(parent.rng1_)), it2_(begin_end(parent.rng2_))
            {}
            template<bool Other>
            CPP_ctor(cursor)(cursor<Other> that)(
                requires Const && (!Other))
              : fun_(std::move(that.fun_))
              , it1_(std::move(that.end1_))
              , it2_(std::move(that.end2_))
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
            bool equal(sentinel<Const> const &s) const
            {
                // By returning true if *any* of the iterators are equal, we allow
                // transformed ranges to be of different lengths, stopping when the first
                // one reaches the end.
                return it1_ == s.end1_ || it2_ == s.end2_;
            }
            CPP_member
            auto prev() -> CPP_ret(void)(
                requires BidirectionalRange<R1> && BidirectionalRange<R2>)
            {
                --it1_;
                --it2_;
            }
            CPP_member
            auto advance(difference_type n) -> CPP_ret(void)(
                requires RandomAccessRange<R1> && RandomAccessRange<R2>)
            {
                ranges::advance(it1_, n);
                ranges::advance(it2_, n);
            }
            CPP_member
            auto distance_to(cursor const &that) const ->
                CPP_ret(difference_type)(
                    requires
                        SizedSentinel<iterator_t<R1>, iterator_t<R1>> &&
                        SizedSentinel<iterator_t<R2>, iterator_t<R2>>)
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

        template<bool Const>
        using end_cursor_t =
            meta::if_c<
                BoundedRange<meta::const_if_c<Const, Rng1>> &&
                BoundedRange<meta::const_if_c<Const, Rng2>> &&
                !SinglePass<iterator_t<meta::const_if_c<Const, Rng1>>> &&
                !SinglePass<iterator_t<meta::const_if_c<Const, Rng2>>>,
                cursor<Const>,
                sentinel<Const>>;

        cursor<simple_view<Rng1>() && simple_view<Rng2>()>
        begin_cursor()
        {
            return {*this, ranges::begin};
        }
        end_cursor_t<simple_view<Rng1>() && simple_view<Rng2>()> end_cursor()
        {
            return {*this, ranges::end};
        }
        template<bool Const = true>
        auto begin_cursor() const ->
            CPP_ret(cursor<true>)(
                requires Const &&
                    Range<meta::const_if_c<Const, Rng1>> &&
                    Range<meta::const_if_c<Const, Rng2>> &&
                    detail::IterTransform2Readable<
                        Fun const,
                        meta::const_if_c<Const, Rng1>,
                        meta::const_if_c<Const, Rng2>>)
        {
            return {*this, ranges::begin};
        }
        template<bool Const = true>
        auto end_cursor() const ->
            CPP_ret(end_cursor_t<Const>)(
                requires Const &&
                    Range<meta::const_if_c<Const, Rng1>> &&
                    Range<meta::const_if_c<Const, Rng2>> &&
                    detail::IterTransform2Readable<
                        Fun const,
                        meta::const_if_c<Const, Rng1>,
                        meta::const_if_c<Const, Rng2>>)
        {
            return {*this, ranges::end};
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
        static constexpr auto size() -> CPP_ret(size_type_)(
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
                    requires ViewableRange<Rng> && InputRange<Rng> &&
                        CopyConstructible<Fun> &&
                        detail::IterTransform1Readable<Fun, Rng>)
            {
                return {all(static_cast<Rng &&>(rng)), std::move(fun)};
            }

            template<typename Rng1, typename Rng2, typename Fun>
            auto operator()(Rng1 &&rng1, Rng2 &&rng2, Fun fun) const ->
                CPP_ret(iter_transform2_view<all_t<Rng1>, all_t<Rng2>, Fun>)(
                    requires ViewableRange<Rng1> && InputRange<Rng1> &&
                        ViewableRange<Rng2> && InputRange<Rng2> &&
                        CopyConstructible<Fun> &&
                        detail::IterTransform2Readable<Fun, Rng1, Rng2>)
            {
                return {all(static_cast<Rng1 &&>(rng1)),
                        all(static_cast<Rng2 &&>(rng2)),
                        std::move(fun)};
            }
        };

        /// \relates iter_transform_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<iter_transform_fn>, iter_transform)

        // Don't forget to update view::for_each whenever this set
        // of concepts changes
        CPP_def
        (
            template(typename Rng, typename Fun)
            concept TransformableRange,
                ViewableRange<Rng> && InputRange<Rng> &&
                CopyConstructible<Fun> &&
                RegularInvocable<Fun &, range_reference_t<Rng>> &&
                (!std::is_void<indirect_result_t<Fun &, iterator_t<Rng>>>::value)
        );

        CPP_def
        (
            template(typename Rng1, typename Rng2, typename Fun)
            concept TransformableRanges,
                ViewableRange<Rng1> && InputRange<Rng1> &&
                ViewableRange<Rng2> && InputRange<Rng2> &&
                CopyConstructible<Fun> &&
                (!std::is_void<
                    indirect_result_t<Fun &, iterator_t<Rng1>, iterator_t<Rng2>>>::value)
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
        };

        /// \relates transform_fn
        /// \ingroup group-views
        RANGES_INLINE_VARIABLE(view<transform_fn>, transform)
    }
    /// @}
}

#include <range/v3/detail/satisfy_boost_range.hpp>
RANGES_SATISFY_BOOST_RANGE(::ranges::iter_transform_view)
RANGES_SATISFY_BOOST_RANGE(::ranges::transform_view)

#endif
