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

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <utility>
#include <range/v3/core.hpp>
#include <range/v3/algorithm/find_end.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

template<class Iter1, class Iter2, typename Sent1 = Iter1, typename Sent2 = Iter2>
void
test()
{
    using namespace ranges;

    int ia[] = {0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 0, 1, 2, 3, 0, 1, 2, 0, 1, 0};
    constexpr auto sa = size(ia);
    int b[] = {0};
    int c[] = {0, 1};
    int d[] = {0, 1, 2};
    int e[] = {0, 1, 2, 3};
    int f[] = {0, 1, 2, 3, 4};
    int g[] = {0, 1, 2, 3, 4, 5};
    int h[] = {0, 1, 2, 3, 4, 5, 6};
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(b), Sent2(b + 1)) == Iter1(ia + sa - 1));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(c), Sent2(c + 2)) == Iter1(ia + 18));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(d), Sent2(d + 3)) == Iter1(ia + 15));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(e), Sent2(e + 4)) == Iter1(ia + 11));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(f), Sent2(f + 5)) == Iter1(ia + 6));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(g), Sent2(g + 6)) == Iter1(ia));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(h), Sent2(h + 7)) == Iter1(ia + sa));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(b), Sent2(b)) == Iter1(ia + sa));
    CHECK(find_end(Iter1(ia), Sent1(ia), Iter2(b), Sent2(b + 1)) == Iter1(ia));

    auto ir = make_iterator_range(Iter1(ia), Sent1(ia + sa));
    CHECK(find_end(ir, make_iterator_range(Iter2(b), Sent2(b + 1))) == Iter1(ia + sa - 1));
    CHECK(find_end(ir, make_iterator_range(Iter2(c), Sent2(c + 2))) == Iter1(ia + 18));
    CHECK(find_end(ir, make_iterator_range(Iter2(d), Sent2(d + 3))) == Iter1(ia + 15));
    CHECK(find_end(ir, make_iterator_range(Iter2(e), Sent2(e + 4))) == Iter1(ia + 11));
    CHECK(find_end(ir, make_iterator_range(Iter2(f), Sent2(f + 5))) == Iter1(ia + 6));
    CHECK(find_end(ir, make_iterator_range(Iter2(g), Sent2(g + 6))) == Iter1(ia));
    CHECK(find_end(ir, make_iterator_range(Iter2(h), Sent2(h + 7))) == Iter1(ia + sa));
    CHECK(find_end(ir, make_iterator_range(Iter2(b), Sent2(b))) == Iter1(ia + sa));

    CHECK(find_end(std::move(ir), make_iterator_range(Iter2(b), Sent2(b + 1))).get_unsafe() == Iter1(ia + sa - 1));
    CHECK(find_end(std::move(ir), make_iterator_range(Iter2(c), Sent2(c + 2))).get_unsafe() == Iter1(ia + 18));
    CHECK(find_end(std::move(ir), make_iterator_range(Iter2(d), Sent2(d + 3))).get_unsafe() == Iter1(ia + 15));
    CHECK(find_end(std::move(ir), make_iterator_range(Iter2(e), Sent2(e + 4))).get_unsafe() == Iter1(ia + 11));
    CHECK(find_end(std::move(ir), make_iterator_range(Iter2(f), Sent2(f + 5))).get_unsafe() == Iter1(ia + 6));
    CHECK(find_end(std::move(ir), make_iterator_range(Iter2(g), Sent2(g + 6))).get_unsafe() == Iter1(ia));
    CHECK(find_end(std::move(ir), make_iterator_range(Iter2(h), Sent2(h + 7))).get_unsafe() == Iter1(ia + sa));
    CHECK(find_end(std::move(ir), make_iterator_range(Iter2(b), Sent2(b))).get_unsafe() == Iter1(ia + sa));

    auto er = make_iterator_range(Iter1(ia), Sent1(ia));
    CHECK(find_end(er, make_iterator_range(Iter2(b), Sent2(b + 1))) == Iter1(ia));
    CHECK(find_end(std::move(er), make_iterator_range(Iter2(b), Sent2(b + 1))).get_unsafe() == Iter1(ia));
}

