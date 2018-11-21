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
#include <range/v3/range_fwd.hpp>
#include <range/v3/data.hpp>
#include <range/v3/size.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/range_traits.hpp>

#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
// Non-portable forward declarations of standard containers
RANGES_BEGIN_NAMESPACE_STD
    template<typename Key, typename Compare /*= less<Key>*/, typename Alloc /*= allocator<Key>*/>
    class set;

    template<typename Key, typename Compare /*= less<Key>*/, typename Alloc /*= allocator<Key>*/>
    class multiset;

    template<typename Key, typename Hash /*= hash<Key>*/, typename Pred /*= equal_to<Key>*/, typename Alloc /*= allocator<Key>*/>
    class unordered_set;

    template<typename Key, typename Hash /*= hash<Key>*/, typename Pred /*= equal_to<Key>*/, typename Alloc /*= allocator<Key>*/>
    class unordered_multiset;
RANGES_END_NAMESPACE_STD
#else
#include <set>
#include <unordered_set>
#endif

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename T>
            struct view_predicate_;
        }
        /// \endcond

        /// \addtogroup group-concepts
        /// @{

        // Specialize this if the default is wrong.
        template<typename T>
        struct enable_view
        {};

        ///
        /// Range concepts below
        ///
        CPP_def
        (
            template(typename T)
            concept Range,
                //Sentinel<sentinel_t<T>, iterator_t<T>>
                requires (T& t)
                (
                    ranges::end(t)
                )
        );

        CPP_def
        (
            template(typename T, typename V)
            concept OutputRange,
                Range<T> && OutputIterator<iterator_t<T>, V>
        );

        CPP_def
        (
            template(typename T)
            concept InputRange,
                Range<T> && InputIterator<iterator_t<T>>
        );

        CPP_def
        (
            template(typename T)
            concept ForwardRange,
                InputRange<T> && ForwardIterator<iterator_t<T>>
        );

        CPP_def
        (
            template(typename T)
            concept BidirectionalRange,
                ForwardRange<T> && BidirectionalIterator<iterator_t<T>>
        );

        CPP_def
        (
            template(typename T)
            concept RandomAccessRange,
                BidirectionalRange<T> && RandomAccessIterator<iterator_t<T>>
        );

        /// \cond
        namespace detail
        {
            template<typename Rng>
            using data_reference_t = decltype(*data(std::declval<Rng&>()));

            template<typename Rng>
            using element_t = meta::_t<std::remove_reference<data_reference_t<Rng>>>;
        }
        /// \endcond

        CPP_def
        (
            template(typename T)
            concept ContiguousRange,
                RandomAccessRange<T> &&
                Same<range_value_t<T>, meta::_t<std::remove_cv<detail::element_t<T>>>> &&
                Same<detail::data_reference_t<T>, range_reference_t<T>>
        );

        CPP_def
        (
            template(typename T)
            concept BoundedRange,
                Range<T> &&
                Same<iterator_t<T>, sentinel_t<T>>
        );

        CPP_def
        (
            template(typename T)
            concept SizedRange,
                requires (T &t)
                (
                    ranges::size(t),
                    concepts::requires_<Integral<decltype(ranges::size(t))>>
                ) &&
                Range<T> && !disable_sized_range<uncvref_t<T>>
        );

        ///
        /// View concepts below
        ///

        CPP_def
        (
            template(typename T)
            concept View,
                Range<T> && Movable<T> && DefaultConstructible<T> &&
                detail::view_predicate_<T>::value
        );

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

        // Additional concepts for checking additional orthogonal properties
        CPP_def
        (
            template(typename T)
            concept BoundedView,
                View<T> && BoundedRange<T>
        );

        CPP_def
        (
            template(typename T)
            concept SizedView,
                View<T> && SizedRange<T>
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
        // bounded_range_tag_of
        using bounded_range_tag = ::concepts::tag<BoundedRangeConcept, range_tag>;

        template<typename T>
        using bounded_range_tag_of =
            ::concepts::tag_of<
                meta::list<
                    BoundedRangeConcept,
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
        // bounded_view_tag_of
        using view_tag = ::concepts::tag<ViewConcept, range_tag>;
        using bounded_view_tag = ::concepts::tag<BoundedViewConcept, view_tag>;

        template<typename T>
        using bounded_view_tag_of =
            ::concepts::tag_of<
                meta::list<
                    BoundedViewConcept,
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

        /// @}

        /// \cond
        namespace detail
        {
            template<typename T>
            std::is_same<iter_reference_t<iterator_t<T>>, iter_reference_t<iterator_t<T const>>>
            view_like_(int);

            template<typename T>
            std::true_type
            view_like_(long);

            template<typename T>
            using view_like = decltype(detail::view_like_<T>(42));

            // Something is a view if it's a Range and either:
            //  - It doesn't look like a container, or
            //  - It's derived from view_base
            template<typename T>
            struct view_predicate_
              : meta::_t<meta::if_<
                    meta::is_trait<enable_view<T>>,
                    enable_view<T>,
                    meta::bool_<view_like<T>() || (bool) DerivedFrom<T, view_base>>>>
            {};

            template<typename T>
            struct view_predicate_<std::initializer_list<T>>
              : std::false_type
            {};

            template<typename Key, typename Compare, typename Alloc>
            struct view_predicate_<std::set<Key, Compare, Alloc>>
              : std::false_type
            {};

            template<typename Key, typename Compare, typename Alloc>
            struct view_predicate_<std::multiset<Key, Compare, Alloc>>
              : std::false_type
            {};

            template<typename Key, typename Hash, typename Pred, typename Alloc>
            struct view_predicate_<std::unordered_set<Key, Hash, Pred, Alloc>>
              : std::false_type
            {};

            template<typename Key, typename Hash, typename Pred, typename Alloc>
            struct view_predicate_<std::unordered_multiset<Key, Hash, Pred, Alloc>>
              : std::false_type
            {};

            template<typename T>
            struct is_view_
              : meta::bool_<(bool)View<T>>
            {};

            template<typename T>
            struct is_range_
              : meta::bool_<(bool)Range<T>>
            {};
        }
        /// \endcond
    }
}

#endif
