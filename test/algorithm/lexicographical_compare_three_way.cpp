// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Google LLC 2020-present
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

#include <range/v3/core.hpp>
#include <range/v3/algorithm/lexicographical_compare_three_way.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

#if __cplusplus > 201703L
#include <version>

#if defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_three_way_comparison)

template<class Iter1, class Iter2, class Sent1 = Iter1, class Sent2 = Iter2>
void
test_iter1()
{
    int ia[] = {1, 2, 3, 4};
    constexpr auto sa = ranges::size(ia);
    int ib[] = {1, 2, 3};
    using ranges::lexicographical_compare_three_way;
    CHECK(lexicographical_compare_three_way(Iter1(ia), Sent1(ia+sa), Iter1(ia), Sent1(ia+sa)) == std::strong_ordering::equal);
    CHECK(lexicographical_compare_three_way(Iter1(ia), Sent1(ia+sa), Iter2(ib), Sent2(ib+2)) == std::strong_ordering::greater);
    CHECK(lexicographical_compare_three_way(Iter1(ib), Sent1(ib+2), Iter2(ia), Sent2(ia+sa)) == std::strong_ordering::less);
    CHECK(lexicographical_compare_three_way(Iter1(ia), Sent1(ia+sa), Iter2(ib), Sent2(ib+3)) == std::strong_ordering::greater);
    CHECK(lexicographical_compare_three_way(Iter1(ib), Sent1(ib+3), Iter2(ia), Sent2(ia+sa)) == std::strong_ordering::less);
    CHECK(lexicographical_compare_three_way(Iter1(ia), Sent1(ia+sa), Iter2(ib+1), Sent2(ib+3)) == std::strong_ordering::less);
    CHECK(lexicographical_compare_three_way(Iter1(ib+1), Sent1(ib+3), Iter2(ia), Sent2(ia+sa)) == std::strong_ordering::greater);
}

void test_iter()
{
    typedef Sentinel<const int*> S;

    test_iter1<InputIterator<const int*>, InputIterator<const int*> >();
    test_iter1<InputIterator<const int*>, ForwardIterator<const int*> >();
    test_iter1<InputIterator<const int*>, BidirectionalIterator<const int*> >();
    test_iter1<InputIterator<const int*>, RandomAccessIterator<const int*> >();
    test_iter1<InputIterator<const int*>, InputIterator<const int*>, S, S>();
    test_iter1<InputIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_iter1<InputIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_iter1<InputIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_iter1<InputIterator<const int*>, const int*>();

    test_iter1<ForwardIterator<const int*>, InputIterator<const int*> >();
    test_iter1<ForwardIterator<const int*>, ForwardIterator<const int*> >();
    test_iter1<ForwardIterator<const int*>, BidirectionalIterator<const int*> >();
    test_iter1<ForwardIterator<const int*>, RandomAccessIterator<const int*> >();
    test_iter1<ForwardIterator<const int*>, InputIterator<const int*>, S, S>();
    test_iter1<ForwardIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_iter1<ForwardIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_iter1<ForwardIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_iter1<ForwardIterator<const int*>, const int*>();

    test_iter1<BidirectionalIterator<const int*>, InputIterator<const int*> >();
    test_iter1<BidirectionalIterator<const int*>, ForwardIterator<const int*> >();
    test_iter1<BidirectionalIterator<const int*>, BidirectionalIterator<const int*> >();
    test_iter1<BidirectionalIterator<const int*>, RandomAccessIterator<const int*> >();
    test_iter1<BidirectionalIterator<const int*>, InputIterator<const int*>, S, S>();
    test_iter1<BidirectionalIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_iter1<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_iter1<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_iter1<BidirectionalIterator<const int*>, const int*>();

    test_iter1<RandomAccessIterator<const int*>, InputIterator<const int*> >();
    test_iter1<RandomAccessIterator<const int*>, ForwardIterator<const int*> >();
    test_iter1<RandomAccessIterator<const int*>, BidirectionalIterator<const int*> >();
    test_iter1<RandomAccessIterator<const int*>, RandomAccessIterator<const int*> >();
    test_iter1<RandomAccessIterator<const int*>, InputIterator<const int*>, S, S>();
    test_iter1<RandomAccessIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_iter1<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_iter1<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_iter1<RandomAccessIterator<const int*>, const int*>();

    test_iter1<const int*, InputIterator<const int*> >();
    test_iter1<const int*, ForwardIterator<const int*> >();
    test_iter1<const int*, BidirectionalIterator<const int*> >();
    test_iter1<const int*, RandomAccessIterator<const int*> >();
    test_iter1<const int*, InputIterator<const int*>, const int*, S>();
    test_iter1<const int*, ForwardIterator<const int*>, const int*, S>();
    test_iter1<const int*, BidirectionalIterator<const int*>, const int*, S>();
    test_iter1<const int*, RandomAccessIterator<const int*>, const int*, S>();
    test_iter1<const int*, const int*>();
}

