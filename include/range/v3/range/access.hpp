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

#ifndef RANGES_V3_RANGE_ACCESS_HPP
#define RANGES_V3_RANGE_ACCESS_HPP

#include <functional> // for reference_wrapper (whose use with begin/end is deprecated)
#include <initializer_list>
#include <iterator>
#include <limits>
#include <utility>

#ifdef __has_include
#if __has_include(<span>)
#include <span>
#endif
#if __has_include(<string_view>)
#include <string_view>
#endif
#endif

#include <range/v3/range_fwd.hpp>

#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/reverse_iterator.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/utility/static_const.hpp>

#include <range/v3/detail/prologue.hpp>

namespace ranges
{
#if defined(__cpp_lib_string_view) && __cpp_lib_string_view > 0
    template<class CharT, class Traits>
    RANGES_INLINE_VAR constexpr bool
        enable_borrowed_range<std::basic_string_view<CharT, Traits>> = true;
#endif

#if defined(__cpp_lib_span) && __cpp_lib_span > 0
    template<class T, std::size_t N>
    RANGES_INLINE_VAR constexpr bool enable_borrowed_range<std::span<T, N>> = true;
#endif

    namespace detail
    {
        template<typename T>
        RANGES_INLINE_VAR constexpr bool _borrowed_range =
            enable_borrowed_range<uncvref_t<T>>;

        template<typename T>
        RANGES_INLINE_VAR constexpr bool _borrowed_range<T &> = true;
    } // namespace detail

    /// \cond
    namespace _begin_
    {
        // Poison pill for std::begin. (See the detailed discussion at
        // https://github.com/ericniebler/stl2/issues/139)
        template<typename T>
        void begin(T &&) = delete;

#ifdef RANGES_WORKAROUND_MSVC_895622
        void begin();
#endif

        template<typename T>
        void begin(std::initializer_list<T>) = delete;

        template(typename I)(
            /// \pre
            requires input_or_output_iterator<I>)
        void is_iterator(I);

        // clang-format off
        template<typename T>
        CPP_requires(has_member_begin_,
            requires(T & t) //
            (
                _begin_::is_iterator(t.begin())
            ));
        template<typename T>
        CPP_concept has_member_begin =
            CPP_requires_ref(_begin_::has_member_begin_, T);

        template<typename T>
        CPP_requires(has_non_member_begin_,
            requires(T & t) //
            (
                _begin_::is_iterator(begin(t))
            ));
        template<typename T>
        CPP_concept has_non_member_begin =
            CPP_requires_ref(_begin_::has_non_member_begin_, T);
        // clang-format on

        struct fn
        {
        private:
            template<bool>
            struct impl_
            {
                // has_member_begin == true
                template<typename R>
                static constexpr auto invoke(R && r) noexcept(noexcept(r.begin()))
                {
                    return r.begin();
                }
            };

            template<typename R>
            using impl = impl_<has_member_begin<R>>;

        public:
            template<typename R, std::size_t N>
            void operator()(R(&&)[N]) const = delete;

            template<typename R, std::size_t N>
            constexpr R * operator()(R (&array)[N]) const noexcept
            {
                return array;
            }

            template(typename R)(
                /// \pre
                requires detail::_borrowed_range<R> AND
                    (has_member_begin<R> || has_non_member_begin<R>))
            constexpr auto operator()(R && r) const //
                noexcept(noexcept(impl<R>::invoke(r)))
            {
                return impl<R>::invoke(r);
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED(
                "Using a reference_wrapper as a range is deprecated. Use views::ref "
                "instead.")
            constexpr auto
            operator()(std::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) -> decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED(
                "Using a reference_wrapper as a range is deprecated. Use views::ref "
                "instead.")
            constexpr auto
            operator()(ranges::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) -> decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }
        };

        template<>
        struct fn::impl_<false>
        {
            // has_member_begin == false
            template<typename R>
            static constexpr auto invoke(R && r) noexcept(noexcept(begin(r)))
            {
                return begin(r);
            }
        };

        template<typename R>
        using _t = decltype(fn{}(std::declval<R>()));
    } // namespace _begin_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return \c r, if \c r is an array. Otherwise, `r.begin()` if that expression is
    ///   well-formed and returns an input_or_output_iterator. Otherwise, `begin(r)` if
    ///   that expression returns an input_or_output_iterator.
    RANGES_DEFINE_CPO(_begin_::fn, begin)

    /// \cond
    namespace _end_
    {
        // Poison pill for std::end. (See the detailed discussion at
        // https://github.com/ericniebler/stl2/issues/139)
        template<typename T>
        void end(T &&) = delete;

#ifdef RANGES_WORKAROUND_MSVC_895622
        void end();
#endif

