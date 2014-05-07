// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008.
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef RANGES_V3_VIEW_ZIP_HPP
#define RANGES_V3_VIEW_ZIP_HPP

#include <tuple>
#include <limits>
#include <utility>
#include <functional>
#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            constexpr struct
            {
                template<typename T, typename U>
                bool operator()(T const &t, U const &u) const
                {
                    return static_cast<bool>(t == u);
                }
            } equal_to {};

            constexpr struct
            {
                template<typename T>
                auto operator()(T const & t) const -> decltype(*t)
                {
                    return *t;
                }
            } deref {};

            constexpr struct
            {
                template<typename T>
                void operator()(T & t) const
                {
                    --t;
                }
            } dec {};

            constexpr struct
            {
                template<typename T>
                void operator()(T & t) const
                {
                    ++t;
                }
            } inc {};

            constexpr struct
            {
                template<typename T, typename D>
                void operator()(T & t, D d) const
                {
                    ranges::advance(t, d);
                }
            } advance {};

            constexpr struct
            {
                template<typename T>
                auto operator()(T const &t, T const &u) const ->
                    decltype(u - t)
                {
                    return u - t;
                }
            } distance_to {};

            constexpr struct
            {
                template<typename T, typename U>
                auto operator()(T const &t, U const &u) const ->
                    decltype(true ? t : u)
                {
                    return t < u ? t : u;
                }
            } min_ {};

            constexpr struct
            {
                template<typename T, typename U>
                auto operator()(T const &t, U const &u) const ->
                    decltype(true ? u : t)
                {
                    return t < u ? u : t;
                }
            } max_ {};
        } // namespace detail

        template<typename ...Rngs>
        struct zipped_view
          : range_facade<
                zipped_view<Rngs...>,
                logical_and<is_infinite<Rngs>::value...>::value>
        {
        private:
            friend range_core_access;
            std::tuple<Rngs...> rngs_;
            using difference_type = common_type_t<range_difference_t<Rngs>...>;
            using size_type = meta_apply<std::make_unsigned, difference_type>;

            struct sentinel;
            struct cursor
            {
            private:
                friend struct sentinel;
                std::tuple<range_iterator_t<Rngs>...> its_;
            public:
                using difference_type = common_type_t<range_difference_t<Rngs>...>;
                using single_pass =
                    logical_or<(bool) ranges::Derived<ranges::input_iterator_tag,
                        range_category_t<Rngs>>()...>;
                using value_type = std::tuple<range_value_t<Rngs>...>;
                cursor() = default;
                cursor(std::tuple<range_iterator_t<Rngs>...> its)
                  : its_(std::move(its))
                {}
                std::tuple<range_reference_t<Rngs>...> current() const
                {
                    return ranges::tuple_transform(its_, detail::deref);
                }
                void next()
                {
                    ranges::tuple_for_each(its_, detail::inc);
                }
                bool equal(cursor const &that) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return ranges::tuple_foldl(
                        ranges::tuple_transform(its_, that.its_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
                CONCEPT_REQUIRES(
                    logical_and<(bool) ranges::BidirectionalIterator<
                        range_iterator_t<Rngs>>()...>::value)
                void prev()
                {
                    ranges::tuple_for_each(its_, detail::dec);
                }
                CONCEPT_REQUIRES(
                    logical_and<(bool) ranges::RandomAccessIterator<
                        range_iterator_t<Rngs>>()...>::value)
                void advance(difference_type n)
                {
                    using std::placeholders::_1;
                    ranges::tuple_for_each(its_, std::bind(detail::advance, _1, n));
                }
                CONCEPT_REQUIRES(
                    logical_and<(bool) ranges::RandomAccessIterator<
                        range_iterator_t<Rngs>>()...>::value)
                difference_type distance_to(cursor const &that) const
                {
                    // Return the smallest distance (in magnitude) of any of the iterator
                    // pairs. This is to accomodate zippers of sequences of different length.
                    if(0 < std::get<0>(that.its_) - std::get<0>(its_))
                        return ranges::tuple_foldl(
                            ranges::tuple_transform(its_, that.its_, detail::distance_to),
                            (std::numeric_limits<difference_type>::max)(),
                            detail::min_);
                    else
                        return ranges::tuple_foldl(
                            ranges::tuple_transform(its_, that.its_, detail::distance_to),
                            (std::numeric_limits<difference_type>::min)(),
                            detail::max_);
                }
            };

            struct sentinel
            {
            private:
                std::tuple<range_sentinel_t<Rngs>...> ends_;
            public:
                sentinel() = default;
                sentinel(std::tuple<range_sentinel_t<Rngs>...> ends)
                  : ends_(std::move(ends))
                {}
                bool equal(cursor const &pos) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return ranges::tuple_foldl(
                        ranges::tuple_transform(pos.its_, ends_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
            };

            cursor begin_cursor() const
            {
                return {ranges::tuple_transform(rngs_, ranges::begin)};
            }
            detail::conditional_t<
                logical_and<(bool) ranges::Range<Rngs>()...>::value,
                cursor,
                sentinel>
            end_cursor() const
            {
                return {ranges::tuple_transform(rngs_, ranges::end)};
            }
        public:
            zipped_view() = default;
            explicit zipped_view(Rngs &&...rngs)
              : rngs_{std::forward<Rngs>(rngs)...}
            {}
            CONCEPT_REQUIRES(logical_and<(bool) ranges::SizedIterable<Rngs>()...>::value)
            size_type size() const
            {
                return ranges::tuple_foldl(
                    ranges::tuple_transform(rngs_, ranges::size),
                    (std::numeric_limits<size_type>::max)(),
                    detail::min_);
            }
        };

        namespace view
        {
            struct zipper : bindable<zipper>
            {
                template<typename...Rngs>
                static zipped_view<Rngs...> invoke(zipper, Rngs &&... rngs)
                {
                    CONCEPT_ASSERT(logical_and<(bool) ranges::Iterable<Rngs>()...>::value);
                    return zipped_view<Rngs...>{std::forward<Rngs>(rngs)...};
                }
            };

            RANGES_CONSTEXPR zipper zip {};
        }
    }
}

#endif