template<class Iter1, class Iter2, class Sent1 = Iter1, class Sent2 = Iter2>
void
test_iter_comp1()
{
    int ia[] = {1, 2, 3, 4};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    int ib[] = {1, 2, 3};
    auto c = [](auto const x, auto const y) {
        return static_cast<std::weak_ordering>(x <=> y);
    };
    using ranges::lexicographical_compare_three_way;
    CHECK(lexicographical_compare_three_way(Iter1(ia), Sent1(ia+sa), Iter1(ia), Sent1(ia+sa), c) == std::weak_ordering::equivalent);
    CHECK(lexicographical_compare_three_way(Iter1(ia), Sent1(ia+sa), Iter2(ib), Sent2(ib+2), c) == std::weak_ordering::greater);
    CHECK(lexicographical_compare_three_way(Iter1(ib), Sent1(ib+2), Iter2(ia), Sent2(ia+sa), c) == std::weak_ordering::less);
    CHECK(lexicographical_compare_three_way(Iter1(ia), Sent1(ia+sa), Iter2(ib), Sent2(ib+3), c) == std::weak_ordering::greater);
    CHECK(lexicographical_compare_three_way(Iter1(ib), Sent1(ib+3), Iter2(ia), Sent2(ia+sa), c) == std::weak_ordering::less);
    CHECK(lexicographical_compare_three_way(Iter1(ia), Sent1(ia+sa), Iter2(ib+1), Sent2(ib+3), c) == std::weak_ordering::less);
    CHECK(lexicographical_compare_three_way(Iter1(ib+1), Sent1(ib+3), Iter2(ia), Sent2(ia+sa), c) == std::weak_ordering::greater);
}

