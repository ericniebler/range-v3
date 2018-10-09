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
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-concepts
        // Specialize this if the default is wrong.
        template<typename T>
        struct disable_sized_range : std::false_type {};

        /// \cond
        namespace _size_
        {
            template<typename T>
            void size(T const &) = delete;

            struct fn : iter_size_fn
            {
            private:
                template<typename R>
                using cbegin_t = decltype(ranges::cbegin(*(R*)nullptr));
                template<typename R>
                using cend_t = decltype(ranges::cend(*(R*)nullptr));
                template<typename R>
                using member_size_t = detail::decay_t<decltype((*(R*)nullptr).size())>;
                template<typename R>
                using non_member_size_t = detail::decay_t<decltype(size((*(R*)nullptr)))>;

                template<typename R, std::size_t N>
                static constexpr std::size_t impl_(R (&)[N], int) noexcept
                {
                    return N;
                }

                // Prefer member if it returns Integral.
                template<typename R>
                static constexpr auto impl_(R &r, int) noexcept(noexcept(r.size())) ->
                    CPP_ret(member_size_t<R>)(
                        requires Integral<member_size_t<R>> && !disable_sized_range<R>::value)
                {
                    return r.size();
                }

                // Use ADL if it returns Integral.
                template<typename R>
                static constexpr auto impl_(R &r, int) noexcept(noexcept(size(r))) ->
                    CPP_ret(non_member_size_t<R>)(
                        requires Integral<non_member_size_t<R>> && !disable_sized_range<R>::value)
                {
                    return size(r);
                }

                template<typename R>
                static RANGES_CXX14_CONSTEXPR auto impl_(R &r, ...) ->
                    CPP_ret(size_type_t<cbegin_t<R>>)(
                        requires ForwardIterator<cbegin_t<R>> &&
                            SizedSentinel<cend_t<R>, cbegin_t<R>>)
                {
                    return ranges::iter_size(ranges::cbegin(r), ranges::cend(r));
                }

            public:
                using iter_size_fn::operator();

                template<typename R>
                constexpr auto CPP_auto_fun(operator())(R &&r) (const)
                (
                    return fn::impl_(r, 42)
                )

                template<typename T, typename Fn = fn>
                constexpr auto CPP_auto_fun(operator())(std::reference_wrapper<T> ref) (const)
                (
                    return Fn()(ref.get())
                )

                template<typename T, typename Fn = fn>
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