        template<typename T>
        void end(std::initializer_list<T>) = delete;

        template(typename I, typename S)(
            /// \pre
            requires sentinel_for<S, I>)
        void _is_sentinel(S, I);

        // clang-format off
        template<typename T>
        CPP_requires(has_member_end_,
            requires(T & t) //
            (
                _end_::_is_sentinel(t.end(), ranges::begin(t))
            ));
        template<typename T>
        CPP_concept has_member_end =
            CPP_requires_ref(_end_::has_member_end_, T);

        template<typename T>
        CPP_requires(has_non_member_end_,
            requires(T & t) //
            (
                _end_::_is_sentinel(end(t), ranges::begin(t))
            ));
        template<typename T>
        CPP_concept has_non_member_end =
            CPP_requires_ref(_end_::has_non_member_end_, T);
        // clang-format on

        struct fn
        {
        private:
            template<bool>
            struct impl_
            {
                // has_member_end == true
                template<typename R>
                static constexpr auto invoke(R && r) noexcept(noexcept(r.end()))
                {
                    return r.end();
                }
            };

            template<typename Int>
            using iter_diff_t =
                meta::_t<meta::conditional_t<std::is_integral<Int>::value,
                                             std::make_signed<Int>, //
                                             meta::id<Int>>>;

            template<typename R>
            using impl = impl_<has_member_end<R>>;

        public:
            template<typename R, std::size_t N>
            void operator()(R(&&)[N]) const = delete;

            template<typename R, std::size_t N>
            constexpr R * operator()(R (&array)[N]) const noexcept
            {
                return array + N;
            }

            template(typename R)(
                /// \pre
                requires detail::_borrowed_range<R> AND
                    (has_member_end<R> || has_non_member_end<R>))
            constexpr auto operator()(R && r) const //
                noexcept(noexcept(impl<R>::invoke(r))) //
            {
                return impl<R>::invoke(r);
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED(
                "Using a reference_wrapper as a range is deprecated. Use views::ref "
                "instead.")
            constexpr auto
            operator()(std::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) -> decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED(
                "Using a reference_wrapper as a range is deprecated. Use views::ref "
                "instead.")
            constexpr auto
            operator()(ranges::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) -> decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template(typename Int)(
                /// \pre
                requires detail::integer_like_<Int>)
            auto operator-(Int dist) const
                -> detail::from_end_<iter_diff_t<Int>>
            {
                using SInt = iter_diff_t<Int>;
                RANGES_EXPECT(0 <= dist);
                RANGES_EXPECT(dist <=
                              static_cast<Int>((std::numeric_limits<SInt>::max)()));
                return detail::from_end_<SInt>{-static_cast<SInt>(dist)};
            }
        };

        // has_member_end == false
        template<>
        struct fn::impl_<false>
        {
            template<typename R>
            static constexpr auto invoke(R && r) noexcept(noexcept(end(r)))
            {
                return end(r);
            }
        };

        template<typename R>
        using _t = decltype(fn{}(std::declval<R>()));
    } // namespace _end_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return \c r+size(r), if \c r is an array. Otherwise, `r.end()` if that expression
    /// is
    ///   well-formed and returns an input_or_output_iterator. Otherwise, `end(r)` if that
    ///   expression returns an input_or_output_iterator.
    RANGES_DEFINE_CPO(_end_::fn, end)

    /// \cond
    namespace _cbegin_
    {
        struct fn
        {
            template<typename R>
            constexpr _begin_::_t<detail::as_const_t<R>> operator()(R && r) const
                noexcept(noexcept(ranges::begin(detail::as_const(r))))
            {
                return ranges::begin(detail::as_const(r));
            }
        };
    } // namespace _cbegin_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return The result of calling `ranges::begin` with a const-qualified
    ///    reference to r.
    RANGES_INLINE_VARIABLE(_cbegin_::fn, cbegin)

    /// \cond
    namespace _cend_
    {
        struct fn
        {
            template<typename R>
            constexpr _end_::_t<detail::as_const_t<R>> operator()(R && r) const
                noexcept(noexcept(ranges::end(detail::as_const(r))))
            {
                return ranges::end(detail::as_const(r));
            }
        };
    } // namespace _cend_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return The result of calling `ranges::end` with a const-qualified
    ///    reference to r.
    RANGES_INLINE_VARIABLE(_cend_::fn, cend)

    /// \cond
    namespace _rbegin_
    {
        template<typename R>
        void rbegin(R &&) = delete;
        // Non-standard, to keep unqualified rbegin(r) from finding std::rbegin
        // and returning a std::reverse_iterator.
        template<typename T>
        void rbegin(std::initializer_list<T>) = delete;
        template<typename T, std::size_t N>
        void rbegin(T (&)[N]) = delete;