void test_iter_comp()
{
    typedef Sentinel<const int*> S;

    test_iter_comp1<InputIterator<const int*>, InputIterator<const int*> >();
    test_iter_comp1<InputIterator<const int*>, ForwardIterator<const int*> >();
    test_iter_comp1<InputIterator<const int*>, BidirectionalIterator<const int*> >();
    test_iter_comp1<InputIterator<const int*>, RandomAccessIterator<const int*> >();
    test_iter_comp1<InputIterator<const int*>, InputIterator<const int*>, S, S>();
    test_iter_comp1<InputIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_iter_comp1<InputIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_iter_comp1<InputIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_iter_comp1<InputIterator<const int*>, const int*>();

    test_iter_comp1<ForwardIterator<const int*>, InputIterator<const int*> >();
    test_iter_comp1<ForwardIterator<const int*>, ForwardIterator<const int*> >();
    test_iter_comp1<ForwardIterator<const int*>, BidirectionalIterator<const int*> >();
    test_iter_comp1<ForwardIterator<const int*>, RandomAccessIterator<const int*> >();
    test_iter_comp1<ForwardIterator<const int*>, InputIterator<const int*>, S, S>();
    test_iter_comp1<ForwardIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_iter_comp1<ForwardIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_iter_comp1<ForwardIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_iter_comp1<ForwardIterator<const int*>, const int*>();

    test_iter_comp1<BidirectionalIterator<const int*>, InputIterator<const int*> >();
    test_iter_comp1<BidirectionalIterator<const int*>, ForwardIterator<const int*> >();
    test_iter_comp1<BidirectionalIterator<const int*>, BidirectionalIterator<const int*> >();
    test_iter_comp1<BidirectionalIterator<const int*>, RandomAccessIterator<const int*> >();
    test_iter_comp1<BidirectionalIterator<const int*>, InputIterator<const int*>, S, S>();
    test_iter_comp1<BidirectionalIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_iter_comp1<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_iter_comp1<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_iter_comp1<BidirectionalIterator<const int*>, const int*>();

    test_iter_comp1<RandomAccessIterator<const int*>, InputIterator<const int*> >();
    test_iter_comp1<RandomAccessIterator<const int*>, ForwardIterator<const int*> >();
    test_iter_comp1<RandomAccessIterator<const int*>, BidirectionalIterator<const int*> >();
    test_iter_comp1<RandomAccessIterator<const int*>, RandomAccessIterator<const int*> >();
    test_iter_comp1<RandomAccessIterator<const int*>, InputIterator<const int*>, S, S>();
    test_iter_comp1<RandomAccessIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_iter_comp1<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_iter_comp1<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_iter_comp1<RandomAccessIterator<const int*>, const int*>();

    test_iter_comp1<const int*, InputIterator<const int*> >();
    test_iter_comp1<const int*, ForwardIterator<const int*> >();
    test_iter_comp1<const int*, BidirectionalIterator<const int*> >();
    test_iter_comp1<const int*, RandomAccessIterator<const int*> >();
    test_iter_comp1<const int*, InputIterator<const int*>, const int*, S>();
    test_iter_comp1<const int*, ForwardIterator<const int*>, const int*, S>();
    test_iter_comp1<const int*, BidirectionalIterator<const int*>, const int*, S>();
    test_iter_comp1<const int*, RandomAccessIterator<const int*>, const int*, S>();
    test_iter_comp1<const int*, const int*>();
}

template<class Iter1, class Iter2, class Sent1 = Iter1, class Sent2 = Iter2>
void
test_range1()
{
    int ia[] = {1, 2, 3, 4};
    constexpr auto sa = ranges::size(ia);
    int ib[] = {1, 2, 3};
    using ranges::lexicographical_compare_three_way;
    {
        auto r = ranges::subrange(Iter1(ia), Sent1(ia+sa));
        CHECK(lexicographical_compare_three_way(r, r) == std::strong_ordering::equal);
    }
    {
        auto r1 = ranges::subrange(Iter1(ia), Sent1(ia+sa));
        auto r2 = ranges::subrange(Iter2(ib), Sent2(ib+2));
        CHECK(lexicographical_compare_three_way(r1, r2) == std::strong_ordering::greater);
        CHECK(lexicographical_compare_three_way(r2, r1) == std::strong_ordering::less);
    }
    {
        auto r1 = ranges::subrange(Iter1(ia), Sent1(ia+sa));
        auto r2 = ranges::subrange(Iter2(ib), Sent2(ib+3));
        CHECK(lexicographical_compare_three_way(r1, r2) == std::strong_ordering::greater);
        CHECK(lexicographical_compare_three_way(r2, r1) == std::strong_ordering::less);
    }
    {
        auto r1 = ranges::subrange(Iter1(ia), Sent1(ia+sa));
        auto r2 = ranges::subrange(Iter2(ib+1), Sent2(ib+3));
        CHECK(lexicographical_compare_three_way(r1, r2) == std::strong_ordering::less);
        CHECK(lexicographical_compare_three_way(r2, r1) == std::strong_ordering::greater);
    }
}

