// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
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

#include <iterator>
#include <functional>
#include <initializer_list>
#include <range/v3/range_fwd.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace adl_begin_end_detail
        {
            using std::begin;
            using std::end;

            // A reference-wrapped Iterable is an Iterable
            template<typename T>
            auto begin(std::reference_wrapper<T> ref) -> decltype(begin(ref.get()))
            {
                return begin(ref.get());
            }

            template<typename T>
            auto end(std::reference_wrapper<T> ref) -> decltype(end(ref.get()))
            {
                return end(ref.get());
            }

            template<typename T>
            auto begin(ranges::reference_wrapper<T> ref) -> decltype(begin(ref.get()))
            {
                return begin(ref.get());
            }

            template<typename T>
            auto end(ranges::reference_wrapper<T> ref) -> decltype(end(ref.get()))
            {
                return end(ref.get());
            }

            // Handle char*, std::pair<iterator, iterator>, std::pair<interator, int> ?

            struct begin_fn
            {
                template<typename Rng>
                auto operator()(Rng && rng) const ->
                    decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct end_fn
            {
                template<typename Rng>
                auto operator()(Rng && rng) const ->
                    decltype(end(rng))
                {
                    return end(rng);
                }
            };

            struct cbegin_fn
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const ->
                    decltype(begin(rng))
                {
                    return begin(rng);
                }
            };

            struct cend_fn
            {
                template<typename Rng>
                auto operator()(Rng const & rng) const ->
                    decltype(end(rng))
                {
                    return end(rng);
                }
            };
        }
        /// \endcond

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `begin` free function
        constexpr begin_fn begin {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `end` free function
        constexpr end_fn end {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `begin` free function
        /// with a const-qualified argument.
        constexpr cbegin_fn cbegin {};

        /// \ingroup group-core
        /// \return The result of an unqualified call to the `end` free function
        /// with a const-qualified argument.
        constexpr cend_fn cend {};
    }
}

#endif
