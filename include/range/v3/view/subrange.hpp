/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_SUBRANGE_HPP
#define RANGES_V3_VIEW_SUBRANGE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <concepts/concepts.hpp>

#include <range/v3/iterator/operations.hpp>
#include <range/v3/iterator/unreachable_sentinel.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/dangling.hpp>
#include <range/v3/utility/get.hpp>
#include <range/v3/view/interface.hpp>

namespace ranges
{
    /// \addtogroup group-views
    /// @{
    enum class subrange_kind : bool
    {
        unsized,
        sized
    };

    /// \cond
    namespace detail
    {
        template<std::size_t N, typename T>
        using tuple_element_fun_t = void (*)(meta::_t<std::tuple_element<N, T>> const &);

        // clang-format off
        CPP_def
        (
            template(typename From, typename To)
            concept convertible_to_not_slicing_,
                convertible_to<From, To> &&
                // A conversion is a slicing conversion if the source and the destination
                // are both pointers, and if the pointed-to types differ after removing
                // cv qualifiers.
                (!(std::is_pointer<decay_t<From>>::value &&
                    std::is_pointer<decay_t<To>>::value &&
                    not_same_as_<meta::_t<std::remove_pointer<decay_t<From>>>,
                               meta::_t<std::remove_pointer<decay_t<To>>>>))
        );
        CPP_def
        (
            template(typename T)
            concept pair_like_gcc_bugs_3_,
                requires(T t, tuple_element_fun_t<0, T> p0, tuple_element_fun_t<1, T> p1)
                (
                    p0( get<0>(t) ),
                    p1( get<1>(t) )
                )
        );
        CPP_def
        (
            template(typename T)
            concept pair_like_gcc_bugs_2_,
                derived_from<std::tuple_size<T>, meta::size_t<2>> &&
                pair_like_gcc_bugs_3_<T>
        );
        CPP_def
        (
            template(typename T)
            concept pair_like_gcc_bugs_,
                ranges::defer::type<meta::_t<std::tuple_size<T>>> &&
                defer::pair_like_gcc_bugs_2_<T>
        );
        CPP_def
        (
            template(typename T)
            concept get_first_and_second_,
                requires(T &t)
                (
                    get<0>(t),
                    get<1>(t)
                )
        );
        CPP_def
        (
            template(typename T)
            concept pair_like_,
                (!std::is_reference<T>::value) &&
                (defer::get_first_and_second_<T> &&
                 defer::pair_like_gcc_bugs_<T>)
        );
        // clang-format on

        // Short-circuit the PairLike concept for things known to be pairs:
        template<typename T>
        struct pair_like : meta::bool_<pair_like_<T>>
        {};
        template<typename F, typename S>
        struct pair_like<std::pair<F, S>> : meta::bool_<true>
        {};
        template<typename... Ts>
        struct pair_like<std::tuple<Ts...>> : meta::bool_<sizeof...(Ts) == 2u>
        {};

        // clang-format off
        CPP_def
        (
            template(typename T, typename U, typename V)
            concept pair_like_convertible_from_gcc_bugs_,
                convertible_to_not_slicing_<U, meta::_t<std::tuple_element<0, T>>> &&
                convertible_to<V, meta::_t<std::tuple_element<1, T>>>
        );
        CPP_def
        (
            template(typename T, typename U, typename V)
            concept pair_like_convertible_from_,
                (!range<T>) &&
                constructible_from<T, U, V> &&
                (ranges::defer::is_true<pair_like<uncvref_t<T>>::value> &&
                 defer::pair_like_convertible_from_gcc_bugs_<T, U, V>)
        );
        CPP_def
        (
            template(typename R, typename I, typename S)
            concept range_convertible_to_,
                forwarding_range_<R> &&
                convertible_to_not_slicing_<iterator_t<R>, I> &&
                convertible_to<sentinel_t<R>, S>
        );
        // clang-format on

