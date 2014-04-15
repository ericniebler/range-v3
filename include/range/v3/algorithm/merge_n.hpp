//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
// Copyright (c) 2009 Alexander Stepanov and Paul McJones
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. The authors make no
// representations about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.
//
// Algorithms from
// Elements of Programming
// by Alexander Stepanov and Paul McJones
// Addison-Wesley Professional, 2009
#ifndef RANGES_V3_ALGORITHM_MERGE_N_HPP
#define RANGES_V3_ALGORITHM_MERGE_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/algorithm/detail/combine_copy_n.hpp>

namespace ranges
{
    inline namespace v3
    {
        using merge_n_fn = combine_copy_n_fn;
        RANGES_CONSTEXPR merge_n_fn merge_n{};
    } // namespace v3
} // namespace ranges

#endif // include guard
