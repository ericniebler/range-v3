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

#include <list>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/algorithm/equal.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto rng0 = rgi | view::take(6);
    has_type<int &>(*begin(rng0));
    models<concepts::BoundedRange>(rng0);
    models<concepts::SizedRange>(rng0);
    models<concepts::RandomAccessIterator>(begin(rng0));
    ::check_equal(rng0, {0, 1, 2, 3, 4, 5});
    CHECK(size(rng0) == 6u);

    auto rng0b = rgi | view::take(20);
    has_type<int &>(*begin(rng0b));
    models<concepts::BoundedRange>(rng0b);
    models<concepts::SizedRange>(rng0b);
    models<concepts::RandomAccessIterator>(begin(rng0b));
    ::check_equal(rng0b, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    CHECK(size(rng0b) == 11u);

    auto rng1 = rng0 | view::reverse;
    has_type<int &>(*begin(rng1));
    models<concepts::BoundedRange>(rng1);
    models<concepts::SizedRange>(rng1);
    models<concepts::RandomAccessIterator>(begin(rng1));
    ::check_equal(rng1, {5, 4, 3, 2, 1, 0});

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng2 = v | view::take(6) | view::reverse;
    has_type<int &>(*begin(rng2));
    models<concepts::BoundedRange>(rng2);
    models<concepts::SizedRange>(rng2);
    models<concepts::RandomAccessIterator>(begin(rng2));
    ::check_equal(rng2, {5, 4, 3, 2, 1, 0});

    std::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng3 = l | view::take(6);
    has_type<int &>(*begin(rng3));
    models<concepts::Range>(rng3);
    models_not<concepts::BoundedRange>(rng3);
    models<concepts::SizedRange>(rng3);
    models<concepts::BidirectionalIterator>(begin(rng3));
    models_not<concepts::RandomAccessIterator>(begin(rng3));
    ::check_equal(rng3, {0, 1, 2, 3, 4, 5});
    CHECK(size(rng3) == 6u);

    auto rng3b = l | view::take(20);
    has_type<int &>(*begin(rng3b));
    models<concepts::Range>(rng3b);
    models_not<concepts::BoundedRange>(rng3b);
    models<concepts::SizedRange>(rng3b);
    models<concepts::BidirectionalIterator>(begin(rng3b));
    models_not<concepts::RandomAccessIterator>(begin(rng3b));
    ::check_equal(rng3b, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    CHECK(size(rng3b) == 11u);

    auto rng4 = view::iota(10) | view::take(10);
    ::models<concepts::BoundedRange>(rng4);
    ::models<concepts::SizedRange>(rng4);
    static_assert(!ranges::is_infinite<decltype(rng4)>::value, "");
    ::check_equal(rng4, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19});
    CHECK(size(rng4) == 10u);

    auto rng5 = view::iota(10) | view::take(10) | view::reverse;
    ::models<concepts::BoundedRange>(rng5);
    ::models<concepts::SizedRange>(rng5);
    static_assert(!ranges::is_infinite<decltype(rng5)>::value, "");
    ::check_equal(rng5, {19, 18, 17, 16, 15, 14, 13, 12, 11, 10});
    CHECK(size(rng5) == 10u);

    auto c_str = view::delimit("hello world", '\0');
    models<concepts::RandomAccessIterable>(c_str);
    models_not<concepts::SizedIterable>(c_str);

    auto rng6 = c_str | view::take(5);
    ::models<concepts::RandomAccessIterable>(rng6);
    ::models_not<concepts::BoundedRange>(rng6);
    ::models_not<concepts::SizedRange>(rng6);
    check_equal(rng6, {'h','e','l','l','o'});

    auto rng7 = c_str | view::take(20);
    check_equal(rng7, {'h','e','l','l','o',' ','w','o','r','l','d'});

    range<std::list<int>::iterator> rl{l.begin(), l.end()};
    ::models<concepts::BidirectionalIterable>(rl);
    ::models<concepts::BoundedRange>(rl);
    ::models_not<concepts::SizedRange>(rl);

    auto rng8 = rl | view::take(5);
    ::models<concepts::BidirectionalIterable>(rng8);
    ::models_not<concepts::BoundedRange>(rng8);
    ::models_not<concepts::SizedRange>(rng8);
    check_equal(rng8, {0, 1, 2, 3, 4});

    auto rng9 = rl | view::take(20);
    ::models<concepts::BidirectionalIterable>(rng9);
    ::models_not<concepts::BoundedRange>(rng9);
    ::models_not<concepts::SizedRange>(rng9);
    check_equal(rng9, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

#ifdef RANGES_CPP_STD_14_OR_GREATER
    {
        constexpr auto rng6 = view::iota(10) | view::take(10);
        ::models<concepts::BoundedRange>(rng6);
        ::models<concepts::SizedRange>(rng6);
        static_assert(!ranges::is_infinite<decltype(rng6)>::value, "");
        static_assert(ranges::equal(rng6, {10, 11, 12, 13, 14, 15, 16, 17, 18, 19}), "");
        static_assert(size(rng6) == 10u, "");

        constexpr auto rng7 = view::iota(10) | view::take(10) | view::reverse;
        ::models<concepts::BoundedRange>(rng7);
        ::models<concepts::SizedRange>(rng7);
        static_assert(!ranges::is_infinite<decltype(rng7)>::value, "");
        static_assert(ranges::equal(rng7, {19, 18, 17, 16, 15, 14, 13, 12, 11, 10}), "");
        static_assert(size(rng7) == 10u, "");
    }
#endif

    return test_result();
}
