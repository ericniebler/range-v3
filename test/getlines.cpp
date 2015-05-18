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

#include <sstream>
#include <range/v3/core.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

int main()
{
    std::stringstream sin{
R"(Now is
the time
for all
good men
)"};

    ::check_equal(ranges::getlines(sin), {"Now is", "the time", "for all", "good men"});

    using Rng = decltype(ranges::getlines(sin));
    CONCEPT_ASSERT(ranges::InputView<Rng>());
    CONCEPT_ASSERT(!ranges::ForwardView<Rng>());

    return ::test_result();
}
