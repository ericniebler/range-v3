// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Michel Morin 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <forward_list>
#include <list>
#include <vector>
#include <limits>
#include <range/v3/core.hpp>
#include <range/v3/distance_compare.hpp>
#include <range/v3/utility/unreachable.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

int main()
{
    using namespace ranges;

    {
        std::vector<int> v{1,2,3,4};
        auto v_min = (std::numeric_limits<range_difference_t<decltype(v)>>::min)();
        auto v_max = (std::numeric_limits<range_difference_t<decltype(v)>>::max)();
        CHECK(distance_compare(v, 3) > 0);
        CHECK(distance_compare(v, 4) == 0);
        CHECK(distance_compare(v, 5) < 0);
        CHECK(distance_compare(v, v_min) > 0);
        CHECK(distance_compare(v, v_max) < 0);

        range<range_iterator_t<decltype(v)>> rng {v.begin(), v.end()};
        CHECK(distance_compare(rng, 3) > 0);
        CHECK(distance_compare(rng, 4) == 0);
        CHECK(distance_compare(rng, 5) < 0);
        CHECK(distance_compare(rng, v_min) > 0);
        CHECK(distance_compare(rng, v_max) < 0);

        v.clear();
        CHECK(distance_compare(v, -1) > 0);
        CHECK(distance_compare(v, 0) == 0);
        CHECK(distance_compare(v, 1) < 0);
        CHECK(distance_compare(v, v_min) > 0);
        CHECK(distance_compare(v, v_max) < 0);
    }

    {
        std::list<int> l{1,2,3,4};
        auto l_min = (std::numeric_limits<range_difference_t<decltype(l)>>::min)();
        auto l_max = (std::numeric_limits<range_difference_t<decltype(l)>>::max)();
        CHECK(distance_compare(l, 3) > 0);
        CHECK(distance_compare(l, 4) == 0);
        CHECK(distance_compare(l, 5) < 0);
        CHECK(distance_compare(l, l_min) > 0);
        CHECK(distance_compare(l, l_max) < 0);

        // std::list is typically implemented as a (virtually) cyclic list; 
        // the "end" node is connected to the "begin" node (i.e. ++end() == begin()).
        // So we can use it to construct an infinite range.
        range<range_iterator_t<decltype(l)>, unreachable> inf_rng {l.begin(), {}};
        CHECK(distance_compare(inf_rng, 3) > 0);
        CHECK(distance_compare(inf_rng, 4) > 0);
        CHECK(distance_compare(inf_rng, 5) > 0);
        CHECK(distance_compare(inf_rng, l_min) > 0);
        // CHECK(distance_compare(inf_rng, l_max) > 0); // might take a huge amount of time

        l.clear();
        CHECK(distance_compare(l, -1) > 0);
        CHECK(distance_compare(l, 0) == 0);
        CHECK(distance_compare(l, 1) < 0);
        CHECK(distance_compare(l, l_min) > 0);
        CHECK(distance_compare(l, l_max) < 0);
    }

    {
        std::forward_list<int> f{1,2,3,4};
        auto f_min = (std::numeric_limits<range_difference_t<decltype(f)>>::min)();
        auto f_max = (std::numeric_limits<range_difference_t<decltype(f)>>::max)();
        CHECK(distance_compare(f, 3) > 0);
        CHECK(distance_compare(f, 4) == 0);
        CHECK(distance_compare(f, 5) < 0);
        CHECK(distance_compare(f, f_min) > 0);
        CHECK(distance_compare(f, f_max) < 0);

        sized_range<range_iterator_t<decltype(f)>> sized_rng {f.begin(), f.end(), 4};
        CHECK(distance_compare(sized_rng, 3) > 0);
        CHECK(distance_compare(sized_rng, 4) == 0);
        CHECK(distance_compare(sized_rng, 5) < 0);
        CHECK(distance_compare(sized_rng, f_min) > 0);
        CHECK(distance_compare(sized_rng, f_max) < 0);

        f.clear();
        CHECK(distance_compare(f, -1) > 0);
        CHECK(distance_compare(f, 0) == 0);
        CHECK(distance_compare(f, 1) < 0);
        CHECK(distance_compare(f, f_min) > 0);
        CHECK(distance_compare(f, f_max) < 0);
    }

    return ::test_result();
}