struct count_equal
{
    static unsigned count;
    template<class T>
    bool operator()(const T& x, const T& y)
    {
        ++count; return x == y;
    }
};

unsigned count_equal::count = 0;

template<class Iter1, class Iter2, typename Sent1 = Iter1, typename Sent2 = Iter2>
void
test_pred()
{
    using namespace ranges;

    int ia[] = {0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 0, 1, 2, 3, 0, 1, 2, 0, 1, 0};
    constexpr auto sa = size(ia);
    int b[] = {0};
    int c[] = {0, 1};
    int d[] = {0, 1, 2};
    int e[] = {0, 1, 2, 3};
    int f[] = {0, 1, 2, 3, 4};
    int g[] = {0, 1, 2, 3, 4, 5};
    int h[] = {0, 1, 2, 3, 4, 5, 6};

    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(b), Sent2(b + 1), count_equal()) == Iter1(ia + sa - 1));
    CHECK(count_equal::count <= 1 * (sa - 1 + 1));
    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(c), Sent2(c + 2), count_equal()) == Iter1(ia + 18));
    CHECK(count_equal::count <= 2 * (sa - 2 + 1));
    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(d), Sent2(d + 3), count_equal()) == Iter1(ia + 15));
    CHECK(count_equal::count <= 3 * (sa - 3 + 1));
    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(e), Sent2(e + 4), count_equal()) == Iter1(ia + 11));
    CHECK(count_equal::count <= 4 * (sa - 4 + 1));
    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(f), Sent2(f + 5), count_equal()) == Iter1(ia + 6));
    CHECK(count_equal::count <= 5 * (sa - 5 + 1));
    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(g), Sent2(g + 6), count_equal()) == Iter1(ia));
    CHECK(count_equal::count <= 6 * (sa - 6 + 1));
    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(h), Sent2(h + 7), count_equal()) == Iter1(ia + sa));
    CHECK(count_equal::count <= 7 * (sa - 7 + 1));
    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(b), Sent2(b), count_equal()) == Iter1(ia + sa));
    CHECK(count_equal::count == 0u);
    count_equal::count = 0;
    CHECK(find_end(Iter1(ia), Sent1(ia), Iter2(b), Sent2(b + 1), count_equal()) == Iter1(ia));
    CHECK(count_equal::count == 0u);

    auto ir = make_iterator_range(Iter1(ia), Sent1(ia + sa));
    count_equal::count = 0;
    CHECK(find_end(ir, make_iterator_range(Iter2(b), Sent2(b + 1)), count_equal()) == Iter1(ia + sa - 1));
    CHECK(count_equal::count <= 1 * (sa - 1 + 1));
    count_equal::count = 0;
    CHECK(find_end(ir, make_iterator_range(Iter2(c), Sent2(c + 2)), count_equal()) == Iter1(ia + 18));
    CHECK(count_equal::count <= 2 * (sa - 2 + 1));
    count_equal::count = 0;
    CHECK(find_end(ir, make_iterator_range(Iter2(d), Sent2(d + 3)), count_equal()) == Iter1(ia + 15));
    CHECK(count_equal::count <= 3 * (sa - 3 + 1));
    count_equal::count = 0;
    CHECK(find_end(ir, make_iterator_range(Iter2(e), Sent2(e + 4)), count_equal()) == Iter1(ia + 11));
    CHECK(count_equal::count <= 4 * (sa - 4 + 1));
    count_equal::count = 0;
    CHECK(find_end(ir, make_iterator_range(Iter2(f), Sent2(f + 5)), count_equal()) == Iter1(ia + 6));
    CHECK(count_equal::count <= 5 * (sa - 5 + 1));
    count_equal::count = 0;
    CHECK(find_end(ir, make_iterator_range(Iter2(g), Sent2(g + 6)), count_equal()) == Iter1(ia));
    CHECK(count_equal::count <= 6 * (sa - 6 + 1));
    count_equal::count = 0;
    CHECK(find_end(ir, make_iterator_range(Iter2(h), Sent2(h + 7)), count_equal()) == Iter1(ia + sa));
    CHECK(count_equal::count <= 7 * (sa - 7 + 1));
    count_equal::count = 0;
    CHECK(find_end(ir, make_iterator_range(Iter2(b), Sent2(b)), count_equal()) == Iter1(ia + sa));
    CHECK(count_equal::count == 0u);
    count_equal::count = 0;
    auto er = make_iterator_range(Iter1(ia), Sent1(ia));
    CHECK(find_end(er, make_iterator_range(Iter2(b), Sent2(b + 1)), count_equal()) == Iter1(ia));
    CHECK(count_equal::count == 0u);

    static_assert(std::is_same<Iter1, decltype(find_end(er, {1, 2, 3}))>::value, "");
}

