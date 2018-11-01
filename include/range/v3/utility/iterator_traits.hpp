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

#ifndef RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP
#define RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP

#include <type_traits>
#include <iterator>
#include <meta/meta.hpp>
#include <concepts/concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/associated_types.hpp>
#include <range/v3/utility/move.hpp> // for iter_move
#include <range/v3/utility/common_type.hpp> // for iter_move

namespace ranges
{
    inline namespace v3
    {
        struct InputIteratorConcept;
        struct ForwardIteratorConcept;
        struct BidirectionalIteratorConcept;
        struct RandomAccessIteratorConcept;
        struct ContiguousIteratorConcept;

        /// \addtogroup group-core
        /// @{
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
        /// @}

        /// \cond
        namespace detail
        {
            template<typename I,
                typename = iter_reference_t<I>,
                typename R = decltype(iter_move(std::declval<I &>())),
                typename = R&>
            using rvalue_reference_t = R;

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T>
            T upgrade_iterator_category_(T *, void *);

            template<typename T>
            ranges::input_iterator_tag upgrade_iterator_category_(T *, std::input_iterator_tag *);

            template<typename T>
            ranges::forward_iterator_tag upgrade_iterator_category_(T *, std::forward_iterator_tag *);

            template<typename T>
            ranges::bidirectional_iterator_tag upgrade_iterator_category_(T *, std::bidirectional_iterator_tag *);

            template<typename T>
            ranges::random_access_iterator_tag upgrade_iterator_category_(T *, std::random_access_iterator_tag *);

            template<typename T>
            struct upgrade_iterator_category
            {
                using type = decltype(detail::upgrade_iterator_category_(_nullptr_v<T>(), _nullptr_v<T>()));
            };

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T, typename B>
            meta::nil_ downgrade_iterator_category_(T *, void *, B);

            template<typename T, typename B>
            meta::id<T>
            downgrade_iterator_category_(T *, std::input_iterator_tag *, B);

            template<typename T, typename B>
            meta::id<T>
            downgrade_iterator_category_(T *, std::output_iterator_tag *, B);

            template<typename T, typename B>
            meta::id<std::input_iterator_tag>
            downgrade_iterator_category_(T *, ranges::input_iterator_tag *, B);

            template<typename T>
            meta::id<std::forward_iterator_tag>
            downgrade_iterator_category_(T *, ranges::forward_iterator_tag *, std::true_type);

            template<typename T>
            meta::id<std::bidirectional_iterator_tag>
            downgrade_iterator_category_(T *, ranges::bidirectional_iterator_tag *, std::true_type);

            template<typename T>
            meta::id<std::random_access_iterator_tag>
            downgrade_iterator_category_(T *, ranges::random_access_iterator_tag *, std::true_type);

            template<typename Tag, typename Reference>
            struct downgrade_iterator_category
              : decltype(detail::downgrade_iterator_category_(_nullptr_v<Tag>(), _nullptr_v<Tag>(),
                    std::integral_constant<bool, std::is_reference<Reference>::value>()))
            {};

            ////////////////////////////////////////////////////////////////////////////////////////
            template<typename T>
            meta::if_<std::is_object<T>, ranges::contiguous_iterator_tag>
            iterator_category_helper(T **);

            template<typename T>
            meta::_t<upgrade_iterator_category<typename T::iterator_category>>
            iterator_category_helper(T *);

            template<typename T>
            using iterator_category_ = decltype(detail::iterator_category_helper(_nullptr_v<T>()));

            template<typename I>
            struct has_nothrow_iter_move
              : meta::bool_<noexcept(rvalue_reference_t<I>(ranges::iter_move(std::declval<I &>())))>
            {};
        } // namespace detail
        /// \endcond

        /// \addtogroup group-concepts
        /// @{
        template<typename T>
        struct iterator_category
          : meta::defer<detail::iterator_category_, T>
        {};

        template<typename T>
        struct iterator_category<T const>
          : iterator_category<T>
        {};
        /// @}

        /// \addtogroup group-core
        /// @{
        ////////////////////////////////////////////////////////////////////////////////////////////
        // iterator traits
        // template<typename I>
        // using iter_value_t = meta::_t<value_type<I>>;

        template<typename I>
        using rvalue_reference_t = detail::rvalue_reference_t<I>;

        template<typename I>
        using iter_common_reference_t = common_reference_t<iter_reference_t<I>, iter_value_t<I> &>;

        template<typename I>
        using iterator_category_t = meta::_t<iterator_category<I>>;

        // template<typename I>
        // using iter_difference_t = meta::_t<difference_type<I>>;

        template<typename I>
        using size_type_t = meta::_t<std::make_unsigned<iter_difference_t<I>>>;
        /// @}

        /// \cond
        namespace detail
        {
            template<typename I>
            using arrow_type_ = decltype(std::declval<I &>().operator->());

            template<typename I>
            struct pointer_type_
              : meta::if_<
                    meta::is_trait<meta::defer<arrow_type_, I>>,
                    meta::defer<arrow_type_, I>,
                    std::add_pointer<iter_reference_t<I>>>
            {};
        }
        /// \endcond
    }
}

#endif // RANGES_V3_UTILITY_ITERATOR_TRAITS_HPP
