/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_ZIP_WITH_HPP
#define RANGES_V3_VIEW_ZIP_WITH_HPP

#include <tuple>
#include <limits>
#include <utility>
#include <functional>
#include <type_traits>
#include <meta/meta.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/all.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            constexpr struct
            {
                template<typename T, typename U>
                RANGES_RELAXED_CONSTEXPR
                bool operator()(T const &t, U const &u) const
                {
                    return static_cast<bool>(t == u);
                }
            } equal_to {};

            constexpr struct
            {
                template<typename T>
                RANGES_RELAXED_CONSTEXPR
                void operator()(T & t) const
                {
                    --t;
                }
            } dec {};

            constexpr struct
            {
                template<typename T>
                RANGES_RELAXED_CONSTEXPR
                void operator()(T & t) const
                {
                    ++t;
                }
            } inc {};

            constexpr struct
            {
                template<typename T, typename D>
                RANGES_RELAXED_CONSTEXPR
                void operator()(T & t, D d) const
                {
                    advance(t, d);
                }
            } advance_ {};

            constexpr struct
            {
                template<typename T>
                RANGES_RELAXED_CONSTEXPR
                auto operator()(T const &t, T const &u) const ->
                    decltype(u - t)
                {
                    return u - t;
                }
            } distance_to {};

            constexpr struct
            {
                template<typename T, typename U>
                RANGES_RELAXED_CONSTEXPR
                auto operator()(T const &t, U const &u) const ->
                    decltype(true ? t : u)
                {
                    return t < u ? t : u;
                }
            } min_ {};

            constexpr struct
            {
                template<typename T, typename U>
                RANGES_RELAXED_CONSTEXPR
                auto operator()(T const &t, U const &u) const ->
                    decltype(true ? u : t)
                {
                    return t < u ? u : t;
                }
            } max_ {};
        } // namespace detail
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Fun, typename...Rngs>
        struct iter_zip_with_view
          : range_facade<iter_zip_with_view<Fun, Rngs...>, meta::and_<is_infinite<Rngs>...>::value>
        {
        private:
            friend range_access;
            semiregular_t<function_type<Fun>> fun_;
            std::tuple<Rngs...> rngs_;
            using difference_type_ = common_type_t<range_difference_t<Rngs>...>;
            using size_type_ = meta::eval<std::make_unsigned<difference_type_>>;

            struct sentinel;
            struct cursor
            {
            private:
                friend sentinel;
                using fun_ref_ = semiregular_ref_or_val_t<function_type<Fun>, true>;
                fun_ref_ fun_;
                std::tuple<range_iterator_t<Rngs>...> its_;

                template<std::size_t...Is>
                RANGES_RELAXED_CONSTEXPR
                auto indirect_move_(meta::index_sequence<Is...>) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fun_(move_tag{}, std::get<Is>(its_)...)
                )
                template<typename Sent>
                RANGES_RELAXED_CONSTEXPR
                friend auto indirect_move(basic_iterator<cursor, Sent> const &it)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    get_cursor(it).indirect_move_(meta::make_index_sequence<sizeof...(Rngs)>{})
                )
            public:
                using difference_type =
                    common_type_t<range_difference_t<Rngs>...>;
                using single_pass =
                    meta::or_c<(bool) Derived<ranges::input_iterator_tag,
                        range_category_t<Rngs>>()...>;
                using value_type =
                    detail::decay_t<decltype(fun_(copy_tag{}, range_iterator_t<Rngs>{}...))>;

                RANGES_RELAXED_CONSTEXPR cursor() = default;
                RANGES_RELAXED_CONSTEXPR cursor(fun_ref_ fun, std::tuple<range_iterator_t<Rngs>...> its)
                  : fun_(std::move(fun)), its_(std::move(its))
                {}
                RANGES_RELAXED_CONSTEXPR auto current() const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    tuple_apply(fun_, its_)
                )
                RANGES_RELAXED_CONSTEXPR void next()
                {
                    tuple_for_each(its_, detail::inc);
                }
                RANGES_RELAXED_CONSTEXPR bool equal(cursor const &that) const
                {
                    // By returning true if *any* of the iterators are equal, we allow
                    // zipped ranges to be of different lengths, stopping when the first
                    // one reaches the end.
                    return tuple_foldl(
                        tuple_transform(its_, that.its_, detail::equal_to),
                        false,
                        [](bool a, bool b) { return a || b; });
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) BidirectionalIterable<Rngs>()...>::value)
                RANGES_RELAXED_CONSTEXPR void prev()
                {
                    tuple_for_each(its_, detail::dec);
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessIterable<Rngs>()...>::value)
                RANGES_RELAXED_CONSTEXPR void advance(difference_type n)
                {
                    tuple_for_each(its_, binder_1<decltype(detail::advance_), decltype(n)>(detail::advance_, n));
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessIterable<Rngs>()...>::value)
                RANGES_RELAXED_CONSTEXPR difference_type distance_to(cursor const &that) const
                {
                    // Return the smallest distance (in magnitude) of any of the iterator
                    // pairs. This is to accommodate zippers of sequences of different length.
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
                std::tuple<range_sentinel_t<Rngs>...> ends_;
            public:
                RANGES_RELAXED_CONSTEXPR sentinel() = default;
                RANGES_RELAXED_CONSTEXPR sentinel(detail::any, std::tuple<range_sentinel_t<Rngs>...> ends)
                sentinel(detail::any, std::tuple<range_sentinel_t<Rngs>...> ends)
                  : ends_(std::move(ends))
                {}
                RANGES_RELAXED_CONSTEXPR bool equal(cursor const &pos) const
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

            using are_bounded_t = meta::and_c<(bool) BoundedIterable<Rngs>()...>;

            RANGES_RELAXED_CONSTEXPR cursor begin_cursor()
            {
                return {fun_, tuple_transform(rngs_, begin)};
            }
            RANGES_RELAXED_CONSTEXPR meta::if_<are_bounded_t, cursor, sentinel> end_cursor()
            {
                return {fun_, tuple_transform(rngs_, end)};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool) Iterable<Rngs const>()...>::value)
            RANGES_RELAXED_CONSTEXPR cursor begin_cursor() const
            {
                return {fun_, tuple_transform(rngs_, begin)};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool) Iterable<Rngs const>()...>::value)
            RANGES_RELAXED_CONSTEXPR meta::if_<are_bounded_t, cursor, sentinel> end_cursor() const
            {
                return {fun_, tuple_transform(rngs_, end)};
            }
        public:
            RANGES_RELAXED_CONSTEXPR iter_zip_with_view() = default;
            RANGES_RELAXED_CONSTEXPR explicit iter_zip_with_view(Rngs ...rngs)
              : fun_(as_function(Fun{}))
              , rngs_{std::move(rngs)...}
            {}
            RANGES_RELAXED_CONSTEXPR explicit iter_zip_with_view(Fun fun, Rngs ...rngs)
              : fun_(as_function(std::move(fun)))
              , rngs_{std::move(rngs)...}
            {}
            CONCEPT_REQUIRES(meta::and_c<(bool) SizedIterable<Rngs>()...>::value)
            RANGES_RELAXED_CONSTEXPR size_type_ size() const
            {
                return tuple_foldl(
                    tuple_transform(rngs_, ranges::size),
                    (std::numeric_limits<size_type_>::max)(),
                    detail::min_);
            }
        };

        template<typename Fun, typename...Rngs>
        struct zip_with_view
          : iter_zip_with_view<indirected<Fun>, Rngs...>
        {
            RANGES_RELAXED_CONSTEXPR zip_with_view() = default;
            RANGES_RELAXED_CONSTEXPR explicit zip_with_view(Rngs ...rngs)
              : iter_zip_with_view<indirected<Fun>, Rngs...>{
                  {Fun{}}, std::move(rngs)...}
            {}
            RANGES_RELAXED_CONSTEXPR explicit zip_with_view(Fun fun, Rngs ...rngs)
              : iter_zip_with_view<indirected<Fun>, Rngs...>{
                  {std::move(fun)}, std::move(rngs)...}
            {}
        };

        namespace view
        {
            struct iter_zip_with_fn
            {
                template<typename Fun, typename ...Rngs>
                using Concept = meta::and_<
                    InputIterable<Rngs>...,
                    Callable<Fun, range_iterator_t<Rngs>...>,
                    Callable<Fun, copy_tag, range_iterator_t<Rngs>...>,
                    Callable<Fun, move_tag, range_iterator_t<Rngs>...>>;

                template<typename...Rngs, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Fun, Rngs...>())>
                RANGES_RELAXED_CONSTEXPR
                iter_zip_with_view<Fun, all_t<Rngs>...> operator()(Fun fun, Rngs &&... rngs) const
                {
                    return iter_zip_with_view<Fun, all_t<Rngs>...>{
                        std::move(fun),
                        all(std::forward<Rngs>(rngs))...
                    };
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Fun, typename...Rngs,
                    CONCEPT_REQUIRES_(!Concept<Fun, Rngs...>())>
                RANGES_RELAXED_CONSTEXPR
                void operator()(Fun, Rngs &&...) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputIterable<Rngs>...>(),
                        "All of the objects passed to view::iter_zip_with must model the InputIterable "
                        "concept");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, range_iterator_t<Rngs>...>(),
                        "The function passed to view::iter_zip_with must be callable with arguments "
                        "of the ranges' iterator types.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, copy_tag, range_iterator_t<Rngs>...>(),
                        "The function passed to view::iter_zip_with must be callable with "
                        "copy_tag and arguments of the ranges' iterator types.");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, move_tag, range_iterator_t<Rngs>...>(),
                        "The function passed to view::iter_zip_with must be callable with "
                        "move_tag and arguments of the ranges' iterator types.");
                }
            #endif
            };

            /// \relates iter_zip_with_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& iter_zip_with = static_const<iter_zip_with_fn>::value;
            }

            struct zip_with_fn
            {
                template<typename Fun, typename ...Rngs>
                using Concept = meta::and_<
                    InputIterable<Rngs>...,
                    Callable<Fun, range_reference_t<Rngs> &&...>>;

                template<typename...Rngs, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Fun, Rngs...>())>
                RANGES_RELAXED_CONSTEXPR
                zip_with_view<Fun, all_t<Rngs>...> operator()(Fun fun, Rngs &&... rngs) const
                {
                    return zip_with_view<Fun, all_t<Rngs>...>{
                        std::move(fun),
                        all(std::forward<Rngs>(rngs))...
                    };
                }

            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Fun, typename...Rngs,
                    CONCEPT_REQUIRES_(!Concept<Fun, Rngs...>())>
                RANGES_RELAXED_CONSTEXPR
                void operator()(Fun, Rngs &&...) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputIterable<Rngs>...>(),
                        "All of the objects passed to view::zip_with must model the InputIterable "
                        "concept");
                    CONCEPT_ASSERT_MSG(
                        Callable<Fun, range_reference_t<Rngs> &&...>(),
                        "The function passed to view::zip_with must be callable with arguments "
                        "of the ranges' reference types.");
                }
            #endif
            };

            /// \relates zip_with_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& zip_with = static_const<zip_with_fn>::value;
            }
        }
        /// @}
    }
}

#endif
