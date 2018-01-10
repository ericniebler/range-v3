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

#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/range_traits.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

using namespace ranges;

int main()
{
    const char* text =
R"(Now is
the time
for all
good men
)";

    std::stringstream sin{text};
    auto rng = getlines(sin);
    ::check_equal(rng, {"Now is", "the time", "for all", "good men"});

    using Rng = decltype(rng);
    CONCEPT_ASSERT(InputView<Rng>());
    CONCEPT_ASSERT(!ForwardView<Rng>());
    CONCEPT_ASSERT(Same<range_rvalue_reference_t<Rng>, std::string &&>());

    return ::test_result();
}
