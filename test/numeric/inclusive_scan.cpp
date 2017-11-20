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
#include <range/v3/numeric/inclusive_scan.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include "../test_utils.hpp"
#include <array>

template<typename TIn, typename TOut, std::size_t N, typename Func>
void test_one(std::array<TIn, N> const& in, std::array<TOut, N> const& expected, Func run) 
{
    std::array<TOut, N> result{};
    auto r = run(in, result);
    CHECK(base(std::get<0>(r)) == in.data() + in.size());
    CHECK(base(std::get<1>(r)) == result.data() + result.size());
    ::check_equal(result, expected);
}

template<class InIter, class OutIter, class InSent = InIter> void test()
{
    using ranges::inclusive_scan;
    using ranges::make_iterator_range;

    using ArrayT = std::array<int, 5>;

    { // No-init version
        ArrayT const in{{1, 2, 3, 4, 5}};
        ArrayT const expected{{1, 3, 6, 10, 15}};

        // iterator
        test_one(in, expected, [](ArrayT const& in, ArrayT& result) {
            return inclusive_scan(InIter(in.data()), InSent(in.data() + in.size()), OutIter(result.data()));
        });

        // range + output iterator
        test_one(in, expected, [](ArrayT const& in, ArrayT& result) {
            auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
            return inclusive_scan(rng, OutIter(result.data()));
        });

        // range + output range
        test_one(in, expected, [](ArrayT const& in, ArrayT& result) {
            auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
            auto orng = make_iterator_range(OutIter(result.data()), OutIter(result.data() + result.size()));
            return inclusive_scan(rng, orng);
        });

        // BinaryOp
        test_one(in, ArrayT{{1, -1, -4, -8, -13}}, [](ArrayT const& in, ArrayT& result) {
            auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
            auto orng = make_iterator_range(OutIter(result.data()), OutIter(result.data() + result.size()));
            return inclusive_scan(rng, orng, std::minus<int>());
        });
    }

    { // Init version
        ArrayT const in{{1, 2, 3, 4, 5}};
        ArrayT const expected{{10, 12, 15, 19, 24}};
        int const init = 9;

        // iterator
        test_one(in, expected, [](ArrayT const& in, ArrayT& result) {
            return inclusive_scan(InIter(in.data()), InSent(in.data() + in.size()), OutIter(result.data()), std::plus<int>{}, init);
        });

        // range + output iterator
        test_one(in, expected, [](ArrayT const& in, ArrayT& result) {
            auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
            return inclusive_scan(rng, OutIter(result.data()), std::plus<int>{}, init);
        });

        // range + output range
        test_one(in, expected, [](ArrayT const& in, ArrayT& result) {
            auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
            auto orng = make_iterator_range(OutIter(result.data()), OutIter(result.data() + result.size()));
            return inclusive_scan(rng, orng, std::plus<int>{}, init);
        });

        // BinaryOp
        test_one(in, ArrayT{{8, 6, 3, -1, -6}}, [](ArrayT const& in, ArrayT& result) {
            auto rng = make_iterator_range(InIter(in.data()), InSent(in.data() + in.size()));
            auto orng = make_iterator_range(OutIter(result.data()), OutIter(result.data() + result.size()));
            return inclusive_scan(rng, orng, std::minus<int>(), init);
        });
    }
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

    using ranges::inclusive_scan;

    { // Test projections
        struct S {
            int i;
        };

        using SArrayT = std::array<S, 5>;
        using ArrayT = std::array<int, 5>;

        test_one(SArrayT{{{1}, {2}, {3}, {4}, {5}}}, ArrayT{{1, 3, 6, 10, 15}},
            [](SArrayT const& in, ArrayT& result) {
                return inclusive_scan(in.data(), in.data() + in.size(), result.data(),
                    std::plus<int>(), &S::i);
            });

        test_one(SArrayT{{{1}, {2}, {3}, {4}, {5}}}, ArrayT{{10, 12, 15, 19, 24}},
            [](SArrayT const& in, ArrayT& result) {
                return inclusive_scan(in.data(), in.data() + in.size(), result.data(),
                    std::plus<int>(), 9, &S::i);
            });
    }

    { // Test calling it with an array
        int ia[] = {1, 2, 3, 4, 5};
        int ir[] = {1, 2, 6, 24, 120};
        const unsigned s = sizeof(ir) / sizeof(ir[0]);
        int ib[s] = {0};
        auto r = inclusive_scan(ia, ib, std::multiplies<int>());
        CHECK(base(std::get<0>(r)) == ia + s);
        CHECK(base(std::get<1>(r)) == ib + s);
        ::check_equal(ib, ir);
    }

    return ::test_result();
}