        // clang-format off
        template<typename T>
        CPP_requires(has_member_rbegin_,
            requires(T & t) //
            (
                _begin_::is_iterator(t.rbegin())
            ));
        template<typename T>
        CPP_concept has_member_rbegin =
            CPP_requires_ref(_rbegin_::has_member_rbegin_, T);

        template<typename T>
        CPP_requires(has_non_member_rbegin_,
            requires(T & t) //
            (
                _begin_::is_iterator(rbegin(t))
            ));
        template<typename T>
        CPP_concept has_non_member_rbegin =
            CPP_requires_ref(_rbegin_::has_non_member_rbegin_, T);

        template<typename I>
        void _same_type(I, I);

        template<typename T>
        CPP_requires(can_reverse_end_,
            requires(T & t) //
            (
                // make_reverse_iterator is constrained with
                // bidirectional_iterator.
                ranges::make_reverse_iterator(ranges::end(t)),
                _rbegin_::_same_type(ranges::begin(t), ranges::end(t))
            ));
        template<typename T>
        CPP_concept can_reverse_end =
            CPP_requires_ref(_rbegin_::can_reverse_end_, T);
        // clang-format on

        struct fn
        {
        private:
            // has_member_rbegin == true
            template<int>
            struct impl_
            {
                template<typename R>
                static constexpr auto invoke(R && r) noexcept(noexcept(r.rbegin()))
                {
                    return r.rbegin();
                }
            };

            template<typename R>
            using impl =
                impl_<has_member_rbegin<R> ? 0 : has_non_member_rbegin<R> ? 1 : 2>;

        public:
            template(typename R)(
                /// \pre
                requires detail::_borrowed_range<R> AND
                    (has_member_rbegin<R> ||
                     has_non_member_rbegin<R> ||
                     can_reverse_end<R>)) //
            constexpr auto operator()(R && r) const //
                noexcept(noexcept(impl<R>::invoke(r))) //
            {
                return impl<R>::invoke(r);
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED(
                "Using a reference_wrapper as a range is deprecated. Use views::ref "
                "instead.")
            constexpr auto operator()(std::reference_wrapper<T> ref) const //
                noexcept(noexcept(Fn{}(ref.get()))) //
                -> decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED(
                "Using a reference_wrapper as a range is deprecated. Use views::ref "
                "instead.")
            constexpr auto
            operator()(ranges::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) //
                -> decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }
        };

        // has_non_member_rbegin == true
        template<>
        struct fn::impl_<1>
        {
            template<typename R>
            static constexpr auto invoke(R && r) noexcept(noexcept(rbegin(r)))
            {
                return rbegin(r);
            }
        };

        // can_reverse_end
        template<>
        struct fn::impl_<2>
        {
            template<typename R>
            static constexpr auto invoke(R && r)
                noexcept(noexcept(ranges::make_reverse_iterator(ranges::end(r))))
            {
                return ranges::make_reverse_iterator(ranges::end(r));
            }
        };

        template<typename R>
        using _t = decltype(fn{}(std::declval<R>()));
    } // namespace _rbegin_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return `make_reverse_iterator(r + ranges::size(r))` if r is an array. Otherwise,
    ///   `r.rbegin()` if that expression is well-formed and returns an
    ///   input_or_output_iterator. Otherwise, `make_reverse_iterator(ranges::end(r))` if
    ///   `ranges::begin(r)` and `ranges::end(r)` are both well-formed and have the same
    ///   type that satisfies `bidirectional_iterator`.
    RANGES_DEFINE_CPO(_rbegin_::fn, rbegin)

    /// \cond
    namespace _rend_
    {
        template<typename R>
        void rend(R &&) = delete;
        // Non-standard, to keep unqualified rend(r) from finding std::rend
        // and returning a std::reverse_iterator.
        template<typename T>
        void rend(std::initializer_list<T>) = delete;
        template<typename T, std::size_t N>
        void rend(T (&)[N]) = delete;

        // clang-format off
        template<typename T>
        CPP_requires(has_member_rend_,
            requires(T & t) //
            (
                _end_::_is_sentinel(t.rend(), ranges::rbegin(t))
            ));
        template<typename T>
        CPP_concept has_member_rend =
            CPP_requires_ref(_rend_::has_member_rend_, T);

        template<typename T>
        CPP_requires(has_non_member_rend_,
            requires(T & t) //
            (
                _end_::_is_sentinel(rend(t), ranges::rbegin(t))
            ));
        template<typename T>
        CPP_concept has_non_member_rend =
            CPP_requires_ref(_rend_::has_non_member_rend_, T);

