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
#include <range/v3/core.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/action/sort.hpp>
#include "./simple_test.hpp"
#include "./test_utils.hpp"

template<typename T>
struct vector_like : std::vector<T> {
    using std::vector<T>::vector;

    using typename std::vector<T>::size_type;

    size_type last_reservation{};
    size_type reservation_count{};

    void reserve(size_type n) {
      std::vector<T>::reserve(n);
      last_reservation = n;
      ++reservation_count;
    }
};

int main()
{
    using namespace ranges;

    auto lst0 = view::ints | view::transform([](int i){return i*i;}) | view::take(10)
        | to_<std::list>();
    static_assert((bool)Same<decltype(lst0), std::list<int>>(), "");
    ::check_equal(lst0, {0,1,4,9,16,25,36,49,64,81});

    auto vec0 = view::ints | view::transform([](int i){return i*i;}) | view::take(10)
        | to_vector | action::sort(std::greater<int>{});
    static_assert((bool)Same<decltype(vec0), std::vector<int>>(), "");
    ::check_equal(vec0, {81,64,49,36,25,16,9,4,1,0});

    auto vec1 = view::ints | view::transform([](int i){return i*i;}) | view::take(10)
        | to_<std::vector<long>>() | action::sort(std::greater<long>{});
    static_assert((bool)Same<decltype(vec1), std::vector<long>>(), "");
    ::check_equal(vec1, {81,64,49,36,25,16,9,4,1,0});

    const std::size_t N = 4096;
    auto vl = view::iota(0, int{N}) | to_<vector_like<int>>();
    static_assert((bool)Same<decltype(vl), vector_like<int>>(), "");
    CHECK(vl.reservation_count == std::size_t{1});
    CHECK(vl.last_reservation == N);

    return ::test_result();
}