        template<typename S, typename I>
        constexpr auto is_sized_sentinel_() noexcept -> CPP_ret(bool)( //
            requires sentinel_for<S, I>)
        {
            return (bool)sized_sentinel_for<S, I>;
        }

        template<subrange_kind K, typename S, typename I>
        constexpr bool store_size_() noexcept
        {
            return K == subrange_kind::sized && !(bool)sized_sentinel_for<S, I>;
        }
    } // namespace detail
    /// \endcond

    template<typename I, typename S = I,
             subrange_kind K =
                 static_cast<subrange_kind>(detail::is_sized_sentinel_<S, I>())>
    struct subrange;

    /// \cond
    namespace _subrange_
    {
        struct adl_hook
        {};

        // A temporary subrange can be safely passed to ranges::begin and ranges::end.
        template<typename I, typename S, subrange_kind K>
        constexpr I begin(subrange<I, S, K> && r) noexcept(
            std::is_nothrow_copy_constructible<I>::value)
        {
            return r.begin();
        }
        template<typename I, typename S, subrange_kind K>
        constexpr I begin(subrange<I, S, K> const && r) noexcept(
            std::is_nothrow_copy_constructible<I>::value)
        {
            return r.begin();
        }
        template<typename I, typename S, subrange_kind K>
        constexpr S end(subrange<I, S, K> && r) noexcept(
            std::is_nothrow_copy_constructible<S>::value)
        {
            return r.end();
        }
        template<typename I, typename S, subrange_kind K>
        constexpr S end(subrange<I, S, K> const && r) noexcept(
            std::is_nothrow_copy_constructible<S>::value)
        {
            return r.end();
        }

        template<std::size_t N, typename I, typename S, subrange_kind K>
        constexpr auto get(subrange<I, S, K> const & r) -> CPP_ret(I)( //
            requires(N == 0))
        {
            return r.begin();
        }
        template<std::size_t N, typename I, typename S, subrange_kind K>
        constexpr auto get(subrange<I, S, K> const & r) -> CPP_ret(S)( //
            requires(N == 1))
        {
            return r.end();
        }
    } // namespace _subrange_
    /// \endcond

