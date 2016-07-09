/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
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
        /// \cond
        namespace adl_size_detail
        {
            template<typename Rng>
            constexpr auto impl(Rng && rng, ...)
            RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
            (
                iter_size(begin(rng), end(rng))
            )
            template<typename Rng,
                typename Result = detail::decay_t<decltype(size(std::declval<Rng&>()))>,
                CONCEPT_REQUIRES_(Integral<Result>())>
            constexpr Result impl(Rng && rng, long)
                noexcept(noexcept(size(rng)))
            {
                return size(rng);
            }
            template<typename Rng,
                typename Result = detail::decay_t<decltype(std::declval<Rng&>().size())>,
                CONCEPT_REQUIRES_(Integral<Result>())>
            constexpr Result impl(Rng && rng, int)
                noexcept(noexcept(rng.size()))
            {
                return rng.size();
            }

            struct size_fn : iter_size_fn
            {
                using iter_size_fn::operator();

                // Built-in arrays
                template<typename T, std::size_t N>
                constexpr std::size_t operator()(T (&)[N]) const noexcept
                {
                    return N;
                }
                template<typename T, std::size_t N>
                constexpr std::size_t operator()(T const (&)[N]) const noexcept
                {
                    return N;
                }
                template<typename T, std::size_t N>
                constexpr std::size_t operator()(T (&&)[N]) const noexcept
                {
                    return N;
                }

                // A reference-wrapped Range
                template<typename T>
                constexpr auto operator()(std::reference_wrapper<T> t) const
                    noexcept(noexcept(std::declval<const size_fn&>()(t.get()))) ->
                    decltype(std::declval<const size_fn&>()(t.get()))
                {
                    return (*this)(t.get());
                }
                template<typename T, bool RValue>
                constexpr auto operator()(ranges::reference_wrapper<T, RValue> t) const
                    noexcept(noexcept(std::declval<const size_fn&>()(t.get()))) ->
                    decltype(std::declval<const size_fn&>()(t.get()))
                {
                    return (*this)(t.get());
                }

                // Other
                template<typename Rng>
                constexpr auto operator()(Rng && rng) const
                RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
                (
                    adl_size_detail::impl(detail::forward<Rng>(rng), 42)
                )
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \return The result of an unqualified call to `size`
        RANGES_INLINE_VARIABLE(adl_size_detail::size_fn, size)
    }
}

#endif
