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
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/view/all.hpp>

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
                    advance(t, d);
                }
            } advance_ {};

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

        template<typename Fun, typename ...Rngs>
        struct zipped_with_view
          : range_facade<
                zipped_with_view<Fun, Rngs...>,
                logical_and<is_infinite<Rngs>::value...>::value>
        {
        private:
            friend range_core_access;
            optional<invokable_t<Fun>> fun_;
            std::tuple<view::all_t<Rngs>...> rngs_;
            using difference_type_ = common_type_t<range_difference_t<Rngs>...>;
            using size_type_ = meta_apply<std::make_unsigned, difference_type_>;

            struct sentinel;
            struct cursor
            {
            private:
                friend struct sentinel;
                invokable_t<Fun> const *fun_;
                std::tuple<range_iterator_t<view::all_t<Rngs>>...> its_;
            public:
                using difference_type = common_type_t<range_difference_t<Rngs>...>;
                using single_pass =
                    logical_or<(bool) Derived<ranges::input_iterator_tag,
                        range_category_t<Rngs>>()...>;
                using value_type =
                    detail::uncvref_t<result_of_t<invokable_t<Fun>(range_value_t<Rngs>...)>>;
                cursor() = default;
                cursor(invokable_t<Fun> const &fun, std::tuple<range_iterator_t<view::all_t<Rngs>>...> its)
                  : fun_(&fun), its_(std::move(its))
                {}
                auto current() const ->
                    decltype(tuple_apply(*fun_, tuple_transform(its_, detail::deref)))
                {
                    return tuple_apply(*fun_, tuple_transform(its_, detail::deref));
                }
                void next()
                {
                    tuple_for_each(its_, detail::inc);
                }
                bool equal(cursor const &that) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return tuple_foldl(
                        tuple_transform(its_, that.its_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
                CONCEPT_REQUIRES(logical_and<(bool) BidirectionalIterable<Rngs>()...>::value)
                void prev()
                {
                    tuple_for_each(its_, detail::dec);
                }
                CONCEPT_REQUIRES(logical_and<(bool) RandomAccessIterable<Rngs>()...>::value)
                void advance(difference_type n)
                {
                    using std::placeholders::_1;
                    tuple_for_each(its_, std::bind(detail::advance_, _1, n));
                }
                CONCEPT_REQUIRES(logical_and<(bool) RandomAccessIterable<Rngs>()...>::value)
                difference_type distance_to(cursor const &that) const
                {
                    // Return the smallest distance (in magnitude) of any of the iterator
                    // pairs. This is to accomodate zippers of sequences of different length.
                    if(0 < std::get<0>(that.its_) - std::get<0>(its_))
                        return tuple_foldl(
                            tuple_transform(its_, that.its_, detail::distance_to),
                            (std::numeric_limits<difference_type>::max)(),
                            detail::min_);
                    else
                        return tuple_foldl(
                            tuple_transform(its_, that.its_, detail::distance_to),
                            (std::numeric_limits<difference_type>::min)(),
                            detail::max_);
                }
            };

            struct sentinel
            {
            private:
                std::tuple<range_sentinel_t<view::all_t<Rngs>>...> ends_;
            public:
                sentinel() = default;
                sentinel(invokable_t<Fun> const &, std::tuple<range_sentinel_t<view::all_t<Rngs>>...> ends)
                  : ends_(std::move(ends))
                {}
                bool equal(cursor const &pos) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return tuple_foldl(
                        tuple_transform(pos.its_, ends_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
            };

            using are_bounded_t = logical_and<(bool) BoundedIterable<Rngs>()...>;

            cursor begin_cursor()
            {
                return {*fun_, tuple_transform(rngs_, begin)};
            }
            detail::conditional_t<are_bounded_t::value, cursor, sentinel> end_cursor()
            {
                return {*fun_, tuple_transform(rngs_, end)};
            }
            CONCEPT_REQUIRES(logical_and<(bool) Iterable<Rngs const>()...>::value)
            cursor begin_cursor() const
            {
                return {*fun_, tuple_transform(rngs_, begin)};
            }
            CONCEPT_REQUIRES(logical_and<(bool) Iterable<Rngs const>()...>::value)
            detail::conditional_t<are_bounded_t::value, cursor, sentinel> end_cursor() const
            {
                return {*fun_, tuple_transform(rngs_, end)};
            }
        public:
            zipped_with_view() = default;
            explicit zipped_with_view(Fun fun, Rngs &&...rngs)
              : fun_{invokable(std::move(fun))}
              , rngs_{view::all(std::forward<Rngs>(rngs))...}
            {}
            CONCEPT_REQUIRES(logical_and<(bool) SizedIterable<Rngs>()...>::value)
            size_type_ size() const
            {
                return tuple_foldl(
                    tuple_transform(rngs_, ranges::size),
                    (std::numeric_limits<size_type_>::max)(),
                    detail::min_);
            }
        };

        template<typename ...Rngs>
        struct zipped_view
          : zipped_with_view<make_tuple_fn, Rngs...>
        {
            zipped_view() = default;
            zipped_view(Rngs &&...rngs)
              : zipped_with_view<make_tuple_fn, Rngs...>{make_tuple, std::forward<Rngs>(rngs)...}
            {}
        };

        namespace view
        {
            struct zip_fn : bindable<zip_fn>
            {
                template<typename...Rngs>
                static zipped_view<Rngs...> invoke(zip_fn, Rngs &&... rngs)
                {
                    CONCEPT_ASSERT(logical_and<(bool) Iterable<Rngs>()...>::value);
                    return zipped_view<Rngs...>{std::forward<Rngs>(rngs)...};
                }
            };

            RANGES_CONSTEXPR zip_fn zip {};

            struct zip_with_fn : bindable<zip_with_fn>
            {
                template<typename Fun, typename...Rngs>
                static zipped_with_view<Fun, Rngs...> invoke(zip_with_fn, Fun fun, Rngs &&... rngs)
                {
                    CONCEPT_ASSERT(logical_and<(bool) Iterable<Rngs>()...>::value);
                    CONCEPT_ASSERT(Invokable<Fun, range_value_t<Rngs>...>());
                    return zipped_with_view<Fun, Rngs...>{std::move(fun), std::forward<Rngs>(rngs)...};
                }
            };

            RANGES_CONSTEXPR zip_with_fn zip_with {};
        }
    }
}

#endif
