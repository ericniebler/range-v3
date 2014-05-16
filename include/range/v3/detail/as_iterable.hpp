//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

// Don't include this header directly
#ifndef RANGES_V3_RANGE_CONCEPTS_HPP
#error This file should only be included from range_concepts.hpp
#endif

        namespace detail
        {
            // An Iterable is container-like if the constness of its reference type
            // is sensitive to the constness of the Iterable
            template<typename T>
            using owns_its_elements_t = not_t<
                std::is_same<
                    decltype(*begin(std::declval<as_ref_t<T>>())),
                    decltype(*begin(std::declval<as_cref_t<T>>()))>>;

            template<typename I, typename Size,
                CONCEPT_REQUIRES_(RandomAccessIterator<I>())>
            iterator_range<I>
            container_view_all2(I begin, I end, Size size)
            {
                RANGES_ASSERT(size == static_cast<Size>(end - begin));
                return {begin, end};
            }

            template<typename I, typename S, typename Size>
            sized_iterator_range<I, S>
            container_view_all2(I begin, S end, Size size)
            {
                return {begin, end, size};
            }

            template<typename I, typename Size>
            iterator_range<counted_iterator<I>, counted_sentinel<I>>
            container_view_all2(counted_iterator<I> begin, counted_sentinel<I> end, Size size)
            {
                RANGES_ASSERT(size == static_cast<Size>(end.count() - begin.count()));
                return {begin, end};
            }

            template<typename I, typename Size>
            iterator_range<counted_iterator<I>, counted_iterator<I>>
            container_view_all2(counted_iterator<I> begin, counted_iterator<I> end, Size size)
            {
                RANGES_ASSERT(size == static_cast<Size>(end.count() - begin.count()));
                return {begin, end};
            }

            template<typename T>
            iterator_range<concepts::ConvertibleToIterable::iterator_t<T>,
                concepts::ConvertibleToIterable::sentinel_t<T>>
            container_view_all(T & t, concepts::ConvertibleToIterable*)
            {
                return {begin(t), end(t)};
            }

            template<typename T>
            auto container_view_all(T & t, concepts::ConvertibleToSizedIterable*) ->
                decltype(detail::container_view_all2(begin(t), end(t), 0))
            {
                return detail::container_view_all2(begin(t), end(t), size(t));
            }

            template<typename T>
            using convertible_to_sized_iterable_concept =
                concepts::most_refined<
                    typelist<concepts::ConvertibleToSizedIterable,
                             concepts::ConvertibleToIterable>, T>;

            template<typename T>
            using convertible_to_sized_iterable_concept_t =
                meta_apply<convertible_to_sized_iterable_concept, T>;

            template<typename T, typename C = convertible_to_sized_iterable_concept<T>>
            struct container_view_all_type
            {
                using type = decltype(detail::container_view_all(std::declval<T>(), C()));
            };

            template<typename T>
            using container_view_all_t = meta_apply<container_view_all_type, T>;
        }

        template<typename T, typename Enable = void>
        struct is_iterable
          : std::conditional<
                std::is_same<T, detail::uncvref_t<T>>::value,
                std::is_base_of<range_base, T>,
                is_iterable<detail::uncvref_t<T>>
            >::type
        {};

        template<typename T>
        struct is_iterable<std::initializer_list<T>>
          : std::true_type
        {};

        struct as_iterable_fn
        {
            /// If it's an iterable already, pass it though.
            template<typename T,
                CONCEPT_REQUIRES_(
                    ConvertibleToIterable<T>() &&
                    is_iterable<T>::value)>
            T operator()(T && t) const
            {
                return std::forward<T>(t);
            }

            /// If it is container-like, turn it into an iterator_range
            template<typename T,
                CONCEPT_REQUIRES_(
                    ConvertibleToIterable<T>() &&
                    !is_iterable<T>::value &&
                    detail::owns_its_elements_t<T>::value &&
                    std::is_lvalue_reference<T>::value)>
            detail::container_view_all_t<T> operator()(T && t) const
            {
                return detail::container_view_all(t, detail::convertible_to_sized_iterable_concept<T>());
            }

            // TODO handle char const * by turning it into a delimited range
        };

        RANGES_CONSTEXPR as_iterable_fn as_iterable {};

        template<typename ConvertibleToIterable>
        using as_iterable_t = decltype(as_iterable(std::declval<ConvertibleToIterable>()));
