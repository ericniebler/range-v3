/// \file
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

#ifndef RANGES_V3_UTILITY_INTEGER_SEQUENCE_HPP
#define RANGES_V3_UTILITY_INTEGER_SEQUENCE_HPP

#include <cstddef>
#include <type_traits>
#include <range/v3/detail/config.hpp>
#include <range/v3/utility/meta.hpp>

namespace ranges
{
    inline namespace v3
    {

    using meta::integer_sequence;
    using meta::make_index_sequence;
    using meta::index_sequence;

    }
}

#endif