    template<typename I, typename S, subrange_kind K>
    struct subrange
      : view_interface<subrange<I, S, K>,
                       same_as<S, unreachable_sentinel_t>
                           ? infinite
                           : K == subrange_kind::sized ? finite : unknown>
      , private _subrange_::adl_hook
    {
        CPP_assert(input_or_output_iterator<I>);
        CPP_assert(sentinel_for<S, I>);
        CPP_assert(K == subrange_kind::sized || !sized_sentinel_for<S, I>);
        CPP_assert(K != subrange_kind::sized || !same_as<S, unreachable_sentinel_t>);

        using size_type = detail::iter_size_t<I>;
        using iterator = I;
        using sentinel = S;

        subrange() = default;

        template<typename I2>
        constexpr CPP_ctor(subrange)(I2 && i, S s)( //
            requires detail::convertible_to_not_slicing_<I2, I> &&
            (!detail::store_size_<K, S, I>()))
          : data_{static_cast<I2 &&>(i), std::move(s)}
        {}

        template<typename I2>
        constexpr CPP_ctor(subrange)(I2 && i, S s, size_type n)( //
            requires detail::convertible_to_not_slicing_<I2, I> &&
            (detail::store_size_<K, S, I>()))
          : data_{static_cast<I2 &&>(i), std::move(s), n}
        {
            if(RANGES_CONSTEXPR_IF((bool)random_access_iterator<I>))
            {
                using D = iter_difference_t<I>;
                RANGES_EXPECT(n <= (size_type)std::numeric_limits<D>::max());
                RANGES_EXPECT(ranges::next(first_(), (D)n) == last_());
            }
        }
        template<typename I2>
        constexpr CPP_ctor(subrange)(I2 && i, S s, size_type n)( //
            requires detail::convertible_to_not_slicing_<I2, I> &&
                sized_sentinel_for<S, I>)
          : data_{static_cast<I2 &&>(i), std::move(s)}
        {
            RANGES_EXPECT(static_cast<size_type>(last_() - first_()) == n);
        }

        template<typename R>
        constexpr CPP_ctor(subrange)(R && r)(
            requires defer::not_same_as_<R, subrange> &&
                detail::defer::range_convertible_to_<R, I, S> &&
                    defer::is_true<!detail::store_size_<K, S, I>()>)
          : subrange{ranges::begin(r), ranges::end(r)}
        {}

        template<typename R>
        constexpr CPP_ctor(subrange)(R && r)(
            requires defer::not_same_as_<R, subrange> &&
                detail::defer::range_convertible_to_<R, I, S> && defer::is_true<
                    detail::store_size_<K, S, I>()> && defer::sized_range<R>)
          : subrange{ranges::begin(r), ranges::end(r), ranges::size(r)}
        {}

        template<typename R>
        constexpr CPP_ctor(subrange)(R && r, size_type n)(
            requires detail::range_convertible_to_<R, I, S> &&
            (K == subrange_kind::sized))
          : subrange{ranges::begin(r), ranges::end(r), n}
        {
            if(RANGES_CONSTEXPR_IF((bool)sized_range<R>))
            {
                RANGES_EXPECT(n == ranges::size(r));
            }
        }

        /// Implicit conversion to something that looks like a container.
        CPP_template(typename Container)(                                         //
            requires detail::convertible_to_container<subrange const, Container>) //
            constexpr
            operator Container() const
        {
            return ranges::to<Container>(*this);
        }

        CPP_template(typename PairLike)( //
            requires defer::not_same_as_<PairLike, subrange> &&
                detail::defer::pair_like_convertible_from_<PairLike, const I &,
                                                           const S &>) //
            constexpr
            operator PairLike() const
        {
            return PairLike(first_(), last_());
        }

        constexpr I begin() const noexcept(std::is_nothrow_copy_constructible<I>::value)
        {
            return first_();
        }
        constexpr S end() const noexcept(std::is_nothrow_copy_constructible<S>::value)
        {
            return last_();
        }
        constexpr bool empty() const
        {
            return first_() == last_();
        }

        CPP_member
        constexpr auto size() const -> CPP_ret(size_type)( //
            requires(K == subrange_kind::sized))
        {
            return get_size_();
        }

        RANGES_NODISCARD
        constexpr subrange next(iter_difference_t<I> n = 1) const
        {
            auto tmp = *this;
            tmp.advance(n);
            return tmp;
        }

        CPP_member
        RANGES_NODISCARD constexpr auto prev(iter_difference_t<I> n = 1) const
            -> CPP_ret(subrange)( //
                requires bidirectional_iterator<I>)
        {
            auto tmp = *this;
            tmp.advance(-n);
            return tmp;
        }

        constexpr subrange & advance(iter_difference_t<I> n)
        {
            set_size_(get_size_() -
                      static_cast<size_type>(n - ranges::advance(first_(), n, last_())));
            return *this;
        }

    private:
        detail::if_then_t<detail::store_size_<K, S, I>(), std::tuple<I, S, size_type>,
                          std::tuple<I, S>>
            data_;

        constexpr I & first_() noexcept
        {
            return std::get<0>(data_);
        }
        constexpr const I & first_() const noexcept
        {
            return std::get<0>(data_);
        }
        constexpr S & last_() noexcept
        {
            return std::get<1>(data_);
        }
        constexpr const S & last_() const noexcept
        {
            return std::get<1>(data_);
        }
        CPP_member
        constexpr auto get_size_() const -> CPP_ret(size_type)( //
            requires sized_sentinel_for<S, I>)
        {
            return static_cast<size_type>(last_() - first_());
        }
        CPP_member
        constexpr auto get_size_() const noexcept -> CPP_ret(size_type)( //
            requires(detail::store_size_<K, S, I>()))
        {
            return std::get<2>(data_);
        }
        static constexpr void set_size_(...) noexcept
        {}
        CPP_member
        constexpr auto set_size_(size_type n) noexcept -> CPP_ret(void)( //
            requires(detail::store_size_<K, S, I>()))
        {
            std::get<2>(data_) = n;
        }
    };

#if RANGES_CXX_DEDUCTION_GUIDES >= RANGES_CXX_DEDUCTION_GUIDES_17
    template<typename I, typename S>
    subrange(I, S)->subrange<I, S>;

