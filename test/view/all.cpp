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

    iterator_range<int *> x = view::all(rgi);
    iterator_range<std::vector<int>::iterator> y = view::all(vi);
    sized_iterator_range<std::list<int>::iterator> z = view::all(li);

    x = view::all(x);
    y = view::all(y);
    z = view::all(z);

    CHECK(x.size() == 7u);
    CHECK(y.size() == 7u);
    CHECK(z.size() == 7u);

    ranges::reference_wrapper<int[7]> rrgi = view::all(std::ref(rgi));
    auto stdref = std::ref(rgi);
    rrgi = view::all(stdref);
    rrgi = view::all(static_cast<std::reference_wrapper<int[7]> const &>(stdref));

    {
        auto v = view::all(debug_input_view<int const>{rgi});
        CHECK(v.size() == size(rgi));
        CHECK(v.data_ == rgi);
        auto v2 = view::all(view::all(view::all(std::move(v))));
        CONCEPT_ASSERT(Same<decltype(v), decltype(v2)>());
        CHECK(!v.valid_);
        CHECK(v2.size() == size(rgi));
        CHECK(v2.data_ == rgi);
    }

    return test_result();
}
