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
            constexpr auto size_(Rng && rng, long) ->
                decltype(iter_size(begin(rng), end(rng)))
            {
                return iter_size(begin(rng), end(rng));
            }

            template<typename Rng>
            constexpr
            auto size_(Rng && rng, int) ->
                decltype(rng.size())
            {
                return rng.size();
            }

            template<typename Rng>
            constexpr
            auto size(Rng && rng) ->
                decltype(adl_size_detail::size_(std::forward<Rng>(rng), 42))
            {
                return adl_size_detail::size_(std::forward<Rng>(rng), 42);
            }

            // A reference-wrapped Range
            template<typename T>
            auto size(std::reference_wrapper<T> t) -> decltype(size(t.get()))
            {
                return size(t.get());
            }

            template<typename T, bool RValue>
            constexpr
            auto size(ranges::reference_wrapper<T, RValue> t) -> decltype(size(t.get()))
            {
                return size(t.get());
            }

            // Built-in arrays
            template<typename T, std::size_t N>
            constexpr std::size_t size(T (&)[N])
            {
                return N;
            }

            template<typename T, std::size_t N>
            constexpr std::size_t size(T const (&)[N])
            {
                return N;
            }

            template<typename T, std::size_t N>
            constexpr std::size_t size(T (&&)[N])
            {
                return N;
            }

            struct size_fn : iter_size_fn
            {
                using iter_size_fn::operator();

                template<typename Rng>
                constexpr auto operator()(Rng &&rng) const ->
                    decltype(size(detail::forward<Rng>(rng)))
                {
                    return size(detail::forward<Rng>(rng));
                }
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \return The result of an unqualified call to `size`
        namespace
        {
            constexpr auto&& size = static_const<adl_size_detail::size_fn>::value;
        }
    }
}

#endif
