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
#include <string>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/istream_range.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto rng0 = rgi | view::slice(3, 9);
    has_type<int &>(*begin(rng0));
    models<concepts::BoundedView>(aux::copy(rng0));
    models<concepts::SizedView>(aux::copy(rng0));
    models<concepts::RandomAccessIterator>(begin(rng0));
    ::check_equal(rng0, {3, 4, 5, 6, 7, 8});

    auto rng1 = rng0 | view::reverse;
    has_type<int &>(*begin(rng1));
    models<concepts::BoundedView>(aux::copy(rng1));
    models<concepts::SizedView>(aux::copy(rng1));
    models<concepts::RandomAccessIterator>(begin(rng1));
    ::check_equal(rng1, {8, 7, 6, 5, 4, 3});

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng2 = v | view::slice(3, 9) | view::reverse;
    has_type<int &>(*begin(rng2));
    models<concepts::BoundedView>(aux::copy(rng2));
    models<concepts::SizedView>(aux::copy(rng2));
    models<concepts::RandomAccessIterator>(begin(rng2));
    ::check_equal(rng2, {8, 7, 6, 5, 4, 3});

    std::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto rng3 = l | view::slice(3, 9);
    has_type<int &>(*begin(rng3));
    models<concepts::View>(aux::copy(rng3));
    models_not<concepts::BoundedView>(aux::copy(rng3));
    models<concepts::SizedView>(aux::copy(rng3));
    models<concepts::BidirectionalIterator>(begin(rng3));
    models_not<concepts::RandomAccessIterator>(begin(rng3));
    ::check_equal(rng3, {3, 4, 5, 6, 7, 8});

    auto rng4 = view::iota(10) | view::slice(10, 20);
    ::models<concepts::BoundedView>(aux::copy(rng4));
    ::models<concepts::SizedView>(aux::copy(rng4));
    static_assert(!ranges::is_infinite<decltype(rng4)>::value, "");
    ::check_equal(rng4, {20, 21, 22, 23, 24, 25, 26, 27, 28, 29});

    auto rng5 = view::iota(10)[{10, 20}];
    ::models<concepts::BoundedView>(aux::copy(rng5));
    ::models<concepts::SizedView>(aux::copy(rng5));
    static_assert(!ranges::is_infinite<decltype(rng5)>::value, "");
    ::check_equal(rng5, {20, 21, 22, 23, 24, 25, 26, 27, 28, 29});

    auto rng6 = view::all(l)[{3, 9}];
    has_type<int &>(*begin(rng6));
    models<concepts::View>(aux::copy(rng6));
    models_not<concepts::BoundedView>(aux::copy(rng6));
    models<concepts::SizedView>(aux::copy(rng6));
    models<concepts::BidirectionalIterator>(begin(rng6));
    models_not<concepts::RandomAccessIterator>(begin(rng6));
    ::check_equal(rng6, {3, 4, 5, 6, 7, 8});

    auto rng7 = view::all(l)[{3, end}];
    has_type<int &>(*begin(rng7));
    models<concepts::View>(aux::copy(rng7));
    models<concepts::BoundedView>(aux::copy(rng7));
    models<concepts::SizedView>(aux::copy(rng7));
    models<concepts::BidirectionalIterator>(begin(rng7));
    models_not<concepts::RandomAccessIterator>(begin(rng7));
    ::check_equal(rng7, {3, 4, 5, 6, 7, 8, 9, 10});

    auto rng8 = view::all(l)[{end-5,end-2}];
    has_type<int &>(*begin(rng8));
    models<concepts::View>(aux::copy(rng8));
    models_not<concepts::BoundedView>(aux::copy(rng8));
    models<concepts::SizedView>(aux::copy(rng8));
    models<concepts::BidirectionalIterator>(begin(rng8));
    models_not<concepts::RandomAccessIterator>(begin(rng8));
    ::check_equal(rng8, {6, 7, 8});

    auto rng9 = view::ints(0)[{0,end}];
    static_assert(is_infinite<decltype(rng9)>::value, "should be infinite");

    {
        std::string str{"0 1 2 3 4 5 6 7 8 9"};
        std::stringstream sin{str};
        auto rng10 = istream<int>(sin)[{3,9}];
        ::check_equal(rng10, {3, 4, 5, 6, 7, 8});
    }

    {
        std::string str{"0 1 2 3 4 5 6 7 8 9"};
        std::stringstream sin{str};
        auto rng11 = istream<int>(sin)[{3,end}];
        ::check_equal(rng11, {3, 4, 5, 6, 7, 8, 9});
    }

    {
        auto letters = view::closed_iota('a','g');
        static_assert(RandomAccessView<decltype(letters)>(), "");
        static_assert(BoundedView<decltype(letters)>(), "");
        ::check_equal(letters[{2,end-2}], {'c','d','e'});
    }

    {
        int const some_ints[] = {0,1,2,3,4,5,6,7,8,9};
        auto rng = debug_input_view<int const>{some_ints} | view::slice(3,10);
        ::check_equal(rng, {3, 4, 5, 6, 7, 8, 9});
    }

    return test_result();
}
