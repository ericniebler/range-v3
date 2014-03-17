// Boost.Range library
//
//  Copyright Eric Niebler 2013.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#include <vector>
#include <iostream>
#include <range/v3/range.hpp>

void test_filter()
{
    using namespace ranges;
    std::cout << "\ntest filter\n";
    std::stringstream sin("1 2 3 4 5 6 7 8 9");
    auto ints = istream<int>(sin);
    auto evens = ints | view::filter([](int i){return i % 2 == 0; });
    using evens_t = decltype(evens);
    range_iterator_t<evens_t> i = evens.begin();
    range_iterator_t<evens_t const> j = evens.begin();
    j = i;
    range_sentinel_t<evens_t> k = evens.end();
    range_sentinel_t<evens_t const> l = evens.end();
    l = k;
    for (; j != evens.end(); ++j)
        std::cout << *j << ' ';
    std::cout << '\n';
}

int main()
{
    using namespace ranges;
    std::vector<int> v {1,2,3,4,5};

    for_each(v, [](int i){ std::cout << i << ' '; });
    std::cout << '\n';

    auto t = view::transform(v, [](int i){return i * i;});
    for_each(t, [](int i){ std::cout << i << ' '; });
    std::cout << '\n';
}
