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

#include <initializer_list>
#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>

#ifndef RANGES_NO_STD_FORWARD_DECLARATIONS
// Non-portable forward declarations of standard containers
RANGES_BEGIN_NAMESPACE_STD
    RANGES_BEGIN_NAMESPACE_CONTAINER
        template<typename Key,
                 typename Compare /*= less<Key>*/,
                 typename Alloc /*= allocator<Key>*/>
        class set;

        template<typename Key,
                 typename Compare /*= less<Key>*/,
                 typename Alloc /*= allocator<Key>*/>
        class multiset;

        template<typename Key,
                 typename Hash /*= hash<Key>*/,
                 typename Pred /*= equal_to<Key>*/,
                 typename Alloc /*= allocator<Key>*/>
        class unordered_set;

        template<typename Key,
                 typename Hash /*= hash<Key>*/,
                 typename Pred /*= equal_to<Key>*/,
                 typename Alloc /*= allocator<Key>*/>
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
    // clang-format off
    CPP_def
    (
        template(typename T)
        concept range_impl_,
            requires(T &&t) (
                ranges::begin(static_cast<T &&>(t)), // not necessarily equality-preserving
                ranges::end(static_cast<T &&>(t))
            )
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept range,
            range_impl_<T &>
    );

    /// \cond
    CPP_def
    (
        template(typename T)
        concept forwarding_range_,
            range<T> && range_impl_<T>
    );
    /// \endcond

    CPP_def
    (
        template(typename T, typename V)
        concept output_range,
            range<T> && output_iterator<iterator_t<T>, V>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept input_range_,
            input_iterator<iterator_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept input_range,
            range<T> && input_range_<T>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept forward_range_,
            forward_iterator<iterator_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept forward_range,
            input_range<T> && forward_range_<T>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept bidirectional_range_,
            bidirectional_iterator<iterator_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept bidirectional_range,
            forward_range<T> && bidirectional_range_<T>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept random_access_range_,
            random_access_iterator<iterator_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept random_access_range,
            bidirectional_range<T> && random_access_range_<T>
    );
    // clang-format on

    /// \cond
    namespace detail
    {
        template<typename Rng>
        using data_t = decltype(ranges::data(std::declval<Rng &>()));

        template<typename Rng>
        using element_t = meta::_t<std::remove_pointer<data_t<Rng>>>;
    } // namespace detail
      /// \endcond

    /// \cond
    // Needed to work around a bug in GCC
    // clang-format off
    CPP_def
    (
        template(typename T)
        concept contiguous_range_,
            contiguous_iterator<iterator_t<T>> &&
            same_as<detail::data_t<T>, meta::_t<std::add_pointer<iter_reference_t<iterator_t<T>>>>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept contiguous_range,
            random_access_range<T> && contiguous_range_<T>
    );

    /// \cond
    // Needed to work around a bug in GCC
    CPP_def
    (
        template(typename T)
        concept common_range_,
            same_as<iterator_t<T>, sentinel_t<T>>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept common_range,
            range<T> && common_range_<T>
    );

    /// \cond
    CPP_def
    (
        template(typename T)
        concept bounded_range,
            common_range<T>
    );
    /// \endcond

    CPP_def
    (
        template(typename T)
        concept sized_range,
            requires (T &t)
            (
                ranges::size(t)
            ) &&
            range<T> &&
            detail::integer_like_<range_size_t<T>> &&
            !disable_sized_range<uncvref_t<T>>
    );
    // clang-format on

    /// \cond
    namespace detail
    {
        struct enable_view_helper_
        {
            bool result_;

            template<typename T>
            static constexpr auto test(T const *) -> CPP_ret(bool)( //
                requires range<T> && range<T const>)
            {
                return RANGES_IS_SAME(iter_reference_t<iterator_t<T>>,
                                      iter_reference_t<iterator_t<T const>>);
            }
            static constexpr auto test(void const *) -> bool
            {
                return true;
            }
            template<typename T>
            constexpr enable_view_helper_(T const * p)
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
        constexpr bool enable_view_impl_(
            std::unordered_set<Key, Hash, Pred, Alloc> const *)
        {
            return false;
        }
        template<typename Key, typename Hash, typename Pred, typename Alloc>
        constexpr bool enable_view_impl_(
            std::unordered_multiset<Key, Hash, Pred, Alloc> const *)
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
        constexpr T const * nullptr_(int)
        {
            return nullptr;
        }
        template<typename T>
        constexpr int nullptr_(long)
        {
            return 0;
        }
    } // namespace detail
    /// \endcond

    // Specialize this if the default is wrong.
    template<typename T>
    RANGES_INLINE_VAR constexpr bool enable_view =
        detail::enable_view_impl_(detail::nullptr_<T>(0));

    ///
    /// View concepts below
    ///

    // clang-format off
    CPP_def
    (
        template(typename T)
        concept view_,
            range<T> &&
            semiregular<T> &&
            enable_view<T>
    );

    CPP_def
    (
        template(typename T)
        concept viewable_range,
            range<T> &&
            (forwarding_range_<T> || view_<detail::decay_t<T>>)
    );
    // clang-format on

    ////////////////////////////////////////////////////////////////////////////////////////////
    // range_tag
    using range_tag = ::concepts::tag<range_concept>;
    using input_range_tag = ::concepts::tag<input_range_concept, range_tag>;
    using forward_range_tag = ::concepts::tag<forward_range_concept, input_range_tag>;
    using bidirectional_range_tag =
        ::concepts::tag<bidirectional_range_concept, forward_range_tag>;
    using random_access_range_tag =
        ::concepts::tag<random_access_range_concept, bidirectional_range_tag>;
    using contiguous_range_tag =
        ::concepts::tag<contiguous_range_concept, random_access_range_tag>;

    template<typename T>
    using range_tag_of = ::concepts::tag_of<
        meta::list<contiguous_range_concept, random_access_range_concept,
                   bidirectional_range_concept, forward_range_concept,
                   input_range_concept, range_concept>,
        T>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // common_range_tag_of
    using common_range_tag = ::concepts::tag<common_range_concept, range_tag>;

    template<typename T>
    using common_range_tag_of =
        ::concepts::tag_of<meta::list<common_range_concept, range_concept>, T>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // sized_range_concept
    using sized_range_tag = ::concepts::tag<sized_range_concept, range_tag>;

    template<typename T>
    using sized_range_tag_of =
        ::concepts::tag_of<meta::list<sized_range_concept, range_concept>, T>;

    /// \cond
    namespace view_detail_
    {
        // clang-format off
        CPP_def
        (
            template(typename T)
            concept view,
                ranges::view_<T>
        );
        // clang-format on
    } // namespace view_detail_
    /// \endcond

    namespace cpp20
    {
        using ranges::bidirectional_range;
        using ranges::common_range;
        using ranges::contiguous_range;
        using ranges::enable_view;
        using ranges::forward_range;
        using ranges::input_range;
        using ranges::output_range;
        using ranges::random_access_range;
        using ranges::range;
        using ranges::sized_range;
        using ranges::viewable_range;
        using ranges::view_detail_::view;
    } // namespace cpp20
    /// @}
} // namespace ranges

#endif
