///////////////////////////////////////////////////////////////////////////////
/// \file push_back.hpp
///   Contains range-based  non-member version of the STL containers' push_back
///   member function.
//
// Copyright 2004 Eric Niebler.
// Copyright 2014 Gonzalo Brito Gadeschi.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef RANGES_V3_EXTENSION_PUSH_BACK_HPP
#define RANGES_V3_EXTENSION_PUSH_BACK_HPP

#include <range/v3/extension/insert.hpp>

namespace ranges
{
    inline namespace v3
    {
        struct push_back_fn
        {
            template <typename I, typename S, typename O,
                      typename OIt = decltype(end(std::declval<O>())),
                      CONCEPT_REQUIRES_(Insertable<O, OIt, I, S>())>
            auto operator()(O &to, I &&from_begin, S &&from_end) const
                RANGE_RETURNS(insert(to, end(to), std::forward<I>(from_begin),
                                     std::forward<S>(from_end)));

            template <typename I, typename O,
                      typename OIt = decltype(end(std::declval<O>())),
                      CONCEPT_REQUIRES_(Insertable<O, OIt, I>())>
            auto operator()(O &to, I &&from) const
                RANGE_RETURNS(insert(to, end(to), std::forward<I>(from)));
        };

        RANGES_CONSTEXPR push_back_fn push_back{};
    }
}

#endif