    CPP_template(typename I, typename S)(                           //
        requires input_or_output_iterator<I> && sentinel_for<S, I>) //
        subrange(I, S, detail::iter_size_t<I>)
            ->subrange<I, S, subrange_kind::sized>;

    CPP_template(typename R)(          //
        requires forwarding_range_<R>) //
        subrange(R &&)
            ->subrange<iterator_t<R>, sentinel_t<R>,
                       (sized_range<R> ||
                        sized_sentinel_for<sentinel_t<R>, iterator_t<R>>)
                           ? subrange_kind::sized
                           : subrange_kind::unsized>;

    CPP_template(typename R)(          //
        requires forwarding_range_<R>) //
        subrange(R &&, detail::iter_size_t<iterator_t<R>>)
            ->subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;
#endif

    // in lieu of deduction guides, use make_subrange
    struct make_subrange_fn
    {
        template<typename I, typename S>
        constexpr auto operator()(I i, S s) const -> subrange<I, S>
        {
            return {i, s};
        }
        template<typename I, typename S>
        constexpr auto operator()(I i, S s, detail::iter_size_t<I> n) const
            -> CPP_ret(subrange<I, S, subrange_kind::sized>)( //
                requires input_or_output_iterator<I> && sentinel_for<S, I>)
        {
            return {i, s, n};
        }
        template<typename R>
        constexpr auto operator()(R && r) const -> CPP_ret(
            subrange<iterator_t<R>, sentinel_t<R>,
                     (sized_range<R> || sized_sentinel_for<sentinel_t<R>, iterator_t<R>>)
                         ? subrange_kind::sized
                         : subrange_kind::unsized>)( //
            requires forwarding_range_<R>)
        {
            return {(R &&) r};
        }
        template<typename R>
        constexpr auto operator()(R && r, detail::iter_size_t<iterator_t<R>> n) const
            -> CPP_ret(subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>)( //
                requires forwarding_range_<R>)
        {
            return {(R &&) r, n};
        }
    };

    /// \relates make_subrange_fn
    /// \ingroup group-views
    RANGES_INLINE_VARIABLE(make_subrange_fn, make_subrange)

    template<typename R>
    using safe_subrange_t = detail::maybe_dangling_<R, subrange<iterator_t<R>>>;

    namespace cpp20
    {
        using ranges::subrange_kind;

        CPP_template(typename I,                                          //
                     typename S = I,                                      //
                     subrange_kind K =                                    //
                     static_cast<subrange_kind>(                          //
                         detail::is_sized_sentinel_<S, I>()))(            //
            requires input_or_output_iterator<I> && sentinel_for<S, I> && //
            (K == subrange_kind::sized || !sized_sentinel_for<S, I>))     //
            using subrange = ranges::subrange<I, S>;

        using ranges::safe_subrange_t;
    } // namespace cpp20
    /// @}
} // namespace ranges

RANGES_DIAGNOSTIC_PUSH
RANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS

namespace std
{
    template<typename I, typename S, ::ranges::subrange_kind K>
    struct tuple_size<::ranges::subrange<I, S, K>> : std::integral_constant<size_t, 2>
    {};
    template<typename I, typename S, ::ranges::subrange_kind K>
    struct tuple_element<0, ::ranges::subrange<I, S, K>>
    {
        using type = I;
    };
    template<typename I, typename S, ::ranges::subrange_kind K>
    struct tuple_element<1, ::ranges::subrange<I, S, K>>
    {
        using type = S;
    };
} // namespace std

RANGES_DIAGNOSTIC_POP

#endif