struct S
{
    int i_;
};

template<class Iter1, class Iter2, typename Sent1 = Iter1, typename Sent2 = Iter2>
void
test_proj()
{
    using namespace ranges;

    S ia[] = {{0}, {1}, {2}, {3}, {4}, {5}, {0}, {1}, {2}, {3}, {4}, {0}, {1}, {2}, {3}, {0}, {1}, {2}, {0}, {1}, {0}};
    constexpr auto sa = size(ia);
    int b[] = {0};
    int c[] = {0, 1};
    int d[] = {0, 1, 2};
    int e[] = {0, 1, 2, 3};
    int f[] = {0, 1, 2, 3, 4};
    int g[] = {0, 1, 2, 3, 4, 5};
    int h[] = {0, 1, 2, 3, 4, 5, 6};
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(b), Sent2(b + 1), equal_to(), &S::i_) == Iter1(ia + sa - 1));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(c), Sent2(c + 2), equal_to(), &S::i_) == Iter1(ia + 18));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(d), Sent2(d + 3), equal_to(), &S::i_) == Iter1(ia + 15));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(e), Sent2(e + 4), equal_to(), &S::i_) == Iter1(ia + 11));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(f), Sent2(f + 5), equal_to(), &S::i_) == Iter1(ia + 6));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(g), Sent2(g + 6), equal_to(), &S::i_) == Iter1(ia));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(h), Sent2(h + 7), equal_to(), &S::i_) == Iter1(ia + sa));
    CHECK(find_end(Iter1(ia), Sent1(ia + sa), Iter2(b), Sent2(b), equal_to(), &S::i_) == Iter1(ia + sa));
    CHECK(find_end(Iter1(ia), Sent1(ia), Iter2(b), Sent2(b + 1), equal_to(), &S::i_) == Iter1(ia));

    auto ir = make_iterator_range(Iter1(ia), Sent1(ia + sa));
    CHECK(find_end(ir, make_iterator_range(Iter2(b), Sent2(b + 1)), equal_to(), &S::i_) == Iter1(ia + sa - 1));
    CHECK(find_end(ir, make_iterator_range(Iter2(c), Sent2(c + 2)), equal_to(), &S::i_) == Iter1(ia + 18));
    CHECK(find_end(ir, make_iterator_range(Iter2(d), Sent2(d + 3)), equal_to(), &S::i_) == Iter1(ia + 15));
    CHECK(find_end(ir, make_iterator_range(Iter2(e), Sent2(e + 4)), equal_to(), &S::i_) == Iter1(ia + 11));
    CHECK(find_end(ir, make_iterator_range(Iter2(f), Sent2(f + 5)), equal_to(), &S::i_) == Iter1(ia + 6));
    CHECK(find_end(ir, make_iterator_range(Iter2(g), Sent2(g + 6)), equal_to(), &S::i_) == Iter1(ia));
    CHECK(find_end(ir, make_iterator_range(Iter2(h), Sent2(h + 7)), equal_to(), &S::i_) == Iter1(ia + sa));
    CHECK(find_end(ir, make_iterator_range(Iter2(b), Sent2(b)), equal_to(), &S::i_) == Iter1(ia + sa));
    auto er = make_iterator_range(Iter1(ia), Sent1(ia));
    CHECK(find_end(er, make_iterator_range(Iter2(b), Sent2(b + 1)), equal_to(), &S::i_) == Iter1(ia));
}