        template<typename T>
        CPP_requires(can_reverse_begin_,
            requires(T & t) //
            (
                // make_reverse_iterator is constrained with
                // bidirectional_iterator.
                ranges::make_reverse_iterator(ranges::begin(t)),
                _rbegin_::_same_type(ranges::begin(t), ranges::end(t))
            ));
        template<typename T>
        CPP_concept can_reverse_begin =
            CPP_requires_ref(_rend_::can_reverse_begin_, T);
        // clang-format on

        struct fn
        {
        private:
            // has_member_rbegin == true
            template<int>
            struct impl_
            {
                template<typename R>
                static constexpr auto invoke(R && r) noexcept(noexcept(r.rend()))
                {
                    return r.rend();
                }
            };

            template<typename R>
            using impl = impl_<has_member_rend<R> ? 0 : has_non_member_rend<R> ? 1 : 2>;

        public:
            template(typename R)(
                /// \pre
                requires detail::_borrowed_range<R> AND
                    (has_member_rend<R> || //
                     has_non_member_rend<R> || //
                     can_reverse_begin<R>)) //
            constexpr auto operator()(R && r) const
                noexcept(noexcept(impl<R>::invoke(r))) //
            {
                return impl<R>::invoke(r);
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED(
                "Using a reference_wrapper as a range is deprecated. Use views::ref "
                "instead.")
            constexpr auto operator()(std::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) //
                -> decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED(
                "Using a reference_wrapper as a range is deprecated. Use views::ref "
                "instead.")
            constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) //
                -> decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }
        };

        // has_non_member_rend == true
        template<>
        struct fn::impl_<1>
        {
            template<typename R>
            static constexpr auto invoke(R && r) noexcept(noexcept(rend(r)))
            {
                return rend(r);
            }
        };

        // can_reverse_begin
        template<>
        struct fn::impl_<2>
        {
            template<typename R>
            static constexpr auto invoke(R && r)
                noexcept(noexcept(ranges::make_reverse_iterator(ranges::begin(r))))
            {
                return ranges::make_reverse_iterator(ranges::begin(r));
            }
        };

        template<typename R>
        using _t = decltype(fn{}(std::declval<R>()));
    } // namespace _rend_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return `make_reverse_iterator(r)` if `r` is an array. Otherwise,
    ///   `r.rend()` if that expression is well-formed and returns a type that
    ///   satisfies `sentinel_for<S, I>` where `I` is the type of `ranges::rbegin(r)`.
    ///   Otherwise, `make_reverse_iterator(ranges::begin(r))` if `ranges::begin(r)`
    ///   and `ranges::end(r)` are both well-formed and have the same type that
    ///   satisfies `bidirectional_iterator`.
    RANGES_DEFINE_CPO(_rend_::fn, rend)

    /// \cond
    namespace _crbegin_
    {
        struct fn
        {
            template<typename R>
            constexpr _rbegin_::_t<detail::as_const_t<R>> operator()(R && r) const
                noexcept(noexcept(ranges::rbegin(detail::as_const(r))))
            {
                return ranges::rbegin(detail::as_const(r));
            }
        };
    } // namespace _crbegin_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return The result of calling `ranges::rbegin` with a const-qualified
    ///    reference to r.
    RANGES_INLINE_VARIABLE(_crbegin_::fn, crbegin)

    /// \cond
    namespace _crend_
    {
        struct fn
        {
            template<typename R>
            constexpr _rend_::_t<detail::as_const_t<R>> operator()(R && r) const
                noexcept(noexcept(ranges::rend(detail::as_const(r))))
            {
                return ranges::rend(detail::as_const(r));
            }
        };
    } // namespace _crend_
    /// \endcond

    /// \ingroup group-range
    /// \param r
    /// \return The result of calling `ranges::rend` with a const-qualified
    ///    reference to r.
    RANGES_INLINE_VARIABLE(_crend_::fn, crend)

    template<typename Rng>
    using iterator_t = decltype(begin(std::declval<Rng &>()));

    template<typename Rng>
    using sentinel_t = decltype(end(std::declval<Rng &>()));

    namespace cpp20
    {
        using ranges::begin;
        using ranges::cbegin;
        using ranges::cend;
        using ranges::crbegin;
        using ranges::crend;
        using ranges::end;
        using ranges::rbegin;
        using ranges::rend;

        using ranges::iterator_t;
        using ranges::sentinel_t;

        using ranges::enable_borrowed_range;
    } // namespace cpp20
} // namespace ranges

#include <range/v3/detail/epilogue.hpp>

#endif
