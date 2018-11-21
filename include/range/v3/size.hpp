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
            int size(T &&) = delete;

            struct fn
            {
            private:
                template<typename R>
                using begin_t = decltype(ranges::begin(static_cast<R(*)()>(nullptr)()));
                template<typename R>
                using end_t = decltype(ranges::end(static_cast<R(*)()>(nullptr)()));
                template<typename R>
                using member_size_t = decltype(+(static_cast<R(*)()>(nullptr)()).size());
                template<typename R>
                using non_member_size_t = decltype(+size(static_cast<R(*)()>(nullptr)()));

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
                    CPP_ret(meta::_t<std::make_unsigned<iter_difference_t<begin_t<R>>>>)(
                        requires ForwardIterator<begin_t<R>> &&
                            SizedSentinel<end_t<R>, begin_t<R>>)
                {
                    using size_type = meta::_t<std::make_unsigned<iter_difference_t<begin_t<R>>>>;
                    return static_cast<size_type>(ranges::end((R &&) r) - ranges::begin((R &&) r));
                }

            public:
                template<typename R>
                constexpr auto CPP_auto_fun(operator())(R &&r) (const)
                (
                    return fn::impl_(static_cast<R &&>(r), 42)
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
        /// \return The result of an unqualified call to `size`
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_size_::fn, size)
        }
    }
}

#endif
