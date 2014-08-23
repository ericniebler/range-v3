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
    std::vector<int> vi;
    std::list<int> li;

    iterator_range<int *> x = view::all(rgi);
    iterator_range<std::vector<int>::iterator> y = view::all(vi);
    sized_iterator_range<std::list<int>::iterator> z = view::all(li);

    x = view::all(x);
    y = view::all(y);
    z = view::all(z);

    return test_result();
}
