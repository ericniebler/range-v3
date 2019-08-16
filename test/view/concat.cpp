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

#include <array>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/generate.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/take_while.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<std::string> his_face{"this", "is", "his", "face"};
    std::vector<std::string> another_mess{"another", "fine", "mess"};
    auto joined = views::concat(his_face, another_mess);
    CPP_assert(view_<decltype(joined)>);
    CPP_assert(random_access_range<decltype(joined)>);
    static_assert(std::is_same<range_reference_t<decltype(joined)>, std::string &>::value, "");
    CHECK(joined.size() == 7u);
    CHECK((joined.end() - joined.begin()) == 7);
    ::check_equal(joined | views::reverse, {"mess", "fine", "another", "face", "his", "is", "this"});

    auto revjoin = joined | views::reverse;
    CHECK((revjoin.end() - revjoin.begin()) == 7);

    auto begin = joined.begin();
    CHECK(*(begin+0) == "this");
    CHECK(*(begin+1) == "is");
    CHECK(*(begin+2) == "his");
    CHECK(*(begin+3) == "face");
    CHECK(*(begin+4) == "another");
    CHECK(*(begin+5) == "fine");
    CHECK(*(begin+6) == "mess");

    CHECK(*(begin) == "this");
    CHECK(*(begin+=1) == "is");
    CHECK(*(begin+=1) == "his");
    CHECK(*(begin+=1) == "face");
    CHECK(*(begin+=1) == "another");
    CHECK(*(begin+=1) == "fine");
    CHECK(*(begin+=1) == "mess");

    auto end = joined.end();
    CHECK(*(end-1) == "mess");
    CHECK(*(end-2) == "fine");
    CHECK(*(end-3) == "another");
    CHECK(*(end-4) == "face");
    CHECK(*(end-5) == "his");
    CHECK(*(end-6) == "is");
    CHECK(*(end-7) == "this");

    CHECK(*(end-=1) == "mess");
    CHECK(*(end-=1) == "fine");
    CHECK(*(end-=1) == "another");
    CHECK(*(end-=1) == "face");
    CHECK(*(end-=1) == "his");
    CHECK(*(end-=1) == "is");
    CHECK(*(end-=1) == "this");

    {
        const std::array<int, 3> a{{0, 1, 2}};
        const std::array<int, 2> b{{3, 4}};
        check_equal(views::concat(a, b), {0, 1, 2, 3, 4});

        auto odd = [](int i) { return i % 2 != 0; };
        auto even_filter = ranges::views::remove_if(odd);

        auto f_rng0 = a | even_filter;
        auto f_rng1 = b | even_filter;

        check_equal(views::concat(f_rng0, f_rng1), {0, 2, 4});
    }

    // Regression test for http://github.com/ericniebler/range-v3/issues/395.
    {
        int i = 0;
        auto rng = ranges::views::concat(ranges::views::generate([&]{ return i++; }))
            | ranges::views::take_while([](int i){ return i < 30; });
        CHECK(ranges::distance(ranges::begin(rng), ranges::end(rng)) == 30);
    }

    {
        int const data[] = {0,1,2,3};
        auto dv = [&]{ return debug_input_view<int const>{data}; };
        auto rng = views::concat(dv(), dv(), dv());
        ::check_equal(rng, {0,1,2,3,0,1,2,3,0,1,2,3});
    }

    return test_result();
}
