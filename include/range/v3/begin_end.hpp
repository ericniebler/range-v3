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
#if defined(__cpp_lib_string_view) && __cpp_lib_string_view > 0
#include <string_view>
#endif

#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/dangling.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/utility/iterator.hpp> // for ranges::reverse_iterator
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
            void begin(T &&) = delete;
            template<typename T>
            void begin(std::initializer_list<T> &&) = delete;

            struct fn
            {
            private:
                template<typename R>
                using member_begin_t =
                    detail::decay_t<decltype(static_cast<R (*)()>(nullptr)().begin())>;
                template<typename R>
                using non_member_begin_t =
                    detail::decay_t<decltype(begin(static_cast<R (*)()>(nullptr)()))>;

            public:
                template<typename R, std::size_t N>
                void operator()(R (&&)[N]) const = delete;

                template<typename R, std::size_t N>
                constexpr R *operator()(R (&array)[N]) const noexcept
                {
                    return array;
                }

                // Prefer member if it returns Iterator.
                template<typename R>
                constexpr auto operator()(R &r) const
                    // TODO: noexcept and guaranteed copy elision
                    noexcept(noexcept(static_cast<member_begin_t<R &>>(r.begin()))) ->
                    CPP_ret(member_begin_t<R &>)(
                        requires Iterator<member_begin_t<R &>>)
                {
                    return r.begin();
                }

                // Use ADL if it returns Iterator.
                template<typename R>
                constexpr auto operator()(R &&r) const volatile
                    noexcept(noexcept(static_cast<non_member_begin_t<R>>(begin((R &&) r)))) ->
                    CPP_ret(non_member_begin_t<R>)(
                        requires Iterator<non_member_begin_t<R>>)
                {
                    return begin((R &&) r);
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

            template<typename R>
            using _t = decltype(fn{}(static_cast<R (*)()>(nullptr)()));
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
            template<typename T>
            void end(std::initializer_list<T> &&) = delete;

            struct fn
            {
            private:
                template<typename R>
                using member_end_t =
                    detail::decay_t<decltype(static_cast<R (*)()>(nullptr)().end())>;
                template<typename R>
                using non_member_end_t =
                    detail::decay_t<decltype(end(static_cast<R (*)()>(nullptr)()))>;

            public:
                template<typename R, std::size_t N>
                void operator()(R (&&)[N]) const = delete;

                template<typename R, std::size_t N>
                constexpr R *operator()(R (&array)[N]) const noexcept
                {
                    return array + N;
                }

                // Prefer member if it returns Sentinel.
                template<typename R>
                constexpr auto operator()(R &r) const
                    noexcept(noexcept(static_cast<member_end_t<R &>>(r.end()))) ->
                    CPP_ret(member_end_t<R &>)(
                        requires Sentinel<member_end_t<R &>, _begin_::_t<R &>>)
                {
                    return r.end();
                }

                // Use ADL if it returns Sentinel.
                template<typename R>
                constexpr auto operator()(R &&r) const volatile
                    noexcept(noexcept(static_cast<non_member_end_t<R>>(end((R &&) r)))) ->
                    CPP_ret(non_member_end_t<R>)(
                        requires Sentinel<non_member_end_t<R>, _begin_::_t<R>>)
                {
                    return end((R &&) r);
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
            };

            template<typename R>
            using _t = decltype(fn{}(static_cast<R (*)()>(nullptr)()));
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
                constexpr _begin_::_t<R const &> operator()(R const &r) const
                    noexcept(noexcept(ranges::begin(r)))
                {
                    return ranges::begin(r);
                }

                template<typename R>
                constexpr _begin_::_t<R const> operator()(R const &&r) const
                    noexcept(noexcept(ranges::begin((R const &&) r)))
                {
                    return ranges::begin((R const &&) r);
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
                constexpr _end_::_t<R const &> operator()(R const &r) const
                    noexcept(noexcept(ranges::end(r)))
                {
                    return ranges::end(r);
                }
                template<typename R>
                constexpr _end_::_t<R const> operator()(R const &&r) const
                    noexcept(noexcept(ranges::end((R const &&) r)))
                {
                    return ranges::end((R const &&) r);
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
            template<typename T>
            void rbegin(std::initializer_list<T>) = delete;

            struct fn
            {
            private:
                template<typename R>
                using member_rbegin_t =
                    detail::decay_t<decltype(static_cast<R (*)()>(nullptr)().rbegin())>;
                template<typename R>
                using non_member_rbegin_t =
                    detail::decay_t<decltype(rbegin(static_cast<R (*)()>(nullptr)()))>;

                template<typename R>
                static constexpr auto impl_(R &&r, int)
                    noexcept(noexcept(static_cast<non_member_rbegin_t<R>>(rbegin((R &&) r)))) ->
                    CPP_ret(non_member_rbegin_t<R>)(
                        requires Iterator<non_member_rbegin_t<R>>)
                {
                    return rbegin((R &&) r);
                }

                template<typename R>
                static constexpr auto impl_(R &&r, long)
                    noexcept(noexcept(ranges::make_reverse_iterator(ranges::end((R &&) r)))) ->
                    CPP_ret(reverse_iterator<_end_::_t<R>>)(
                        requires Same<_begin_::_t<R>, _end_::_t<R>> &&
                            BidirectionalIterator<_end_::_t<R>>)
                {
                    return ranges::make_reverse_iterator(ranges::end(r));
                }

            public:
                // Prefer member if it returns Iterator.
                template<typename R>
                constexpr auto operator()(R &r) const
                    noexcept(noexcept(static_cast<member_rbegin_t<R &>>(r.rbegin()))) ->
                    CPP_ret(member_rbegin_t<R &>)(
                        requires Iterator<member_rbegin_t<R &>>)
                {
                    return r.rbegin();
                }

                template<typename R>
                constexpr auto operator()(R &&r) const volatile
                    noexcept(noexcept(fn::impl_((R &&) r, 0))) ->
                    decltype(fn::impl_((R &&) r, 0))
                {
                    return fn::impl_((R &&) r, 0);
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

            template<typename R>
            using _t = decltype(fn{}(static_cast<R (*)()>(nullptr)()));
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
            template<typename T>
            void rend(std::initializer_list<T>) = delete;

            struct fn
            {
            private:
                template<typename R>
                using member_rend_t =
                    detail::decay_t<decltype(static_cast<R (*)()>(nullptr)().rend())>;
                template<typename R>
                using non_member_rend_t =
                    detail::decay_t<decltype(rend(static_cast<R (*)()>(nullptr)()))>;

                template<typename R>
                static constexpr auto impl_(R &&r, int)
                    noexcept(noexcept(static_cast<non_member_rend_t<R>>(rend((R &&) r)))) ->
                    CPP_ret(non_member_rend_t<R>)(
                        requires Sentinel<non_member_rend_t<R>, _rbegin_::_t<R>>)
                {
                    return rend((R &&) r);
                }

                template<typename R>
                static constexpr auto impl_(R &&r, long)
                    noexcept(noexcept(ranges::make_reverse_iterator(ranges::begin((R &&) r)))) ->
                    CPP_ret(reverse_iterator<_begin_::_t<R>>)(
                        requires Same<_begin_::_t<R>, _end_::_t<R>> &&
                            BidirectionalIterator<_begin_::_t<R>>)
                {
                    return ranges::make_reverse_iterator(ranges::begin(r));
                }

            public:
                // Prefer member if it returns Iterator.
                template<typename R>
                constexpr auto operator()(R &r) const
                    noexcept(noexcept(static_cast<member_rend_t<R &>>(r.rend()))) ->
                    CPP_ret(member_rend_t<R &>)(
                        requires Sentinel<member_rend_t<R &>, _rbegin_::_t<R &>>)
                {
                    return r.rend();
                }

                template<typename R>
                constexpr auto operator()(R &&r) const volatile
                    noexcept(noexcept(fn::impl_((R &&) r, 0))) ->
                    decltype(fn::impl_((R &&) r, 0))
                {
                    return fn::impl_((R &&) r, 0);
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

            template<typename R>
            using _t = decltype(fn{}(static_cast<R (*)()>(nullptr)()));
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
                constexpr _rbegin_::_t<R const &> operator()(R const &r) const
                    noexcept(noexcept(ranges::rbegin(r)))
                {
                    return ranges::rbegin(r);
                }
                template<typename R>
                constexpr _rbegin_::_t<R const> operator()(R const &&r) const
                    noexcept(noexcept(ranges::rbegin((R const &&) r)))
                {
                    return ranges::rbegin((R const &&) r);
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
                constexpr _rend_::_t<R const &> operator()(R const &r) const
                    noexcept(noexcept(ranges::rend(r)))
                {
                    return ranges::rend(r);
                }
                template<typename R>
                constexpr _rend_::_t<R const> operator()(R const &&r) const
                    noexcept(noexcept(ranges::rend((R const &&) r)))
                {
                    return ranges::rend((R const &&) r);
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
}

#endif
