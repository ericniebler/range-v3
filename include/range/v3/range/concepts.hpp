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

#ifndef RANGES_V3_RANGE_CONCEPTS_HPP
#define RANGES_V3_RANGE_CONCEPTS_HPP

#include <utility>
#include <type_traits>
#include <initializer_list>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>

#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
// Non-portable forward declarations of standard containers
RANGES_BEGIN_NAMESPACE_STD
RANGES_BEGIN_NAMESPACE_CONTAINER
    template<typename Key, typename Compare /*= less<Key>*/, typename Alloc /*= allocator<Key>*/>
    class set;

    template<typename Key, typename Compare /*= less<Key>*/, typename Alloc /*= allocator<Key>*/>
    class multiset;

    template<typename Key, typename Hash /*= hash<Key>*/, typename Pred /*= equal_to<Key>*/, typename Alloc /*= allocator<Key>*/>
    class unordered_set;

    template<typename Key, typename Hash /*= hash<Key>*/, typename Pred /*= equal_to<Key>*/, typename Alloc /*= allocator<Key>*/>
    class unordered_multiset;
RANGES_END_NAMESPACE_CONTAINER
RANGES_END_NAMESPACE_STD
#else
#include <set>
#include <unordered_set>
#endif

namespace ranges
{
    /// \addtogroup group-range
    /// @{

    ///
    /// Range concepts below
    ///

    /// \cond
    CPP_def
    (
        template(typename T)
        concept RangeImpl_,
            requires(T &&t) (
                ranges::begin(static_cast<T &&>(t)), // not necessarily equality-preserving
                ranges::end(static_cast<T &&>(t))
            )
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept Range,
            RangeImpl_<T &>
    );

    /// \cond
    CPP_def
    (
        template(typename T)
        concept ForwardingRange_,
            Range<T> && RangeImpl_<T>
    );
    /// \endcond

    CPP_def
    (
        template(typename T, typename V)
        concept OutputRange,
            Range<T> && OutputIterator<iterator_t<T>, V>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept InputRange_,
            InputIterator<iterator_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept InputRange,
            Range<T> && InputRange_<T>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept ForwardRange_,
            ForwardIterator<iterator_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept ForwardRange,
            InputRange<T> && ForwardRange_<T>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept BidirectionalRange_,
            BidirectionalIterator<iterator_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept BidirectionalRange,
            ForwardRange<T> && BidirectionalRange_<T>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept RandomAccessRange_,
            RandomAccessIterator<iterator_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept RandomAccessRange,
            BidirectionalRange<T> && RandomAccessRange_<T>
    );

    /// \cond
    namespace detail
    {
        template<typename Rng>
        using data_t = decltype(ranges::data(std::declval<Rng&>()));

