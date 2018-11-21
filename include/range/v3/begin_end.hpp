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
                template<typename R>
                using member_begin_t = detail::decay_t<decltype(static_cast<R &(*)()>(nullptr)().begin())>;
                template<typename R>
                using non_member_begin_t = detail::decay_t<decltype(begin(static_cast<R &(*)()>(nullptr)()))>;

                template<typename R, std::size_t N>
                static constexpr R *impl_(R (&array)[N], int) noexcept
                {
                    return array;
                }

                // Prefer member if it returns Iterator.
                template<typename R>
                static constexpr auto impl_(R &r, int)
                    noexcept(noexcept(member_begin_t<R>(r.begin()))) ->
                    CPP_ret(member_begin_t<R>)(
                        requires Iterator<member_begin_t<R>>)
                {
                    return r.begin();
                }

                // Use ADL if it returns Iterator.
                template<typename R>
                static constexpr auto impl_(R &r, long)
                    noexcept(noexcept(non_member_begin_t<R>(begin(r)))) ->
                    CPP_ret(non_member_begin_t<R>)(
                        requires Iterator<non_member_begin_t<R>>)
                {
                    return r.begin();
                }

            public:
                template<typename R>
                constexpr auto CPP_auto_fun(operator())(R &r) (const)
                (
                    return fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto CPP_auto_fun(operator())(std::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto CPP_auto_fun(operator())(ranges::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
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
                template<typename R>
                using begin_t = decltype(ranges::begin(static_cast<R &(*)()>(nullptr)()));
                template<typename R>
                using member_end_t = detail::decay_t<decltype(static_cast<R &(*)()>(nullptr)().end())>;
                template<typename R>
                using non_member_end_t = detail::decay_t<decltype(end(static_cast<R &(*)()>(nullptr)()))>;

                template<typename R, std::size_t N>
                static constexpr R *impl_(R (&array)[N], int) noexcept
                {
                    return array + N;
                }

                // Prefer member if it returns Sentinel.
                template<typename R>
                static constexpr auto impl_(R &r, int)
                    noexcept(noexcept(member_end_t<R>(r.end()))) ->
                    CPP_ret(member_end_t<R>)(
                        requires Sentinel<member_end_t<R>, begin_t<R>>)
                {
                    return r.end();
                }

                // Use ADL if it returns Sentinel.
                template<typename R>
                static constexpr auto impl_(R &r, long)
                    noexcept(noexcept(non_member_end_t<R>(end(r)))) ->
                    CPP_ret(non_member_end_t<R>)(
                        requires Sentinel<non_member_end_t<R>, begin_t<R>>)
                {
                    return end(r);
                }

            public:
                template<typename R>
                constexpr auto CPP_auto_fun(operator())(R &r) (const)
                (
                    return fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto CPP_auto_fun(operator())(std::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto CPP_auto_fun(operator())(ranges::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
                )
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
                constexpr auto CPP_auto_fun(operator())(R const &r) (const)
                (
                    return ranges::begin(r)
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
                constexpr auto CPP_auto_fun(operator())(R const &r) (const)
                (
                    return ranges::end(r)
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
                template<typename R>
                using begin_t = decltype(ranges::begin(static_cast<R &(*)()>(nullptr)()));
                template<typename R>
                using end_t = decltype(ranges::end(static_cast<R &(*)()>(nullptr)()));
                template<typename R>
                using member_rbegin_t = detail::decay_t<decltype(static_cast<R &(*)()>(nullptr)().rbegin())>;

                template<typename R, std::size_t N>
                static constexpr reverse_iterator<R *> impl_(R (&array)[N], int) noexcept
                {
                    return ranges::make_reverse_iterator(array + N);
                }

                // Prefer member if it returns Iterator.
                template<typename R>
                static constexpr auto impl_(R &r, int)
                    noexcept(noexcept(member_rbegin_t<R>(r.rbegin()))) ->
                    CPP_ret(member_rbegin_t<R>)(
                        requires Iterator<member_rbegin_t<R>>)
                {
                    return r.rbegin();
                }

                template<typename R>
                static constexpr auto impl_(R &r, long)
                    noexcept(noexcept(reverse_iterator<end_t<R>>(
                        ranges::make_reverse_iterator(ranges::end(r))))) ->
                    CPP_ret(reverse_iterator<end_t<R>>)(
                        requires Same<begin_t<R>, end_t<R>> && BidirectionalIterator<end_t<R>>)
                {
                    return ranges::make_reverse_iterator(ranges::end(r));
                }

            public:
                template<typename R>
                constexpr auto CPP_auto_fun(operator())(R &r) (const)
                (
                    return fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto CPP_auto_fun(operator())(std::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto CPP_auto_fun(operator())(ranges::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
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
                template<typename R>
                using begin_t = decltype(ranges::begin(static_cast<R &(*)()>(nullptr)()));
                template<typename R>
                using end_t = decltype(ranges::end(static_cast<R &(*)()>(nullptr)()));
                template<typename R>
                using rbegin_t = decltype(ranges::rbegin(static_cast<R &(*)()>(nullptr)()));
                template<typename R>
                using member_rend_t = detail::decay_t<decltype(static_cast<R &(*)()>(nullptr)().rend())>;

                template<typename R, std::size_t N>
                static constexpr reverse_iterator<R *> impl_(R (&array)[N], int) noexcept
                {
                    return ranges::make_reverse_iterator(array);
                }

                // Prefer member if it returns Sentinel.
                template<typename R>
                static constexpr auto impl_(R &r, int)
                    noexcept(noexcept(member_rend_t<R>(r.rend()))) ->
                    CPP_ret(member_rend_t<R>)(
                        requires Sentinel<member_rend_t<R>, rbegin_t<R>>)
                {
                    return r.rend();
                }

                template<typename R>
                static constexpr auto impl_(R &r, long)
                    noexcept(noexcept(reverse_iterator<begin_t<R>>(
                        ranges::make_reverse_iterator(ranges::begin(r))))) ->
                    CPP_ret(reverse_iterator<begin_t<R>>)(
                        requires Same<begin_t<R>, end_t<R>> && BidirectionalIterator<begin_t<R>>)
                {
                    return ranges::make_reverse_iterator(ranges::begin(r));
                }

            public:
                template<typename R>
                constexpr auto CPP_auto_fun(operator())(R &r) (const)
                (
                    return fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto CPP_auto_fun(operator())(std::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
                RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
                constexpr auto CPP_auto_fun(operator())(ranges::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
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
                constexpr auto CPP_auto_fun(operator())(R const &r) (const)
                (
                    return ranges::rbegin(r)
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
                constexpr auto CPP_auto_fun(operator())(R const &r) (const)
                (
                    return ranges::rend(r)
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