int main()
{
    test<forward_iterator<const int*>, forward_iterator<const int*> >();
    test<forward_iterator<const int*>, bidirectional_iterator<const int*> >();
    test<forward_iterator<const int*>, random_access_iterator<const int*> >();
    test<bidirectional_iterator<const int*>, forward_iterator<const int*> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<const int*> >();
    test<random_access_iterator<const int*>, forward_iterator<const int*> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<const int*> >();
    test<random_access_iterator<const int*>, random_access_iterator<const int*> >();

    test<forward_iterator<const int*>, forward_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test<forward_iterator<const int*>, bidirectional_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test<forward_iterator<const int*>, random_access_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test<bidirectional_iterator<const int*>, forward_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test<bidirectional_iterator<const int*>, random_access_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test<random_access_iterator<const int*>, forward_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test<random_access_iterator<const int*>, bidirectional_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test<random_access_iterator<const int*>, random_access_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();

    test_pred<forward_iterator<const int*>, forward_iterator<const int*> >();
    test_pred<forward_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_pred<forward_iterator<const int*>, random_access_iterator<const int*> >();
    test_pred<bidirectional_iterator<const int*>, forward_iterator<const int*> >();
    test_pred<bidirectional_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_pred<bidirectional_iterator<const int*>, random_access_iterator<const int*> >();
    test_pred<random_access_iterator<const int*>, forward_iterator<const int*> >();
    test_pred<random_access_iterator<const int*>, bidirectional_iterator<const int*> >();
    test_pred<random_access_iterator<const int*>, random_access_iterator<const int*> >();

    test_pred<forward_iterator<const int*>, forward_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test_pred<forward_iterator<const int*>, bidirectional_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test_pred<forward_iterator<const int*>, random_access_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test_pred<bidirectional_iterator<const int*>, forward_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test_pred<bidirectional_iterator<const int*>, bidirectional_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test_pred<bidirectional_iterator<const int*>, random_access_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test_pred<random_access_iterator<const int*>, forward_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test_pred<random_access_iterator<const int*>, bidirectional_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();
    test_pred<random_access_iterator<const int*>, random_access_iterator<const int*>, sentinel<const int*>, sentinel<const int *> >();

    test_proj<forward_iterator<const S*>, forward_iterator<const int*> >();
    test_proj<forward_iterator<const S*>, bidirectional_iterator<const int*> >();
    test_proj<forward_iterator<const S*>, random_access_iterator<const int*> >();
    test_proj<bidirectional_iterator<const S*>, forward_iterator<const int*> >();
    test_proj<bidirectional_iterator<const S*>, bidirectional_iterator<const int*> >();
    test_proj<bidirectional_iterator<const S*>, random_access_iterator<const int*> >();
    test_proj<random_access_iterator<const S*>, forward_iterator<const int*> >();
    test_proj<random_access_iterator<const S*>, bidirectional_iterator<const int*> >();
    test_proj<random_access_iterator<const S*>, random_access_iterator<const int*> >();

    test_proj<forward_iterator<const S*>, forward_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();
    test_proj<forward_iterator<const S*>, bidirectional_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();
    test_proj<forward_iterator<const S*>, random_access_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();
    test_proj<bidirectional_iterator<const S*>, forward_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();
    test_proj<bidirectional_iterator<const S*>, bidirectional_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();
    test_proj<bidirectional_iterator<const S*>, random_access_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();
    test_proj<random_access_iterator<const S*>, forward_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();
    test_proj<random_access_iterator<const S*>, bidirectional_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();
    test_proj<random_access_iterator<const S*>, random_access_iterator<const int*>, sentinel<const S*>, sentinel<const int *> >();

    return ::test_result();
}
