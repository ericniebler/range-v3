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

#ifndef RANGES_V3_ITERATOR_TRAITS_HPP
#define RANGES_V3_ITERATOR_TRAITS_HPP

#include <type_traits>
#include <iterator>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/move.hpp> // for iter_move
#include <range/v3/utility/common_type.hpp> // for iter_move
#include <range/v3/iterator/associated_types.hpp>

namespace ranges
{
    struct InputIteratorConcept;
    struct ForwardIteratorConcept;
    struct BidirectionalIteratorConcept;
    struct RandomAccessIteratorConcept;
    struct ContiguousIteratorConcept;

    /// \addtogroup group-core
    /// @{
    using input_iterator_tag = std::input_iterator_tag;
    using forward_iterator_tag = std::forward_iterator_tag;
    using bidirectional_iterator_tag = std::bidirectional_iterator_tag;
    using random_access_iterator_tag = std::random_access_iterator_tag;
    struct contiguous_iterator_tag
      : random_access_iterator_tag
    {};
    /// @}

    /// \cond
    namespace detail
    {
        using input_iterator_tag =
            ::concepts::tag<InputIteratorConcept>;

        using forward_iterator_tag =
            ::concepts::tag<ForwardIteratorConcept, input_iterator_tag>;

        using bidirectional_iterator_tag =
            ::concepts::tag<BidirectionalIteratorConcept, forward_iterator_tag>;

        using random_access_iterator_tag =
            ::concepts::tag<RandomAccessIteratorConcept, bidirectional_iterator_tag>;

        using contiguous_iterator_tag =
            ::concepts::tag<ContiguousIteratorConcept, random_access_iterator_tag>;

        template<typename I,
            typename = iter_reference_t<I>,
            typename R = decltype(iter_move(std::declval<I &>())),
            typename = R&>
        using iter_rvalue_reference_t = R;

        template<typename I>
        constexpr bool has_nothrow_iter_move_v =
            noexcept(iter_rvalue_reference_t<I>(ranges::iter_move(std::declval<I &>())));
    } // namespace detail
    /// \endcond

    /// \addtogroup group-core
    /// @{
    ////////////////////////////////////////////////////////////////////////////////////////////
    // iterator traits
    template<typename I>
    using iter_rvalue_reference_t = detail::iter_rvalue_reference_t<I>;

    template<typename I>
    using iter_common_reference_t =
        common_reference_t<iter_reference_t<I>, iter_value_t<I> &>;
    /// @}

    /// \cond
    template<typename I>
    using size_type_t
        RANGES_DEPRECATED("size_type_t is deprecated.") =
            meta::_t<std::make_unsigned<iter_difference_t<I>>>;

    template<typename I>
    using rvalue_reference_t
        RANGES_DEPRECATED("rvalue_reference_t is deprecated; "
                          "use iter_rvalue_reference_t instead") =
            iter_rvalue_reference_t<I>;
    /// \endcond

    /// \cond
    namespace detail
    {
        template<typename I>
        using iter_arrow_t = decltype(std::declval<I &>().operator->());

        template<typename I>
        using iter_pointer_t = meta::_t<if_then_t<
            meta::is_trait<meta::defer<iter_arrow_t, I>>::value,
            meta::defer<iter_arrow_t, I>,
            std::add_pointer<iter_reference_t<I>>>>;
    }
    /// \endcond
}

#endif // RANGES_V3_ITERATOR_TRAITS_HPP
