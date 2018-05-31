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
                template<typename R, std::size_t N>
                static constexpr std::size_t impl_(R (&)[N], int) noexcept
                {
                    return N;
                }

                // Prefer member if it returns Integral.
                template<typename R,
                    typename = meta::if_c<!disable_sized_range<R>()>,
                    typename N = decltype(aux::copy(std::declval<R &>().size())),
                    CONCEPT_REQUIRES_(Integral<N>())>
                static constexpr N impl_(R &r, int)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    r.size()
                )

                // Use ADL if it returns Integral.
                template<typename R,
                    typename = meta::if_c<!disable_sized_range<R>()>,
                    typename N = decltype(aux::copy(size(std::declval<R &>()))),
                    CONCEPT_REQUIRES_(Integral<N>())>
                static constexpr N impl_(R &r, long)
                RANGES_AUTO_RETURN_NOEXCEPT
                (
                    size(r)
                )

                template<typename R, typename I = decltype(ranges::cbegin(std::declval<R &>())),
                    CONCEPT_REQUIRES_(ForwardIterator<I>())>
                static RANGES_CXX14_CONSTEXPR auto impl_(R &r, ...)
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    ranges::iter_size(ranges::cbegin(r), ranges::cend(r))
                )

            public:
                using iter_size_fn::operator();

                template<typename R>
                constexpr auto operator()(R &&r) const
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
        /// \return The result of an unqualified call to `size`
        inline namespace CPOs
        {
            RANGES_INLINE_VARIABLE(_size_::fn, size)
        }
    }
}

#endif
