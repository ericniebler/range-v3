// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <range/v3/core.hpp>
#include <range/v3/numeric/adjacent_difference.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"
#include <array>

template<typename TIn, typename TOut, std::size_t N, typename Func>
void test_one(std::array<TIn, N> const& in, std::array<TOut, N> const& expected, Func run) {
    std::array<TOut, N> result{};
    auto r = run(in, result);
    CHECK(base(std::get<0>(r)) == in.data() + in.size());
    CHECK(base(std::get<1>(r)) == result.data() + result.size());
    ::check_equal(result, expected);
}

template<class InIter, class OutIter, class InSent = InIter> void test()
{
    using ranges::adjacent_difference;
    using ranges::make_iterator_range;

    std::array<int, 5> const in{{15, 10, 6, 3, 1}};
    std::array<int, 5> const expected{{15, -5, -4, -3, -2}};

    // iterator
    test_one(in, expected, [](auto const& in, auto& result) {
        return adjacent_difference(InIter(in.data()), InSent(in.data() + in.size()), OutIter(result.data()));
    });

    // range + output iterator
    test_one(in, expected, [](auto const& in, auto& result) {
        auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
        return adjacent_difference(rng, OutIter(result.data()));
    });

    // range + output range
    test_one(in, expected, [](auto const& in, auto& result) {
        auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
        auto orng = make_iterator_range(OutIter(result.data()), OutIter(result.data() + result.size()));
        return adjacent_difference(rng, orng);
    });

    // BinaryOp
    test_one(in, std::array<int, 5>{{15, 25, 16, 9, 4}}, [](auto const& in, auto& result) {
        auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
        auto orng = make_iterator_range(OutIter(result.data()), OutIter(result.data() + result.size()));
        return adjacent_difference(rng, orng, std::plus<int>());
    });
}

int main()
{
    test<input_iterator<const int *>, input_iterator<int *>>();
    test<input_iterator<const int *>, forward_iterator<int *>>();
    test<input_iterator<const int *>, bidirectional_iterator<int *>>();
    test<input_iterator<const int *>, random_access_iterator<int *>>();
    test<input_iterator<const int *>, int *>();

    test<forward_iterator<const int *>, input_iterator<int *>>();
    test<forward_iterator<const int *>, forward_iterator<int *>>();
    test<forward_iterator<const int *>, bidirectional_iterator<int *>>();
    test<forward_iterator<const int *>, random_access_iterator<int *>>();
    test<forward_iterator<const int *>, int *>();

    test<bidirectional_iterator<const int *>, input_iterator<int *>>();
    test<bidirectional_iterator<const int *>, forward_iterator<int *>>();
    test<bidirectional_iterator<const int *>, bidirectional_iterator<int *>>();
    test<bidirectional_iterator<const int *>, random_access_iterator<int *>>();
    test<bidirectional_iterator<const int *>, int *>();

    test<random_access_iterator<const int *>, input_iterator<int *>>();
    test<random_access_iterator<const int *>, forward_iterator<int *>>();
    test<random_access_iterator<const int *>, bidirectional_iterator<int *>>();
    test<random_access_iterator<const int *>, random_access_iterator<int *>>();
    test<random_access_iterator<const int *>, int *>();

    test<const int *, input_iterator<int *>>();
    test<const int *, forward_iterator<int *>>();
    test<const int *, bidirectional_iterator<int *>>();
    test<const int *, random_access_iterator<int *>>();
    test<const int *, int *>();

    using ranges::adjacent_difference;

    { // Test projections
        struct S {
            int i;
        };

        test_one(std::array<S, 5>{{{15}, {10}, {6}, {3}, {1}}}, std::array<int, 5>{{15, -5, -4, -3, -2}},
            [](auto const& in, auto& result) {
                return adjacent_difference(in.data(), in.data() + in.size(), result.data(),
                    std::minus<int>(), &S::i);
            });
   }

    { // Test calling it with an array
        int ia[] = {15, 10, 6, 3, 1};
        int ir[] = {15, 25, 16, 9, 4};
        const unsigned s = sizeof(ir) / sizeof(ir[0]);
        int ib[s] = {0};
        auto r = adjacent_difference(ia, ib, std::plus<int>());
        CHECK(base(std::get<0>(r)) == ia + s);
        CHECK(base(std::get<1>(r)) == ib + s);
        for(unsigned i = 0; i < s; ++i)
        {
            CHECK(ib[i] == ir[i]);
        }
    }

    return ::test_result();
}
