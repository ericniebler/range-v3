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
#if defined(__cpp_lib_string_view) && __cpp_lib_string_view > 0
#include <string_view>
#endif

#include <range/v3/range_fwd.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/reverse_iterator.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \cond
    namespace _begin_
    {
        // Poison pill for std::begin. (See the detailed discussion at
        // https://github.com/ericniebler/stl2/issues/139)
        template<typename T>
        void begin(T &&) = delete;

#ifdef RANGES_WORKAROUND_MSVC_620035
        void begin();
#endif

        template<typename T>
        void begin(std::initializer_list<T>) = delete;

        template<class I>
        auto is_iterator(I) -> CPP_ret(void)(requires Iterator<I>);

        CPP_def
        (
            template(typename T)
            concept HasMemberBegin,
                requires (T &t)
                (
                    _begin_::is_iterator(t.begin())
                ) &&
                std::is_lvalue_reference<T>::value
        );

        CPP_def
        (
            template(typename T)
            concept HasNonMemberBegin,
                requires (T &&t)
                (
                    _begin_::is_iterator(begin((T &&) t))
                )
        );

        struct fn
        {
        private:
            template<bool>
            struct impl_
            {
                // HasMemberBegin == true
                template<typename R>
                constexpr auto operator()(R &&r) const
                    noexcept(noexcept(r.begin()))
                {
                    return r.begin();
                }
            };

            template<typename R>
            using impl = impl_<HasMemberBegin<R>>;

        public:
            template<typename R, std::size_t N>
            void operator()(R (&&)[N]) const = delete;

            template<typename R, std::size_t N>
            constexpr R *operator()(R (&array)[N]) const noexcept
            {
                return array;
            }

            template<typename R>
            constexpr auto CPP_fun(operator())(R &&r) (const
                noexcept(noexcept(impl<R>{}((R &&) r)))
                requires (HasMemberBegin<R> || HasNonMemberBegin<R>))
            {
                return impl<R>{}((R &&) r);
            }

#if defined(__cpp_lib_string_view) && __cpp_lib_string_view > 0
            template<class CharT, class Traits>
            constexpr auto operator()(std::basic_string_view<CharT, Traits> r) const noexcept
            {
                return r.begin();
            }
#endif

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
            constexpr auto operator()(std::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) ->
                decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
            constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) ->
                decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }
        };

        // HasMemberBegin == false
        template<>
        struct fn::impl_<false>
        {
            template<typename R>
            constexpr auto operator()(R &&r) const
                noexcept(noexcept(begin((R &&) r)))
            {
                return begin((R &&) r);
            }
        };

        template<typename R>
        using _t = decltype(fn{}(std::declval<R>()));
    }
    /// \endcond

    /// \ingroup group-core
    /// \param r
    /// \return \c r, if \c r is an array. Otherwise, `r.begin()` if that expression is
    ///   well-formed and returns an Iterator. Otherwise, `begin(r)` if that expression
    ///   returns an Iterator.
    RANGES_DEFINE_CPO(_begin_::fn, begin)

    /// \cond
    namespace _end_
    {
        // Poison pill for std::end. (See the detailed discussion at
        // https://github.com/ericniebler/stl2/issues/139)
        template<typename T>
        void end(T &&) = delete;

#ifdef RANGES_WORKAROUND_MSVC_620035
        void end();
#endif

        template<typename T>
        void end(std::initializer_list<T>) = delete;

        template<typename I, typename S>
        auto is_sentinel(S) -> CPP_ret(void)(requires Sentinel<S, I>);

        CPP_def
        (
            template(typename T)
            concept HasMemberEnd,
                requires (T &t)
                (
                    _end_::is_sentinel<_begin_::_t<T>>(t.end())
                ) &&
                std::is_lvalue_reference<T>::value
        );

        CPP_def
        (
            template(typename T)
            concept HasNonMemberEnd,
                requires (T &&t)
                (
                    _end_::is_sentinel<_begin_::_t<T>>(end((T &&) t))
                )
        );

        struct fn
        {
        private:
            template<bool>
            struct impl_
            {
                // HasMemberEnd == true
                template<typename R>
                constexpr auto operator()(R &&r) const
                    noexcept(noexcept(r.end()))
                {
                    return r.end();
                }
            };

            template<typename R>
            using impl = impl_<HasMemberEnd<R>>;

        public:
            template<typename R, std::size_t N>
            void operator()(R (&&)[N]) const = delete;

            template<typename R, std::size_t N>
            constexpr R *operator()(R (&array)[N]) const noexcept
            {
                return array + N;
            }

            template<typename R>
            constexpr auto CPP_fun(operator())(R &&r) (const
                noexcept(noexcept(impl<R>{}((R &&) r)))
                requires (HasMemberEnd<R> || HasNonMemberEnd<R>))
            {
                return impl<R>{}((R &&) r);
            }

#if defined(__cpp_lib_string_view) && __cpp_lib_string_view > 0
            template<class CharT, class Traits>
            constexpr auto operator()(std::basic_string_view<CharT, Traits> r) const noexcept
            {
                return r.end();
            }
#endif

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
            constexpr auto operator()(std::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) ->
                decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
            constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) ->
                decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename Int>
            auto operator-(Int dist) const ->
                CPP_ret(detail::from_end_<meta::_t<std::make_signed<Int>>>)(
                    requires Integral<Int>)
            {
                using SInt = meta::_t<std::make_signed<Int>>;
                RANGES_EXPECT(0 <= dist);
                RANGES_EXPECT(dist <= static_cast<Int>((std::numeric_limits<SInt>::max)()));
                return detail::from_end_<SInt>{-static_cast<SInt>(dist)};
            }
        };

        // HasMemberEnd == false
        template<>
        struct fn::impl_<false>
        {
            template<typename R>
            constexpr auto operator()(R &&r) const
                noexcept(noexcept(end((R &&) r)))
            {
                return end((R &&) r);
            }
        };

        template<typename R>
        using _t = decltype(fn{}(std::declval<R>()));
    }
    /// \endcond

    /// \ingroup group-core
    /// \param r
    /// \return \c r+size(r), if \c r is an array. Otherwise, `r.end()` if that expression is
    ///   well-formed and returns an Iterator. Otherwise, `end(r)` if that expression
    ///   returns an Iterator.
    RANGES_DEFINE_CPO(_end_::fn, end)

    /// \cond
    namespace _cbegin_
    {
        struct fn
        {
            template<typename R>
            constexpr _begin_::_t<detail::as_const_t<R>> operator()(R &&r) const
                noexcept(noexcept(ranges::begin(detail::as_const((R &&) r))))
            {
                return ranges::begin(detail::as_const((R &&) r));
            }
        };
    }
    /// \endcond

    /// \ingroup group-core
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
            constexpr _end_::_t<detail::as_const_t<R>> operator()(R &&r) const
                noexcept(noexcept(ranges::end(detail::as_const((R &&) r))))
            {
                return ranges::end(detail::as_const((R &&) r));
            }
        };
    }
    /// \endcond

    /// \ingroup group-core
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

        CPP_def
        (
            template(typename T)
            concept HasMemberRBegin,
                requires (T &t)
                (
                    _begin_::is_iterator(t.rbegin())
                ) &&
                std::is_lvalue_reference<T>::value
        );

        CPP_def
        (
            template(typename T)
            concept HasNonMemberRBegin,
                requires (T &&t)
                (
                    _begin_::is_iterator(rbegin((T &&) t))
                )
        );

        CPP_def
        (
            template(typename T)
            concept CanReverseEnd,
                requires (T &&t)
                (
                    // make_reverse_iterator is constrained with
                    // BidirectionalIterator.
                    ranges::make_reverse_iterator(ranges::end((T &&) t))
                ) &&
                Same<_begin_::_t<T>, _end_::_t<T>>
        );

        struct fn
        {
        private:
            // HasMemberRBegin == true
            template<int>
            struct impl_
            {
                template<typename R>
                constexpr auto operator()(R &&r) const
                    noexcept(noexcept(r.rbegin()))
                {
                    return r.rbegin();
                }
            };

            template<typename R>
            using impl = impl_<HasMemberRBegin<R> ? 0 : HasNonMemberRBegin<R> ? 1 : 2>;

        public:
            template<typename R>
            constexpr auto CPP_fun(operator())(R &&r) (const
                noexcept(noexcept(impl<R>{}((R &&) r)))
                requires (HasMemberRBegin<R> || HasNonMemberRBegin<R> || CanReverseEnd<R>))
            {
                return impl<R>{}((R &&) r);
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
            constexpr auto operator()(std::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) ->
                decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
            constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) ->
                decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }
        };

        // HasNonMemberRBegin == true
        template<>
        struct fn::impl_<1>
        {
            template<typename R>
            constexpr auto operator()(R &&r) const
                noexcept(noexcept(rbegin((R &&) r)))
            {
                return rbegin((R &&) r);
            }
        };

        // CanReverseEnd
        template<>
        struct fn::impl_<2>
        {
            template<typename R>
            constexpr auto operator()(R &&r) const
                noexcept(noexcept(ranges::make_reverse_iterator(ranges::end((R &&) r))))
            {
                return ranges::make_reverse_iterator(ranges::end((R &&) r));
            }
        };

        template<typename R>
        using _t = decltype(fn{}(std::declval<R>()));
    }
    /// \endcond

    /// \ingroup group-core
    /// \param r
    /// \return `make_reverse_iterator(r+size(r))` if r is an array. Otherwise,
    ///   `r.rbegin()` if that expression is well-formed and returns an Iterator.
    ///   Otherwise, `make_reverse_iterator(ranges::end(r))` if `ranges::begin(r)`
    ///   and `ranges::end(r)` are both well-formed and have the same type that
    ///   satisfies BidirectionalIterator.
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

        CPP_def
        (
            template(typename T)
            concept HasMemberREnd,
                requires (T &t)
                (
                    _end_::is_sentinel<_rbegin_::_t<T &>>(t.rend())
                ) &&
                std::is_lvalue_reference<T>::value
        );

        CPP_def
        (
            template(typename T)
            concept HasNonMemberREnd,
                requires (T &&t)
                (
                    _end_::is_sentinel<_rbegin_::_t<T &>>(rend((T &&) t))
                )
        );

        CPP_def
        (
            template(typename T)
            concept CanReverseBegin,
                requires (T &&t)
                (
                    // make_reverse_iterator is constrained with
                    // BidirectionalIterator.
                    ranges::make_reverse_iterator(ranges::begin((T &&) t))
                ) &&
                Same<_begin_::_t<T>, _end_::_t<T>>
        );

        struct fn
        {
        private:
            // HasMemberRBegin == true
            template<int>
            struct impl_
            {
                template<typename R>
                constexpr auto operator()(R &&r) const
                    noexcept(noexcept(r.rend()))
                {
                    return r.rend();
                }
            };

            template<typename R>
            using impl = impl_<HasMemberREnd<R> ? 0 : HasNonMemberREnd<R> ? 1 : 2>;

        public:
            template<typename R>
            constexpr auto CPP_fun(operator())(R &&r) (const
                noexcept(noexcept(impl<R>{}((R &&) r)))
                requires (HasMemberREnd<R> || HasNonMemberREnd<R> || CanReverseBegin<R>))
            {
                return impl<R>{}((R &&) r);
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
            constexpr auto operator()(std::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) ->
                decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }

            template<typename T, typename Fn = fn>
            RANGES_DEPRECATED("Using a reference_wrapper as a Range is deprecated. Use view::ref instead.")
            constexpr auto operator()(ranges::reference_wrapper<T> ref) const
                noexcept(noexcept(Fn{}(ref.get()))) ->
                decltype(Fn{}(ref.get()))
            {
                return Fn{}(ref.get());
            }
        };

        // HasNonMemberREnd == true
        template<>
        struct fn::impl_<1>
        {
            template<typename R>
            constexpr auto operator()(R &&r) const
                noexcept(noexcept(rend((R &&) r)))
            {
                return rend((R &&) r);
            }
        };

        // CanReverseBegin
        template<>
        struct fn::impl_<2>
        {
            template<typename R>
            constexpr auto operator()(R &&r) const
                noexcept(noexcept(ranges::make_reverse_iterator(ranges::begin((R &&) r))))
            {
                return ranges::make_reverse_iterator(ranges::begin((R &&) r));
            }
        };

        template<typename R>
        using _t = decltype(fn{}(std::declval<R>()));
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
    RANGES_DEFINE_CPO(_rend_::fn, rend)

    /// \cond
    namespace _crbegin_
    {
        struct fn
        {
            template<typename R>
            constexpr _rbegin_::_t<detail::as_const_t<R>> operator()(R &&r) const
                noexcept(noexcept(ranges::rbegin(detail::as_const((R &&) r))))
            {
                return ranges::rbegin(detail::as_const((R &&) r));
            }
        };
    }
    /// \endcond

    /// \ingroup group-core
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
            constexpr _rend_::_t<detail::as_const_t<R>> operator()(R &&r) const
                noexcept(noexcept(ranges::rend(detail::as_const((R &&) r))))
            {
                return ranges::rend(detail::as_const((R &&) r));
            }
        };
    }
    /// \endcond

    /// \ingroup group-core
    /// \param r
    /// \return The result of calling `ranges::rend` with a const-qualified
    ///    reference to r.
    RANGES_INLINE_VARIABLE(_crend_::fn, crend)
}

#endif