void test_range()
{
    typedef Sentinel<const int*> S;

    test_range1<InputIterator<const int*>, InputIterator<const int*> >();
    test_range1<InputIterator<const int*>, ForwardIterator<const int*> >();
    test_range1<InputIterator<const int*>, BidirectionalIterator<const int*> >();
    test_range1<InputIterator<const int*>, RandomAccessIterator<const int*> >();
    test_range1<InputIterator<const int*>, InputIterator<const int*>, S, S>();
    test_range1<InputIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_range1<InputIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_range1<InputIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_range1<InputIterator<const int*>, const int*>();

    test_range1<ForwardIterator<const int*>, InputIterator<const int*> >();
    test_range1<ForwardIterator<const int*>, ForwardIterator<const int*> >();
    test_range1<ForwardIterator<const int*>, BidirectionalIterator<const int*> >();
    test_range1<ForwardIterator<const int*>, RandomAccessIterator<const int*> >();
    test_range1<ForwardIterator<const int*>, InputIterator<const int*>, S, S>();
    test_range1<ForwardIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_range1<ForwardIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_range1<ForwardIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_range1<ForwardIterator<const int*>, const int*>();

    test_range1<BidirectionalIterator<const int*>, InputIterator<const int*> >();
    test_range1<BidirectionalIterator<const int*>, ForwardIterator<const int*> >();
    test_range1<BidirectionalIterator<const int*>, BidirectionalIterator<const int*> >();
    test_range1<BidirectionalIterator<const int*>, RandomAccessIterator<const int*> >();
    test_range1<BidirectionalIterator<const int*>, InputIterator<const int*>, S, S>();
    test_range1<BidirectionalIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_range1<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_range1<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_range1<BidirectionalIterator<const int*>, const int*>();

    test_range1<RandomAccessIterator<const int*>, InputIterator<const int*> >();
    test_range1<RandomAccessIterator<const int*>, ForwardIterator<const int*> >();
    test_range1<RandomAccessIterator<const int*>, BidirectionalIterator<const int*> >();
    test_range1<RandomAccessIterator<const int*>, RandomAccessIterator<const int*> >();
    test_range1<RandomAccessIterator<const int*>, InputIterator<const int*>, S, S>();
    test_range1<RandomAccessIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_range1<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_range1<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_range1<RandomAccessIterator<const int*>, const int*>();

    test_range1<const int*, InputIterator<const int*> >();
    test_range1<const int*, ForwardIterator<const int*> >();
    test_range1<const int*, BidirectionalIterator<const int*> >();
    test_range1<const int*, RandomAccessIterator<const int*> >();
    test_range1<const int*, InputIterator<const int*>, const int*, S>();
    test_range1<const int*, ForwardIterator<const int*>, const int*, S>();
    test_range1<const int*, BidirectionalIterator<const int*>, const int*, S>();
    test_range1<const int*, RandomAccessIterator<const int*>, const int*, S>();
    test_range1<const int*, const int*>();
}

template<class Iter1, class Iter2, class Sent1 = Iter1, class Sent2 = Iter2>
void
test_range_comp1()
{
    int ia[] = {1, 2, 3, 4};
    const unsigned sa = sizeof(ia)/sizeof(ia[0]);
    int ib[] = {1, 2, 3};
    auto c = [](auto const x, auto const y) {
        return static_cast<std::weak_ordering>(x <=> y);
    };
    using ranges::lexicographical_compare_three_way;
    {
        auto r = ranges::subrange(Iter1(ia), Sent1(ia+sa));
        CHECK(lexicographical_compare_three_way(r, r, c) == std::weak_ordering::equivalent);
    }
    {
        auto r1 = ranges::subrange(Iter1(ia), Sent1(ia+sa));
        auto r2 = ranges::subrange(Iter2(ib), Sent2(ib+2));
        CHECK(lexicographical_compare_three_way(r1, r2, c) == std::weak_ordering::greater);
        CHECK(lexicographical_compare_three_way(r2, r1, c) == std::weak_ordering::less);
    }
    {
        auto r1 = ranges::subrange(Iter1(ia), Sent1(ia+sa));
        auto r2 = ranges::subrange(Iter2(ib), Sent2(ib+3));
        CHECK(lexicographical_compare_three_way(r1, r2, c) == std::weak_ordering::greater);
        CHECK(lexicographical_compare_three_way(r2, r1, c) == std::weak_ordering::less);
    }
    {
        auto r1 = ranges::subrange(Iter1(ia), Sent1(ia+sa));
        auto r2 = ranges::subrange(Iter2(ib+1), Sent2(ib+3));
        CHECK(lexicographical_compare_three_way(r1, r2, c) == std::weak_ordering::less);
        CHECK(lexicographical_compare_three_way(r2, r1, c) == std::weak_ordering::greater);
    }
}

