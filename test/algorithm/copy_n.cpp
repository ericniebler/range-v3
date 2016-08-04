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

#include <iterator>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/copy_n.hpp>
#include "../simple_test.hpp"

int main()
{
    using ranges::begin;
    using ranges::end;
    using ranges::size;

    {
      std::istringstream iss{ "1 2 3 4 5" };
      int output[] = { 0, 0, 0, 0 };
      auto const p = ranges::copy_n(std::istream_iterator<int>{iss}, 2, &output[0]);
      ranges::copy_n(std::istream_iterator<int>{iss}, 2, p.second);
      int const expected[] = { 1, 2, 3, 4 };
      CHECK(output[0] == expected[0]);
      CHECK(output[1] == expected[1]);
      CHECK(output[2] == expected[2]);
      CHECK(output[3] == expected[3]);
    }

    return test_result();
}
