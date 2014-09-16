//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <list>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/all.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {1, 1, 1, 2, 3, 4, 4};
    std::vector<int> vi(begin(rgi), end(rgi));
    std::list<int> li(begin(rgi), end(rgi));

    range<int *> x = view::all(rgi);
    range<std::vector<int>::iterator> y = view::all(vi);
    sized_range<std::list<int>::iterator> z = view::all(li);

    x = view::all(x);
    y = view::all(y);
    z = view::all(z);

    CHECK(x.size() == 7u);
    CHECK(y.size() == 7u);
    CHECK(z.size() == 7u);

    return test_result();
}