void test_range_comp()
{
    typedef Sentinel<const int*> S;

    test_range_comp1<InputIterator<const int*>, InputIterator<const int*> >();
    test_range_comp1<InputIterator<const int*>, ForwardIterator<const int*> >();
    test_range_comp1<InputIterator<const int*>, BidirectionalIterator<const int*> >();
    test_range_comp1<InputIterator<const int*>, RandomAccessIterator<const int*> >();
    test_range_comp1<InputIterator<const int*>, InputIterator<const int*>, S, S>();
    test_range_comp1<InputIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_range_comp1<InputIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_range_comp1<InputIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_range_comp1<InputIterator<const int*>, const int*>();

    test_range_comp1<ForwardIterator<const int*>, InputIterator<const int*> >();
    test_range_comp1<ForwardIterator<const int*>, ForwardIterator<const int*> >();
    test_range_comp1<ForwardIterator<const int*>, BidirectionalIterator<const int*> >();
    test_range_comp1<ForwardIterator<const int*>, RandomAccessIterator<const int*> >();
    test_range_comp1<ForwardIterator<const int*>, InputIterator<const int*>, S, S>();
    test_range_comp1<ForwardIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_range_comp1<ForwardIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_range_comp1<ForwardIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_range_comp1<ForwardIterator<const int*>, const int*>();

    test_range_comp1<BidirectionalIterator<const int*>, InputIterator<const int*> >();
    test_range_comp1<BidirectionalIterator<const int*>, ForwardIterator<const int*> >();
    test_range_comp1<BidirectionalIterator<const int*>, BidirectionalIterator<const int*> >();
    test_range_comp1<BidirectionalIterator<const int*>, RandomAccessIterator<const int*> >();
    test_range_comp1<BidirectionalIterator<const int*>, InputIterator<const int*>, S, S>();
    test_range_comp1<BidirectionalIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_range_comp1<BidirectionalIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_range_comp1<BidirectionalIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_range_comp1<BidirectionalIterator<const int*>, const int*>();

    test_range_comp1<RandomAccessIterator<const int*>, InputIterator<const int*> >();
    test_range_comp1<RandomAccessIterator<const int*>, ForwardIterator<const int*> >();
    test_range_comp1<RandomAccessIterator<const int*>, BidirectionalIterator<const int*> >();
    test_range_comp1<RandomAccessIterator<const int*>, RandomAccessIterator<const int*> >();
    test_range_comp1<RandomAccessIterator<const int*>, InputIterator<const int*>, S, S>();
    test_range_comp1<RandomAccessIterator<const int*>, ForwardIterator<const int*>, S, S>();
    test_range_comp1<RandomAccessIterator<const int*>, BidirectionalIterator<const int*>, S, S>();
    test_range_comp1<RandomAccessIterator<const int*>, RandomAccessIterator<const int*>, S, S>();
    test_range_comp1<RandomAccessIterator<const int*>, const int*>();

    test_range_comp1<const int*, InputIterator<const int*> >();
    test_range_comp1<const int*, ForwardIterator<const int*> >();
    test_range_comp1<const int*, BidirectionalIterator<const int*> >();
    test_range_comp1<const int*, RandomAccessIterator<const int*> >();
    test_range_comp1<const int*, InputIterator<const int*>, const int*, S>();
    test_range_comp1<const int*, ForwardIterator<const int*>, const int*, S>();
    test_range_comp1<const int*, BidirectionalIterator<const int*>, const int*, S>();
    test_range_comp1<const int*, RandomAccessIterator<const int*>, const int*, S>();
    test_range_comp1<const int*, const int*>();
}
#endif // spaceship supported
#endif // __cplusplus

int main()
{
#if __cplusplus > 201703L
#if defined(__cpp_impl_three_way_comparison) && defined(__cpp_lib_three_way_comparison)
    ::test_iter();
    ::test_iter_comp();

    ::test_range();
    ::test_range_comp();

    return ::test_result();
#endif // spaceship supported
#endif // __cplusplus
}
