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

#ifndef RANGES_V3_BEGIN_END_HPP
#define RANGES_V3_BEGIN_END_HPP

#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <utility>

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/utility/dangling.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace _begin_
        {
            // Poison pill for std::begin. (See the detailed discussion at
            // https://github.com/ericniebler/stl2/issues/139)
            template<typename T>
            void begin(T &) = delete;
            template<typename T>
            void begin(T const &) = delete;

#ifdef RANGES_WORKAROUND_MSVC_620035
            void begin();
#endif

#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 8 && __GNUC_MINOR__ < 2
            // Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85765
            template<typename T>
            void begin(std::initializer_list<T> volatile &) = delete;
            template<typename T>
            void begin(std::initializer_list<T> const volatile &) = delete;
#endif

            struct fn
            {
            private:
                template<typename R, std::size_t N>
                static constexpr R *impl_(R (&array)[N], int) noexcept
                {
                    return array;
                }

                // Prefer member if it returns Iterator.
                template<typename R,
                    typename I = decltype(aux::copy(std::declval<R &>().begin())),
                    CONCEPT_REQUIRES_(Iterator<I>())>
                static constexpr I impl_(R &r, int)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    r.begin()
                )

                // Use ADL if it returns Iterator.
                template<typename R,
                    typename I = decltype(aux::copy(begin(std::declval<R &>()))),
                    CONCEPT_REQUIRES_(Iterator<I>())>
                static constexpr I impl_(R &r, long)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    begin(r)
                )

            public:
                template<typename R>
                constexpr auto operator()(R &r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fn::impl_(r, 42)
                )

                template<typename R>
                RANGES_DEPRECATED("Passing an rvalue to ranges::begin is deprecated.")
                constexpr auto operator()(const R &&r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                constexpr auto operator()(std::reference_wrapper<T> ref) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
                constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Fn()(ref.get())
                )
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return \c r, if \c r is an array. Otherwise, `r.begin()` if that expression is
        ///   well-formed and returns an Iterator. Otherwise, `begin(r)` if that expression
        ///   returns an Iterator.
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_begin_::fn, begin)
        }

        /// \cond
        namespace _end_
        {
            // Poison pill for std::end. (See the detailed discussion at
            // https://github.com/ericniebler/stl2/issues/139)
            template<typename T>
            void end(T &) = delete;
            template<typename T>
            void end(T const &) = delete;

#ifdef RANGES_WORKAROUND_MSVC_620035
            void end();
#endif

#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 8 && __GNUC_MINOR__ < 2
            // Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85765
            template<typename T>
            void end(std::initializer_list<T> volatile &) = delete;
            template<typename T>
            void end(std::initializer_list<T> const volatile &) = delete;
#endif

            struct fn
            {
            private:
                template<typename R, std::size_t N>
                static constexpr R *impl_(R (&array)[N], int) noexcept
                {
                    return array + N;
                }

                // Prefer member if it returns Sentinel.
                template<typename R,
                    typename I = decltype(ranges::begin(std::declval<R &>())),
                    typename S = decltype(aux::copy(std::declval<R &>().end())),
                    CONCEPT_REQUIRES_(Sentinel<S, I>())>
                static constexpr S impl_(R &r, int)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    r.end()
                )

                // Use ADL if it returns Sentinel.
                template<typename R,
                    typename I = decltype(ranges::begin(std::declval<R &>())),
                    typename S = decltype(aux::copy(end(std::declval<R &>()))),
                    CONCEPT_REQUIRES_(Sentinel<S, I>())>
                static constexpr S impl_(R &r, long)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    end(r)
                )

            public:
                template<typename R>
                constexpr auto operator()(R &r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fn::impl_(r, 42)
                )

                template<typename R>
                RANGES_DEPRECATED("Passing an rvalue to ranges::end is deprecated.")
                constexpr auto operator()(const R &&r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                constexpr auto operator()(std::reference_wrapper<T> ref) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
                constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Fn()(ref.get())
                )

                template<typename Int, CONCEPT_REQUIRES_(Integral<Int>())>
                detail::from_end_<meta::_t<std::make_signed<Int>>> operator-(Int dist) const
                {
                    using T = meta::_t<std::make_signed<Int>>;
                    RANGES_EXPECT(0 <= dist);
                    RANGES_EXPECT(dist <= static_cast<Int>(std::numeric_limits<T>::max()));
                    return {-static_cast<meta::_t<std::make_signed<Int>>>(dist)};
                }
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return \c r+size(r), if \c r is an array. Otherwise, `r.end()` if that expression is
        ///   well-formed and returns an Iterator. Otherwise, `end(r)` if that expression
        ///   returns an Iterator.
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_end_::fn, end)
        }

        /// \cond
        namespace _cbegin_
        {
            struct fn
            {
                template<typename R>
                constexpr auto operator()(R const &r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::begin(r)
                )

                template<typename R>
                RANGES_DEPRECATED("Passing an rvalue to ranges::cbegin is deprecated.")
                constexpr auto operator()(const R &&r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::begin(r)
                )
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return The result of calling `ranges::begin` with a const-qualified
        ///    reference to r.
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_cbegin_::fn, cbegin)
        }

        /// \cond
        namespace _cend_
        {
            struct fn
            {
                template<typename R>
                constexpr auto operator()(R const &r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::end(r)
                )

                template<typename R>
                RANGES_DEPRECATED("Passing an rvalue to ranges::cend is deprecated.")
                constexpr auto operator()(const R &&r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::end(r)
                )
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return The result of calling `ranges::end` with a const-qualified
        ///    reference to r.
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_cend_::fn, cend)
        }

        /// \cond
        namespace _rbegin_
        {
            struct fn
            {
            private:
                template<typename R, std::size_t N>
                static constexpr reverse_iterator<R *> impl_(R (&array)[N], int) noexcept
                {
                    return ranges::make_reverse_iterator(array + N);
                }

                // Prefer member if it returns Iterator.
                template<typename R,
                    typename I = decltype(aux::copy(std::declval<R &>().rbegin())),
                    CONCEPT_REQUIRES_(Iterator<I>())>
                static constexpr I impl_(R &r, int)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    r.rbegin()
                )

                template<typename R,
                    typename I = decltype(ranges::begin(std::declval<R &>())),
                    typename S = decltype(ranges::end(std::declval<R &>())),
                    CONCEPT_REQUIRES_(Same<I, S>() && BidirectionalIterator<I>())>
                static constexpr reverse_iterator<I> impl_(R &r, long)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    ranges::make_reverse_iterator(ranges::end(r))
                )

            public:
                template<typename R>
                constexpr auto operator()(R &r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fn::impl_(r, 42)
                )

                template<typename R>
                RANGES_DEPRECATED("Passing an rvalue to ranges::rbegin is deprecated.")
                constexpr auto operator()(const R &&r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                constexpr auto operator()(std::reference_wrapper<T> ref) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
                constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Fn()(ref.get())
                )
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return `make_reverse_iterator(r+size(r))` if r is an array. Otherwise,
        ///   `r.rbegin()` if that expression is well-formed and returns an Iterator.
        ///   Otherwise, `make_reverse_iterator(ranges::end(r))` if `ranges::begin(r)`
        ///   and `ranges::end(r)` are both well-formed and have the same type that
        ///   satisfies BidirectionalIterator.
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_rbegin_::fn, rbegin)
        }

        /// \cond
        namespace _rend_
        {
            struct fn
            {
            private:
                template<typename R, std::size_t N>
                static constexpr reverse_iterator<R *> impl_(R (&array)[N], int) noexcept
                {
                    return ranges::make_reverse_iterator(array);
                }

                // Prefer member if it returns Sentinel.
                template<typename R,
                    typename I = decltype(ranges::rbegin(std::declval<R &>())),
                    typename S = decltype(aux::copy(std::declval<R &>().rend())),
                    CONCEPT_REQUIRES_(Sentinel<S, I>())>
                static constexpr S impl_(R &r, int)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    r.rend()
                )

                template<typename R,
                    typename I = decltype(ranges::begin(std::declval<R &>())),
                    typename S = decltype(ranges::end(std::declval<R &>())),
                    CONCEPT_REQUIRES_(Same<I, S>() && BidirectionalIterator<I>())>
                static constexpr reverse_iterator<I> impl_(R &r, long)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    ranges::make_reverse_iterator(ranges::begin(r))
                )

            public:
                template<typename R>
                constexpr auto operator()(R &r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fn::impl_(r, 42)
                )

                template<typename R>
                RANGES_DEPRECATED("Passing an rvalue to ranges::rend is deprecated.")
                constexpr auto operator()(const R &&r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                constexpr auto operator()(std::reference_wrapper<T> ref) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
                constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    Fn()(ref.get())
                )
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return `make_reverse_iterator(r))` if r is an array. Otherwise,
        ///   `r.rend()` if that expression is well-formed and returns a type that
        ///   satisfies `Sentinel<S, I>` where `I` is the type of `ranges::rbegin(r)`.
        ///   Otherwise, `make_reverse_iterator(ranges::begin(r))` if `ranges::begin(r)`
        ///   and `ranges::end(r)` are both well-formed and have the same type that
        ///   satisfies BidirectionalIterator.
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_rend_::fn, rend)
        }

        /// \cond
        namespace _crbegin_
        {
            struct fn
            {
                template<typename R>
                constexpr auto operator()(R const &r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::rbegin(r)
                )

                template<typename R>
                RANGES_DEPRECATED("Passing an rvalue to ranges::crbegin is deprecated.")
                constexpr auto operator()(const R &&r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::rbegin(r)
                )
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return The result of calling `ranges::rbegin` with a const-qualified
        ///    reference to r.
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_crbegin_::fn, crbegin)
        }

        /// \cond
        namespace _crend_
        {
            struct fn
            {
                template<typename R>
                constexpr auto operator()(R const &r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::rend(r)
                )

                template<typename R>
                RANGES_DEPRECATED("Passing an rvalue to ranges::crend is deprecated.")
                constexpr auto operator()(const R &&r) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::rend(r)
                )
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \param r
        /// \return The result of calling `ranges::rend` with a const-qualified
        ///    reference to r.
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_crend_::fn, crend)
        }
    }
}

#endif
