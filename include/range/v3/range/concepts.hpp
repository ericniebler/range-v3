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

#ifdef __has_include
#if __has_include(<span>)
#include <span>
#endif
#if __has_include(<string_view>)
#include <string_view>
#endif
#endif

#include <meta/meta.hpp>

#include <concepts/concepts.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/comparisons.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range/traits.hpp>

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
    CPP_concept_fragment(_range_,
        requires(T & t) //
        (
            ranges::begin(t), // not necessarily equality-preserving
            ranges::end(t)
        ));
    template<typename T>
    CPP_concept_bool range =
        CPP_fragment(ranges::_range_, T);

    template<typename T>
    CPP_concept_bool borrowed_range =
        range<T> && detail::_borrowed_range<T>;

    template <typename R>
    RANGES_DEPRECATED("Please use ranges::borrowed_range instead.")
    RANGES_INLINE_VAR constexpr bool safe_range = borrowed_range<R>;

    template<typename T, typename V>
    CPP_concept_fragment(output_range_, requires()(0) &&
        output_iterator<iterator_t<T>, V>
    );
    template<typename T, typename V>
    CPP_concept_bool output_range =
        range<T> && CPP_fragment(ranges::output_range_, T, V);

    template<typename T>
    CPP_concept_fragment(input_range_, requires()(0) &&
        input_iterator<iterator_t<T>>
    );
    template<typename T>
    CPP_concept_bool input_range =
        range<T> && CPP_fragment(ranges::input_range_, T);

    template<typename T>
    CPP_concept_fragment(forward_range_, requires()(0) &&
        forward_iterator<iterator_t<T>>
    );
    template<typename T>
    CPP_concept_bool forward_range =
        input_range<T> && CPP_fragment(ranges::forward_range_, T);

    template<typename T>
    CPP_concept_fragment(bidirectional_range_, requires()(0) &&
        bidirectional_iterator<iterator_t<T>>
    );
    template<typename T>
    CPP_concept_bool bidirectional_range =
        forward_range<T> && CPP_fragment(ranges::bidirectional_range_, T);

    template<typename T>
    CPP_concept_fragment(random_access_range_, requires()(0) &&
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
    CPP_concept_fragment(contiguous_range_, requires()(0) &&
        contiguous_iterator<iterator_t<T>> &&
        same_as<detail::data_t<T>, std::add_pointer_t<iter_reference_t<iterator_t<T>>>>
    );

    template<typename T>
    CPP_concept_bool contiguous_range =
        random_access_range<T> && CPP_fragment(ranges::contiguous_range_, T);

    template<typename T>
    CPP_concept_fragment(common_range_, requires()(0) &&
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
    CPP_concept_fragment(sized_range_,
        requires(T & t) //
        (
            ranges::size(t)
        ) &&
        detail::integer_like_<range_size_t<T>>
    );

    template<typename T>
    CPP_concept_bool sized_range =
        range<T> &&
        !disable_sized_range<uncvref_t<T>> &&
        CPP_fragment(ranges::sized_range_, T);
    // clang-format on

    /// \cond
    namespace ext
    {
        template<typename T>
        struct enable_view
          : std::is_base_of<view_base, T>
        {};
    } // namespace detail
    /// \endcond

    // Specialize this if the default is wrong.
    template<typename T>
    RANGES_INLINE_VAR constexpr bool enable_view =
        ext::enable_view<T>::value;

#if defined(__cpp_lib_string_view) && __cpp_lib_string_view > 0
    template<typename Char, typename Traits>
    RANGES_INLINE_VAR constexpr bool enable_view<std::basic_string_view<Char, Traits>> =
        true;
#endif

#if defined(__cpp_lib_span) && __cpp_lib_span > 0
    template<typename T, std::size_t N>
    RANGES_INLINE_VAR constexpr bool enable_view<std::span<T, N>> = N + 1 < 2;
#endif

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
        (borrowed_range<T> || view_<uncvref_t<T>>);
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
        CPP_concept borrowed_range = CPP_defer(ranges::borrowed_range, T);

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
        using ranges::borrowed_range;
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
        using ranges::view_base;
    } // namespace cpp20
    /// @}
} // namespace ranges

#include <range/v3/detail/reenable_warnings.hpp>

#endif
