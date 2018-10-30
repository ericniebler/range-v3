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
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <memory>
#include <utility>
#include <functional>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/remove_if.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

template<class Iter, class Sent = Iter>
void
test_iter()
{
    int ia[] = {0, 1, 2, 3, 4, 2, 3, 4, 2};
    constexpr auto sa = ranges::size(ia);
    using namespace std::placeholders;
    Iter r = ranges::remove_if(Iter(ia), Sent(ia+sa), std::bind(std::equal_to<int>(), _1, 2));
    CHECK(base(r) == ia + sa-3);
    CHECK(ia[0] == 0);
    CHECK(ia[1] == 1);
    CHECK(ia[2] == 3);
    CHECK(ia[3] == 4);
    CHECK(ia[4] == 3);
    CHECK(ia[5] == 4);
}

template<class Iter, class Sent = Iter>
void
test_range()
{
    int ia[] = {0, 1, 2, 3, 4, 2, 3, 4, 2};
    constexpr auto sa = ranges::size(ia);
    using namespace std::placeholders;
    Iter r = ranges::remove_if(::as_lvalue(ranges::make_iterator_range(Iter(ia), Sent(ia+sa))), std::bind(std::equal_to<int>(), _1, 2));
    CHECK(base(r) == ia + sa-3);
    CHECK(ia[0] == 0);
    CHECK(ia[1] == 1);
    CHECK(ia[2] == 3);
    CHECK(ia[3] == 4);
    CHECK(ia[4] == 3);
    CHECK(ia[5] == 4);
}

struct pred
{
    bool operator()(const std::unique_ptr<int>& i) {return *i == 2;}
};

template<class Iter, class Sent = Iter>
void
test_iter_rvalue()
{
    constexpr unsigned sa = 9;
    std::unique_ptr<int> ia[sa];
    ia[0].reset(new int(0));
    ia[1].reset(new int(1));
    ia[2].reset(new int(2));
    ia[3].reset(new int(3));
    ia[4].reset(new int(4));
    ia[5].reset(new int(2));
    ia[6].reset(new int(3));
    ia[7].reset(new int(4));
    ia[8].reset(new int(2));
    Iter r = ranges::remove_if(Iter(ia), Sent(ia+sa), pred());
    CHECK(base(r) == ia + sa-3);
    CHECK(*ia[0] == 0);
    CHECK(*ia[1] == 1);
    CHECK(*ia[2] == 3);
    CHECK(*ia[3] == 4);
    CHECK(*ia[4] == 3);
    CHECK(*ia[5] == 4);
}

template<class Iter, class Sent = Iter>
void
test_range_rvalue()
{
    constexpr unsigned sa = 9;
    std::unique_ptr<int> ia[sa];
    ia[0].reset(new int(0));
    ia[1].reset(new int(1));
    ia[2].reset(new int(2));
    ia[3].reset(new int(3));
    ia[4].reset(new int(4));
    ia[5].reset(new int(2));
    ia[6].reset(new int(3));
    ia[7].reset(new int(4));
    ia[8].reset(new int(2));
    Iter r = ranges::remove_if(::as_lvalue(ranges::make_iterator_range(Iter(ia), Sent(ia+sa))), pred());
    CHECK(base(r) == ia + sa-3);
    CHECK(*ia[0] == 0);
    CHECK(*ia[1] == 1);
    CHECK(*ia[2] == 3);
    CHECK(*ia[3] == 4);
    CHECK(*ia[4] == 3);
    CHECK(*ia[5] == 4);
}

struct S
{
    int i;
};

int main()
{
    test_iter<forward_iterator<int*> >();
    test_iter<bidirectional_iterator<int*> >();
    test_iter<random_access_iterator<int*> >();
    test_iter<int*>();
    test_iter<forward_iterator<int*>, sentinel<int*>>();
    test_iter<bidirectional_iterator<int*>, sentinel<int*>>();
    test_iter<random_access_iterator<int*>, sentinel<int*>>();

    test_range<forward_iterator<int*> >();
    test_range<bidirectional_iterator<int*> >();
    test_range<random_access_iterator<int*> >();
    test_range<int*>();
    test_range<forward_iterator<int*>, sentinel<int*>>();
    test_range<bidirectional_iterator<int*>, sentinel<int*>>();
    test_range<random_access_iterator<int*>, sentinel<int*>>();

    test_iter_rvalue<forward_iterator<std::unique_ptr<int>*> >();
    test_iter_rvalue<bidirectional_iterator<std::unique_ptr<int>*> >();
    test_iter_rvalue<random_access_iterator<std::unique_ptr<int>*> >();
    test_iter_rvalue<std::unique_ptr<int>*>();
    test_iter_rvalue<forward_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();
    test_iter_rvalue<bidirectional_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();
    test_iter_rvalue<random_access_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();

    test_range_rvalue<forward_iterator<std::unique_ptr<int>*> >();
    test_range_rvalue<bidirectional_iterator<std::unique_ptr<int>*> >();
    test_range_rvalue<random_access_iterator<std::unique_ptr<int>*> >();
    test_range_rvalue<std::unique_ptr<int>*>();
    test_range_rvalue<forward_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();
    test_range_rvalue<bidirectional_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();
    test_range_rvalue<random_access_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();

    {
        // Check projection
        S ia[] = {S{0}, S{1}, S{2}, S{3}, S{4}, S{2}, S{3}, S{4}, S{2}};
        constexpr auto sa = ranges::size(ia);
        using namespace std::placeholders;
        S* r = ranges::remove_if(ia, std::bind(std::equal_to<int>(), _1, 2), &S::i);
        CHECK(r == ia + sa-3);
        CHECK(ia[0].i == 0);
        CHECK(ia[1].i == 1);
        CHECK(ia[2].i == 3);
        CHECK(ia[3].i == 4);
        CHECK(ia[4].i == 3);
        CHECK(ia[5].i == 4);
    }

    {
        // Check rvalue range
        S ia[] = {S{0}, S{1}, S{2}, S{3}, S{4}, S{2}, S{3}, S{4}, S{2}};
        constexpr auto sa = ranges::size(ia);
        using namespace std::placeholders;
        auto r = ranges::remove_if(ranges::view::all(ia), std::bind(std::equal_to<int>(), _1, 2), &S::i);
        CHECK(r.get_unsafe() == ia + sa-3);
        CHECK(ia[0].i == 0);
        CHECK(ia[1].i == 1);
        CHECK(ia[2].i == 3);
        CHECK(ia[3].i == 4);
        CHECK(ia[4].i == 3);
        CHECK(ia[5].i == 4);

        // Some tests for sanitizing an algorithm result
        static_assert(std::is_same<decltype(r), ranges::dangling<S *>>::value, "");
        auto r2 = ranges::sanitize(r);
        static_assert(std::is_same<decltype(r2), ranges::dangling<>>::value, "");
        auto r3 = ranges::sanitize(const_cast<decltype(r) const &>(r));
        static_assert(std::is_same<decltype(r3), ranges::dangling<>>::value, "");
        auto r4 = ranges::sanitize(std::move(r));
        static_assert(std::is_same<decltype(r4), ranges::dangling<>>::value, "");
    }

    return ::test_result();
}
