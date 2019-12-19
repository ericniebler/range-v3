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

#include <range/v3/detail/disable_warnings.hpp>

namespace ranges
{
    /// \addtogroup group-range
    /// @{

    ///
    /// Range concepts below
    ///

    // clang-format off
    template<typename T>
    CPP_concept_bool range =
        CPP_requires ((T &) t) //
        (
            ranges::begin(t), // not necessarily equality-preserving
            ranges::end(t)
        );

    template<typename T>
    CPP_concept_bool safe_range =
        range<T> && detail::_safe_range<T>;

    template<typename T, typename V>
    CPP_concept_fragment(output_range_, (T, V),
        output_iterator<iterator_t<T>, V>
    );
    template<typename T, typename V>
    CPP_concept_bool output_range =
        range<T> && CPP_fragment(ranges::output_range_, T, V);

    template<typename T>
    CPP_concept_fragment(input_range_, (T),
        input_iterator<iterator_t<T>>
    );
    template<typename T>
    CPP_concept_bool input_range =
        range<T> && CPP_fragment(ranges::input_range_, T);

    template<typename T>
    CPP_concept_fragment(forward_range_, (T),
        forward_iterator<iterator_t<T>>
    );
    template<typename T>
    CPP_concept_bool forward_range =
        input_range<T> && CPP_fragment(ranges::forward_range_, T);

    template<typename T>
    CPP_concept_fragment(bidirectional_range_, (T),
        bidirectional_iterator<iterator_t<T>>
    );
    template<typename T>
    CPP_concept_bool bidirectional_range =
        forward_range<T> && CPP_fragment(ranges::bidirectional_range_, T);

    template<typename T>
    CPP_concept_fragment(random_access_range_, (T),
        random_access_iterator<iterator_t<T>>
    );

    template<typename T>
    CPP_concept_bool random_access_range =
        bidirectional_range<T> && CPP_fragment(ranges::random_access_range_, T);
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

    // clang-format off
    template<typename T>
    CPP_concept_fragment(contiguous_range_, (T),
        contiguous_iterator<iterator_t<T>> &&
        same_as<detail::data_t<T>, std::add_pointer_t<iter_reference_t<iterator_t<T>>>>
    );

    template<typename T>
    CPP_concept_bool contiguous_range =
        random_access_range<T> && CPP_fragment(ranges::contiguous_range_, T);

    template<typename T>
    CPP_concept_fragment(common_range_, (T),
        same_as<iterator_t<T>, sentinel_t<T>>
    );

    template<typename T>
    CPP_concept_bool common_range =
        range<T> && CPP_fragment(ranges::common_range_, T);

    /// \cond
    template<typename T>
    CPP_concept_bool bounded_range =
        common_range<T>;
    /// \endcond

    template<typename T>
    CPP_concept_fragment(sized_range_, (T),
        detail::integer_like_<range_size_t<T>>
    );

    template<typename T>
    CPP_concept_bool sized_range =
        range<T> &&
        !disable_sized_range<uncvref_t<T>> &&
        CPP_requires ((T &) t) //
        (
            ranges::size(t)
        ) &&
        CPP_fragment(ranges::sized_range_, T);
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
    template<typename T>
    CPP_concept_bool view_ =
        range<T> &&
        semiregular<T> &&
        enable_view<T>;

    template<typename T>
    CPP_concept_bool viewable_range =
        range<T> &&
        (safe_range<T> || view_<uncvref_t<T>>);
    // clang-format on

    ////////////////////////////////////////////////////////////////////////////////////////////
    // range_tag
    struct range_tag
    {};

    struct input_range_tag : range_tag
    {};
    struct forward_range_tag : input_range_tag
    {};
    struct bidirectional_range_tag : forward_range_tag
    {};
    struct random_access_range_tag : bidirectional_range_tag
    {};
    struct contiguous_range_tag : random_access_range_tag
    {};

    template<typename Rng>
    using range_tag_of =                          //
        std::enable_if_t<                         //
            range<Rng>,                           //
            detail::if_then_t<                    //
                contiguous_range<Rng>,            //
                contiguous_range_tag,             //
                detail::if_then_t<                //
                    random_access_range<Rng>,     //
                    random_access_range_tag,      //
                    detail::if_then_t<            //
                        bidirectional_range<Rng>, //
                        bidirectional_range_tag,  //
                        detail::if_then_t<        //
                            forward_range<Rng>,   //
                            forward_range_tag,    //
                            detail::if_then_t<    //
                                input_range<Rng>, //
                                input_range_tag,  //
                                range_tag>>>>>>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // common_range_tag_of
    struct common_range_tag : range_tag
    {};

    template<typename Rng>
    using common_range_tag_of = //
        std::enable_if_t<       //
            range<Rng>,         //
            detail::if_then_t<common_range<Rng>, common_range_tag, range_tag>>;

    ////////////////////////////////////////////////////////////////////////////////////////////
    // sized_range_concept
    struct sized_range_tag : range_tag
    {};

    template<typename Rng>
    using sized_range_tag_of = //
        std::enable_if_t<      //
            range<Rng>,        //
            detail::if_then_t<sized_range<Rng>, sized_range_tag, range_tag>>;

    namespace defer
    {
        template<typename T>
        CPP_concept range = CPP_defer(ranges::range, T);

        template<typename T>
        CPP_concept safe_range = CPP_defer(ranges::safe_range, T);

        template<typename T, typename V>
        CPP_concept output_range = CPP_defer(ranges::output_range, T, V);

        template<typename T>
        CPP_concept input_range = CPP_defer(ranges::input_range, T);

        template<typename T>
        CPP_concept forward_range = CPP_defer(ranges::forward_range, T);

        template<typename T>
        CPP_concept bidirectional_range = CPP_defer(ranges::bidirectional_range, T);

        template<typename T>
        CPP_concept random_access_range = CPP_defer(ranges::random_access_range, T);

        template<typename T>
        CPP_concept contiguous_range = CPP_defer(ranges::contiguous_range, T);

        template<typename T>
        CPP_concept common_range = CPP_defer(ranges::common_range, T);

        template<typename T>
        CPP_concept sized_range = CPP_defer(ranges::sized_range, T);

        template<typename T>
        CPP_concept view_ = CPP_defer(ranges::view_, T);

        template<typename T>
        CPP_concept viewable_range = CPP_defer(ranges::viewable_range, T);
    } // namespace defer

    /// \cond
    namespace view_detail_
    {
        // clang-format off
        template<typename T>
        CPP_concept_bool view =
            ranges::view_<T>;
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

#include <range/v3/detail/reenable_warnings.hpp>

#endif