        template<typename Rng>
        using element_t = meta::_t<std::remove_pointer<data_t<Rng>>>;
    }
    /// \endcond

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept ContiguousRange_,
            ContiguousIterator<iterator_t<T>> &&
            Same<detail::data_t<T>, meta::_t<std::add_pointer<iter_reference_t<iterator_t<T>>>>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept ContiguousRange,
            RandomAccessRange<T> && ContiguousRange_<T>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept CommonRange_,
            Same<iterator_t<T>, sentinel_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept CommonRange,
            Range<T> && CommonRange_<T>
    );

    /// \cond
    CPP_def
    (
        template(typename T)
        concept BoundedRange,
            CommonRange<T>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept SizedRange,
            requires (T &t)
            (
                ranges::size(t)
            ) &&
            Range<T> &&
            detail::IntegerLike_<range_size_t<T>> &&
            !disable_sized_range<uncvref_t<T>>
    );

    /// \cond
    namespace detail
    {
        struct enable_view_helper_
        {
            bool result_;

            template<typename T>
            static constexpr auto test(T const *) -> CPP_ret(bool)(
                requires Range<T> && Range<T const>)
            {
                return RANGES_IS_SAME(
                    iter_reference_t<iterator_t<T>>,
                    iter_reference_t<iterator_t<T const>>);
            }
            static constexpr auto test(void const *) -> bool
            {
                return true;
            }
            template<typename T>
            constexpr enable_view_helper_(T const *p)
              : result_(enable_view_helper_::test(p))
            {}
        };
        constexpr bool enable_view_impl_(...)
        {
            return false;
        }
        constexpr bool enable_view_impl_(view_base const *)
        {
            return true;
        }
        constexpr bool enable_view_impl_(enable_view_helper_ ev)
        {
            return ev.result_;
        }
        template<typename T>
        constexpr bool enable_view_impl_(std::initializer_list<T> const *)
        {
            return false;
        }
        template<typename Key, typename Compare, typename Alloc>
        constexpr bool enable_view_impl_(std::set<Key, Compare, Alloc> const *)
        {
            return false;
        }
        template<typename Key, typename Compare, typename Alloc>
        constexpr bool enable_view_impl_(std::multiset<Key, Compare, Alloc> const *)
        {
            return false;
        }
        template<typename Key, typename Hash, typename Pred, typename Alloc>
        constexpr bool enable_view_impl_(std::unordered_set<Key, Hash, Pred, Alloc> const *)
        {
            return false;
        }
        template<typename Key, typename Hash, typename Pred, typename Alloc>
        constexpr bool enable_view_impl_(std::unordered_multiset<Key, Hash, Pred, Alloc> const *)
        {
            return false;
        }
        // BUGBUG TODO
        // template<typename BidiIter, typename Alloc>
        // constexpr bool enable_view_impl_(std::match_results<BidiIter, Alloc> const *)
        // {
        //     return false;
        // }
        template<typename T>
        constexpr T const *nullptr_(int)
        {
            return nullptr;
        }
        template<typename T>
        constexpr int nullptr_(long)
        {
            return 0;
        }
    }
    /// \endcond

    // Specialize this if the default is wrong.
    template<typename T>
    RANGES_INLINE_VAR constexpr bool enable_view =
        detail::enable_view_impl_(detail::nullptr_<T>(0));

    ///
    /// View concepts below
    ///

    CPP_def
    (
        template(typename T)
        concept View,
            Range<T> &&
            Semiregular<T> &&
            enable_view<T>
    );

    /// \cond
    // Non-standard, avoid using
    CPP_def
    (
        template(typename T, typename V)
        concept OutputView,
            View<T> && OutputRange<T, V>
    );

    CPP_def
    (
        template(typename T)
        concept InputView,
            View<T> && InputRange<T>
    );

    CPP_def
    (
        template(typename T)
        concept ForwardView,
            View<T> && ForwardRange<T>
    );

    CPP_def
    (
        template(typename T)
        concept BidirectionalView,
            View<T> && BidirectionalRange<T>
    );

    CPP_def
    (
        template(typename T)
        concept RandomAccessView,
            View<T> && RandomAccessRange<T>
    );

    CPP_def
    (
        template(typename T)
        concept ContiguousView,
            RandomAccessView<T> && ContiguousRange<T>
    );
    /// \endcond

    // Additional concepts for checking additional orthogonal properties
    CPP_def
    (
        template(typename T)
        concept CommonView,
            View<T> && CommonRange<T>
    );

    /// \cond
    CPP_def
    (
        template(typename T)
        concept BoundedView,
            CommonView<T>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept SizedView,
            View<T> && SizedRange<T>
    );

    // Not to spec because of move-only Views.
    CPP_def
    (
        template(typename T)
        concept ViewableRange,
            Range<T> &&
            (ForwardingRange_<T> || View<detail::decay_t<T>>)
    );

    ////////////////////////////////////////////////////////////////////////////////////////////
    // range_tag
    using range_tag = ::concepts::tag<RangeConcept>;
    using input_range_tag = ::concepts::tag<InputRangeConcept, range_tag>;
    using forward_range_tag = ::concepts::tag<ForwardRangeConcept, input_range_tag>;
    using bidirectional_range_tag = ::concepts::tag<BidirectionalRangeConcept, forward_range_tag>;
    using random_access_range_tag = ::concepts::tag<RandomAccessRangeConcept, bidirectional_range_tag>;
    using contiguous_range_tag = ::concepts::tag<ContiguousRangeConcept, random_access_range_tag>;

    template<typename T>
    using range_tag_of =
        ::concepts::tag_of<
            meta::list<
                ContiguousRangeConcept,
                RandomAccessRangeConcept,
                BidirectionalRangeConcept,
                ForwardRangeConcept,
                InputRangeConcept,
                RangeConcept>,
            T>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // common_range_tag_of
    using common_range_tag = ::concepts::tag<CommonRangeConcept, range_tag>;

    template<typename T>
    using common_range_tag_of =
        ::concepts::tag_of<
            meta::list<
                CommonRangeConcept,
                RangeConcept>,
            T>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // sized_range_concept
    using sized_range_tag = ::concepts::tag<SizedRangeConcept, range_tag>;

    template<typename T>
    using sized_range_tag_of =
        ::concepts::tag_of<
            meta::list<
                SizedRangeConcept,
                RangeConcept>,
            T>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // common_view_tag_of
    using view_tag = ::concepts::tag<ViewConcept, range_tag>;
    using common_view_tag = ::concepts::tag<CommonViewConcept, view_tag>;

    template<typename T>
    using common_view_tag_of =
        ::concepts::tag_of<
            meta::list<
                CommonViewConcept,
                ViewConcept,
                RangeConcept>,
            T>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // sized_view_tag_of
    using sized_view_tag = ::concepts::tag<SizedViewConcept, view_tag>;

    template<typename T>
    using sized_view_tag_of =
        ::concepts::tag_of<
            meta::list<
                SizedViewConcept,
                ViewConcept,
                RangeConcept>,
            T>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // view_concept
    template<typename T>
    using view_tag_of =
        ::concepts::tag_of<
            meta::list<
                ViewConcept,
                RangeConcept>,
            T>;

    namespace cpp20
    {
        using ranges::Range;
        using ranges::SizedRange;
        // Specialize this is namespace ranges::
        using ranges::enable_view;
        using ranges::View;
        using ranges::OutputRange;
        using ranges::InputRange;
        using ranges::ForwardRange;
        using ranges::BidirectionalRange;
        using ranges::RandomAccessRange;
        using ranges::ContiguousRange;
        using ranges::CommonRange;
        using ranges::ViewableRange;
    }
    /// @}
}

#endif
