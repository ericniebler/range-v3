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

#include <range/v3/core.hpp>
#include <range/v3/algorithm/count_if.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

struct S
{
    int i;
};

struct T
{
    bool b;
    bool m() { return b; }
};

constexpr bool even(int i) { return i % 2 == 0; }

int main()
{
    using namespace ranges;
    auto equals = [](int i){ return std::bind(equal_to{}, i, std::placeholders::_1); };

    int ia[] = {0, 1, 2, 2, 0, 1, 2, 3};
    constexpr unsigned cia = size(ia);

    CHECK(count_if(input_iterator<const int*>(ia),
                   sentinel<const int*>(ia + cia), equals(2)) == 3);
    CHECK(count_if(input_iterator<const int*>(ia),
                   sentinel<const int*>(ia + cia), equals(7)) == 0);
    CHECK(count_if(input_iterator<const int*>(ia),
                   sentinel<const int*>(ia), equals(2)) == 0);

    CHECK(count_if(make_range(input_iterator<const int*>(ia),
                         sentinel<const int*>(ia + cia)), equals(2)) == 3);
    CHECK(count_if(make_range(input_iterator<const int*>(ia),
                         sentinel<const int*>(ia + cia)), equals(7)) == 0);
    CHECK(count_if(make_range(input_iterator<const int*>(ia),
                         sentinel<const int*>(ia)), equals(2)) == 0);

    S sa[] = {{0}, {1}, {2}, {2}, {0}, {1}, {2}, {3}};
    constexpr unsigned csa = size(ia);

    CHECK(count_if(input_iterator<const S*>(sa),
                   sentinel<const S*>(sa + csa), equals(2), &S::i) == 3);
    CHECK(count_if(input_iterator<const S*>(sa),
                   sentinel<const S*>(sa + csa), equals(7), &S::i) == 0);
    CHECK(count_if(input_iterator<const S*>(sa),
                   sentinel<const S*>(sa), equals(2), &S::i) == 0);

    CHECK(count_if(make_range(input_iterator<const S*>(sa),
                         sentinel<const S*>(sa + csa)), equals(2), &S::i) == 3);
    CHECK(count_if(make_range(input_iterator<const S*>(sa),
                         sentinel<const S*>(sa + csa)), equals(7), &S::i) == 0);
    CHECK(count_if(make_range(input_iterator<const S*>(sa),
                         sentinel<const S*>(sa)), equals(2), &S::i) == 0);

    T ta[] = {{true}, {false}, {true}, {false}, {false}, {true}, {false}, {false}, {true}, {false}};
    CHECK(count_if(input_iterator<T*>(ta),
                   sentinel<T*>(ta + size(ta)), &T::m) == 4);
    CHECK(count_if(input_iterator<T*>(ta),
                   sentinel<T*>(ta + size(ta)), &T::b) == 4);
    CHECK(count_if(make_range(input_iterator<T*>(ta),
                         sentinel<T*>(ta + size(ta))), &T::m) == 4);
    CHECK(count_if(make_range(input_iterator<T*>(ta),
                         sentinel<T*>(ta + size(ta))), &T::b) == 4);

#ifdef RANGES_CXX_GREATER_THAN_11
    {
        static_assert(ranges::count_if({0, 1, 2, 1, 3, 1, 4}, even)  == 3, "");
        static_assert(ranges::count_if({1, 1, 3, 1}, even)  == 0, "");
    }
#endif

    return ::test_result();
}
