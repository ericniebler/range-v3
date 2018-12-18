/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_SIZE_HPP
#define RANGES_V3_SIZE_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-concepts
        // Specialize this if the default is wrong.
        template<typename T>
        constexpr bool disable_sized_range = false;

        /// \cond
        namespace _size_
        {
            template<typename T>
            void size(T &&) = delete;

#ifdef RANGES_WORKAROUND_MSVC_620035
            void size();
#endif

            struct fn
            {
            private:
                template<typename R>
                using member_size_t = decltype(+(std::declval<R>()).size());
                template<typename R>
                using non_member_size_t = decltype(+size(std::declval<R>()));

                template<typename R, std::size_t N>
                static constexpr std::size_t impl_(R (&)[N], int) noexcept
                {
                    return N;
                }

                template<typename R, std::size_t N>
                static constexpr std::size_t impl_(R (&&)[N], int) noexcept
                {
                    return N;
                }

                // Prefer member if it returns Integral.
                template<typename R>
                static constexpr auto impl_(R &&r, int) noexcept(noexcept(((R &&) r).size())) ->
                    CPP_ret(member_size_t<R>)(
                        requires Integral<member_size_t<R>> &&
                            !disable_sized_range<uncvref_t<R>>)
                {
                    return ((R &&) r).size();
                }

                // Use ADL if it returns Integral.
                template<typename R>
                static constexpr auto impl_(R &&r, long) noexcept(noexcept(size((R &&) r))) ->
                    CPP_ret(non_member_size_t<R>)(
                        requires Integral<non_member_size_t<R>> &&
                            !disable_sized_range<uncvref_t<R>>)
                {
                    return size((R &&) r);
                }

                template<typename R>
                static constexpr /*c++14*/ auto impl_(R &&r, ...) ->
                    CPP_ret(meta::_t<std::make_unsigned<iter_difference_t<_begin_::_t<R>>>>)(
                        requires ForwardIterator<_begin_::_t<R>> &&
                            SizedSentinel<_end_::_t<R>, _begin_::_t<R>>)
                {
                    using size_type = meta::_t<std::make_unsigned<iter_difference_t<_begin_::_t<R>>>>;
                    return static_cast<size_type>(ranges::end((R &&) r) - ranges::begin((R &&) r));
                }

            public:
                template<typename R>
                constexpr auto operator()(R &&r) const
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
        }
        /// \endcond

        /// \ingroup group-core
        /// \return For a given expression `E` of type `T`, `ranges::size(E)` is equivalent to:
        ///   * `+extent_v<T>` if `T` is an array type.
        ///   * Otherwise, `+E.size()` if it is a valid expression and its type `I` models
        ///     `Integral` and `disable_sized_range<std::remove_cvref_t<T>>` is false.
        ///   * Otherwise, `+size(E)` if it is a valid expression and its type `I` models
        ///     `Integral` with overload resolution performed in a context that includes the
        ///     declaration:
        ///     \code
        ///     template<class T> void size(T&&) = delete;
        ///     \endcode
        ///     and does not include a declaration of `ranges::size`, and
        ///     `disable_sized_range<std::remove_cvref_t<T>>` is false.
        ///   * Otherwise, `static_cast<U>(ranges::end(E) - ranges::begin(E))` where `U` is
        ///     `std::make_unsigned_t<iter_difference_t<iterator_t<T>>>`, except that `E` is
        ///     evaluated once, if it is a valid expression and the types `I` and `S` of
        ///     `ranges::begin(E)` and `ranges::end(E)` model `SizedSentinel<S, I>` and
        ///     `ForwardIterator<I>`.
        ///   * Otherwise, `ranges::size(E)` is ill-formed.
        RANGES_DEFINE_CPO(_size_::fn, size)
    }
}

#endif
