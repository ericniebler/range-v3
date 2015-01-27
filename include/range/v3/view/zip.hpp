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
#include <range/v3/range_adaptor.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/move.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/optional.hpp>
#include <range/v3/utility/common_type.hpp>
#include <range/v3/utility/common_tuple.hpp>
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
                bool operator()(T const &t, U const &u) const
                {
                    return static_cast<bool>(t == u);
                }
            } equal_to {};

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

            struct make_tuple_like_fn
              : make_common_pair_fn
              , make_common_tuple_fn
            {
                using expects_wrapped_references = void;
                using make_common_pair_fn::operator();
                using make_common_tuple_fn::operator();
            };

            template<typename T>
            struct as_std_tuple_like
            {};

            template<typename ...Ts>
            struct as_std_tuple_like<common_tuple<Ts...>>
            {
                using type = std::tuple<Ts...>;
            };

            template<typename First, typename Second>
            struct as_std_tuple_like<common_pair<First, Second>>
            {
                using type = std::pair<First, Second>;
            };
        } // namespace detail
        /// \endcond

        /// \addtogroup group-views
        /// @{
        template<typename Fun, typename...Rngs>
        struct zip_with_view
          : range_facade<zip_with_view<Fun, Rngs...>, meta::and_<is_infinite<Rngs>...>::value>
        {
        private:
            friend range_access;
        friend zip_view<Rngs...>;
            semiregular_invokable_t<unwrap_args_t<Fun>> fun_;
            std::tuple<view::all_t<Rngs>...> rngs_;
            using difference_type_ = common_type_t<range_difference_t<Rngs>...>;
            using size_type_ = meta::eval<std::make_unsigned<difference_type_>>;

            struct sentinel;
            struct cursor
            {
            private:
                friend struct sentinel;
                semiregular_invokable_ref_t<unwrap_args_t<Fun>, true> fun_;
                std::tuple<range_iterator_t<Rngs>...> its_;

                using reference_t_ =
                    concepts::Invokable::result_t<
                        unwrap_args_t<Fun>,
                        forward_ref_t<range_reference_t<Rngs>>...>;
                using rvalue_reference_t_ =
                    concepts::Invokable::result_t<
                        unwrap_args_t<Fun>,
                        forward_ref_t<range_rvalue_reference_t<Rngs>>...>;
            public:
                using difference_type = common_type_t<range_difference_t<Rngs>...>;
                using single_pass =
                    meta::or_c<(bool) Derived<ranges::input_iterator_tag,
                        range_category_t<Rngs>>()...>;
                using value_type =
                    detail::decay_t<
                        concepts::Invokable::result_t<
                            Fun,
                            range_reference_t<Rngs>...>>;
                // This is what gives zip_view iterators their special iter_move behavior:
                template<std::size_t...Is>
                rvalue_reference_t_ indirect_move_(index_sequence<Is...>) const
                {
                    return fun_(ranges::forward_ref<range_rvalue_reference_t<Rngs>>(
                        indirect_move(std::get<Is>(its_)))...);
                }
                struct mixin : basic_mixin<cursor>
                {
                    mixin() = default;
                    using basic_mixin<cursor>::basic_mixin;
                    template<typename Sent>
                    friend rvalue_reference_t_ indirect_move(basic_iterator<cursor, Sent> const &it)
                        noexcept(noexcept(
                            std::declval<invokable_t<unwrap_args_t<Fun>> const &>()(
                                std::declval<forward_ref_t<range_rvalue_reference_t<Rngs>>>()...)))
                    {
                        // http://llvm.org/bugs/show_bug.cgi?id=21109
                        //return it.mixin::get().indirect_move_(make_index_sequence<sizeof...(Rngs)>{});
                        mixin const &mix = it;
                        return mix.get().indirect_move_(make_index_sequence<sizeof...(Rngs)>{});
                    }
                };
                cursor() = default;
                cursor(semiregular_invokable_ref_t<unwrap_args_t<Fun>, true> fun,
                    std::tuple<range_iterator_t<Rngs>...> its)
                  : fun_(std::move(fun)), its_(std::move(its))
                {}
                template<std::size_t...Is>
                reference_t_ current_(index_sequence<Is...>) const
                {
                    return fun_(ranges::forward_ref<range_reference_t<Rngs>>(*std::get<Is>(its_))...);
                }
                reference_t_ current() const
                    noexcept(noexcept(
                        fun_(std::declval<forward_ref_t<range_reference_t<Rngs>>>()...)))
                {
                    return this->current_(make_index_sequence<sizeof...(Rngs)>{});
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
                CONCEPT_REQUIRES(meta::and_c<(bool) BidirectionalIterable<Rngs>()...>::value)
                void prev()
                {
                    tuple_for_each(its_, detail::dec);
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessIterable<Rngs>()...>::value)
                void advance(difference_type n)
                {
                    using std::placeholders::_1;
                    tuple_for_each(its_, std::bind(detail::advance_, _1, n));
                }
                CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessIterable<Rngs>()...>::value)
                difference_type distance_to(cursor const &that) const
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
                sentinel() = default;
                sentinel(
                    semiregular_invokable_ref_t<unwrap_args_t<Fun>, true> const &,
                    std::tuple<range_sentinel_t<Rngs>...> ends)
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

            using are_bounded_t = meta::and_c<(bool) BoundedIterable<Rngs>()...>;

            cursor begin_cursor()
            {
                return {fun_, tuple_transform(rngs_, begin)};
            }
            meta::if_<are_bounded_t, cursor, sentinel> end_cursor()
            {
                return {fun_, tuple_transform(rngs_, end)};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool) Iterable<Rngs const>()...>::value)
            cursor begin_cursor() const
            {
                return {fun_, tuple_transform(rngs_, begin)};
            }
            CONCEPT_REQUIRES(meta::and_c<(bool) Iterable<Rngs const>()...>::value)
            meta::if_<are_bounded_t, cursor, sentinel> end_cursor() const
            {
                return {fun_, tuple_transform(rngs_, end)};
            }
        public:
            zip_with_view() = default;
            explicit zip_with_view(Rngs &&...rngs)
              : fun_(invokable(make_unwrap_args(Fun{})))
              , rngs_{view::all(std::forward<Rngs>(rngs))...}
            {}
            explicit zip_with_view(Fun fun, Rngs &&...rngs)
              : fun_(invokable(make_unwrap_args(std::move(fun))))
              , rngs_{view::all(std::forward<Rngs>(rngs))...}
            {}
            CONCEPT_REQUIRES(meta::and_c<(bool) SizedIterable<Rngs>()...>::value)
            size_type_ size() const
            {
                return tuple_foldl(
                    tuple_transform(rngs_, ranges::size),
                    (std::numeric_limits<size_type_>::max)(),
                    detail::min_);
            }
        };

        // The zip_view is an adapted zip_with_view using a function that makes common_tuples
        // (or common_pairs). In addition, it changes the value type and the rvalue reference
        // type to be std::tuple (or std::pair).
        template<typename...Rngs>
        struct zip_view
          : range_adaptor<zip_view<Rngs...>, zip_with_view<detail::make_tuple_like_fn, Rngs...>>
        {
        private:
            friend range_access;
            using base_range_t = zip_with_view<detail::make_tuple_like_fn, Rngs...>;
            struct adaptor : adaptor_base
            {
                using value_type =
                    meta::eval<detail::as_std_tuple_like<range_value_t<base_range_t>>>;
                using rvalue_reference_ =
                    meta::eval<detail::as_std_tuple_like<range_rvalue_reference_t<base_range_t>>>;
                // BUGBUG This isn't ideal. Should fix the problems with common_pair and
                // common_tuple instead.
                rvalue_reference_ indirect_move(range_iterator_t<base_range_t> const &it) const
                    noexcept(noexcept(rvalue_reference_(ranges::indirect_move(it))))
                {
                    return ranges::indirect_move(it);
                }
            };
            adaptor begin_adaptor() const
            {
                return {};
            }
            adaptor end_adaptor() const
            {
                return {};
            }
        public:
            zip_view() = default;
            zip_view(Rngs &&...rngs)
              : range_adaptor_t<zip_view>{base_range_t{std::forward<Rngs>(rngs)...}}
            {}
        };

        namespace view
        {
            struct zip_fn
            {
                template<typename ...Rngs>
                using Concept = meta::and_<InputIterable<Rngs>...>;

                template<typename...Rngs,
                    CONCEPT_REQUIRES_(Concept<Rngs...>())>
                zip_view<Rngs...> operator()(Rngs &&... rngs) const
                {
                    CONCEPT_ASSERT(meta::and_c<(bool) Iterable<Rngs>()...>::value);
                    return zip_view<Rngs...>{std::forward<Rngs>(rngs)...};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename...Rngs,
                    CONCEPT_REQUIRES_(!Concept<Rngs...>())>
                void operator()(Rngs &&... rngs) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputIterable<Rngs>...>(),
                        "All of the objects passed to view::zip must model the InputIterable "
                        "concept");
                }
            #endif
            };

            /// \relates zip_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& zip = static_const<zip_fn>::value;
            }

            struct zip_with_fn
            {
                template<typename Fun, typename ...Rngs>
                struct Concept
                  : meta::fast_and<
                        InputIterable<Rngs>...,
                        Invokable<unwrap_args_t<Fun>, forward_ref_t<range_reference_t<Rngs>>...>,
                        Invokable<unwrap_args_t<Fun>, forward_ref_t<range_rvalue_reference_t<Rngs>>...>,
                        Invokable<Fun, range_reference_t<Rngs>...>>
                {};

                template<typename...Rngs, typename Fun,
                    CONCEPT_REQUIRES_(Concept<Fun, Rngs...>())>
                zip_with_view<Fun, Rngs...> operator()(Fun fun, Rngs &&... rngs) const
                {
                    return zip_with_view<Fun, Rngs...>{
                        std::move(fun),
                        std::forward<Rngs>(rngs)...
                    };
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Fun, typename...Rngs,
                    CONCEPT_REQUIRES_(!Concept<Fun, Rngs...>())>
                void operator()(Fun, Rngs &&...) const
                {
                    CONCEPT_ASSERT_MSG(meta::and_<InputIterable<Rngs>...>(),
                        "All of the objects passed to view::zip_with must model the InputIterable "
                        "concept");
                    CONCEPT_ASSERT_MSG(
                        Invokable<unwrap_args_t<Fun>, forward_ref_t<range_reference_t<Rngs>>...>() &&
                            Invokable<Fun, range_reference_t<Rngs>...>(),
                        "The function passed to view::zip_with must be callable with N arguments "
                        "of each of the N ranges' reference types.");
                    CONCEPT_ASSERT_MSG(
                        Invokable<unwrap_args_t<Fun>, forward_ref_t<range_rvalue_reference_t<Rngs>>...>(),
                        "The function passed to view::zip_with must be callable with N arguments "
                        "of each of the N ranges' rvalue reference types.");
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
