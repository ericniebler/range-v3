//  Copyright Eric Niebler 2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
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

// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#include <functional>
#include <random>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/as_range.hpp>
#include <range/v3/view/generate.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

/// saw tooth pattern generator (e.g. for 3: 0, 1, 2, 0, 1, 2, ...)
template<class T> struct saw_tooth {
  saw_tooth(T t) : n(t) {}
  const T n;
  mutable T x = T{};
  T operator()() const {
    auto v = x;
    if (++x == n) {
      x = T{};
    }
    return v;
  }
};

/// random pattern generator
template<class T>
struct rnd {
  mutable std::mt19937_64 rng;
  mutable std::uniform_int_distribution<T> dist;
  rnd(T from, T to, long long seed = std::mt19937_64::default_seed)
      : rng(seed), dist(from, to) {}
  T operator()() const { return dist(rng); }
};

template<template <class> class Pattern, class T, class... Ts>
auto make_pattern(T t, Ts... ts) -> Pattern<T> { return Pattern<T>{t, ts...}; }

template<class Container, class Pattern>
auto initialize(Container&& c, Pattern p) -> Container {
  auto zr = ranges::view::zip(c, ranges::view::generate(p));
  ranges::for_each(zr, [](auto&& i) { std::get<0>(i) = std::get<1>(i); });
  return std::move(c);
}

template<class Container>
void test_sort(Container& c) {
  auto middle = std::begin(c) + c.size() / 2;
  /// Test sort vector:
  ranges::sort(c);
  CHECK(std::is_sorted(std::begin(c), std::end(c)));
  /// Test sort already sorted:
  ranges::sort(c);
  CHECK(std::is_sorted(std::begin(c), std::end(c)));
  /// Test sort reversedly sorted:
  ranges::reverse(c);
  ranges::sort(c);
  CHECK(std::is_sorted(std::begin(c), std::end(c)));
  /// Test swap ranges 2 pattern
  std::swap_ranges(std::begin(c), middle, middle);
  ranges::sort(c);
  CHECK(std::is_sorted(std::begin(c), std::end(c)));
  /// Test reversed swap ranges 2 pattern
  ranges::reverse(c);
  std::swap_ranges(std::begin(c), middle, middle);
  ranges::sort(c);
  CHECK(std::is_sorted(std::begin(c), std::end(c)));
}

template<class T>
void sort_tests() {
  /// Lengths to test:
  /// 0, 1, 2, 3, 4, 5, 6, 10, 20, 256, 257, 499, 500, 997, 1000, 1009
  std::vector<int> lengths{0, 1, 2, 3, 4, 5, 6, 10, 20, 256, 257,
                           499, 500, 997, 1000, 1009};
  {
    /// Test 10 random lengths between [0, 1500)
    auto rng = ranges::view::take(ranges::view::iota(0), 10);
    lengths.reserve(200);
    auto rg = make_pattern<rnd>(0, 1500);
    ranges::for_each(rng, [&](int) { lengths.push_back(rg()); });
  }

  for (auto length : lengths) {
    /// saw lengths to test: 1, 2, 3, N/2-1, N/2, N/2+1, N-2, N-1, N
    std::vector<int> saw_lengths = {1, 2, 3, length/2 - 1, length/2,
                                    length/2+1, length - 2, length - 1,
                                    length};
    for (auto saw_length : saw_lengths) {
      auto saw_vec
        = initialize(std::vector<T>(length), make_pattern<saw_tooth>(saw_length));
      test_sort(saw_vec);
    }

    /// test random vector
    auto rnd_vec
      = initialize(std::vector<T>(length), make_pattern<rnd>(0, length));
    test_sort(rnd_vec);
  }
}

int main()
{
  sort_tests<int>();
  sort_tests<unsigned>();
  sort_tests<char>();
  sort_tests<double>();

  return ::test_result();
}
