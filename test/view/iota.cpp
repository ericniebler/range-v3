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
//

#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/algorithm/equal.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

struct Int
{
    using difference_type = int;
    int i = 0;
    Int() = default;
    explicit Int(int i) : i(i) {}
    Int & operator++() {++i; CHECK(i <= 10); return *this;}
    Int operator++(int) {auto tmp = *this; ++*this; return tmp;}
    bool operator==(Int j) const { return i == j.i; }
    bool operator!=(Int j) const { return i != j.i; }
};

int main()
{
    using namespace ranges;

    char const *sz = "hello world";
    ::check_equal(view::iota(forward_iterator<char const*>(sz)) | view::take(10) | view::indirect,
        {'h','e','l','l','o',' ','w','o','r','l'});

    ::check_equal(view::ints | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(0) | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(0,9), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(1,10), {1,2,3,4,5,6,7,8,9,10});

    auto chars = view::ints(std::numeric_limits<char>::min(),
                           std::numeric_limits<char>::max());
    static_assert(Same<int, range_difference_t<decltype(chars)>>(), "");
    ::models<concepts::RandomAccessRange>(chars);

    auto shorts = view::ints(std::numeric_limits<unsigned short>::min(),
                           std::numeric_limits<unsigned short>::max());
    static_assert(Same<int, range_difference_t<decltype(shorts)>>(), "");

    auto uints = view::ints(
        std::numeric_limits<std::uint32_t>::min(),
        std::numeric_limits<std::uint32_t>::max());
    static_assert(Same<std::int64_t, range_difference_t<decltype(uints)>>(), "");
    static_assert(Same<std::uint64_t, range_size_t<decltype(uints)>>(), "");
    CHECK((static_cast<uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1) == uints.size());

    auto ints = view::ints(
        std::numeric_limits<std::int32_t>::min(),
        std::numeric_limits<std::int32_t>::max());
    static_assert(Same<std::int64_t, range_difference_t<decltype(ints)>>(), "");
    static_assert(Same<std::uint64_t, range_size_t<decltype(ints)>>(), "");
    CHECK((static_cast<uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1) == ints.size());

    {
        auto ints = view::iota(Int{0}, Int{10});
        ::check_equal(ints, {Int{0},Int{1},Int{2},Int{3},Int{4},Int{5},Int{6},Int{7},Int{8},Int{9},Int{10}});
    }

#ifdef RANGES_CPP_STD_14_OR_GREATER
    {
        constexpr auto srng1 = view::iota(0, 10);
        constexpr auto srng2 = view::ints(0, 10);
        constexpr auto srng3 = view::ints | view::take(11);
        constexpr auto srng4 = view::ints(0) | view::take(11);
        static_assert(ranges::size(srng1) == 11, "");
        static_assert(ranges::size(srng2) == 11, "");
        static_assert(ranges::size(srng3) == 11, "");
        static_assert(ranges::size(srng4) == 11, "");

        static_assert(ranges::equal(srng1, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), "");
        static_assert(ranges::equal(srng1, srng2), "");
        static_assert(ranges::equal(srng1, srng3), "");
        static_assert(ranges::equal(srng1, srng4), "");
        static_assert(srng1[0] == 0, "");
        static_assert(srng1[10] == 10, "");
        static_assert(srng1[0] == srng2[0], "");
        static_assert(srng1[10] == srng2[10], "");
        static_assert(srng1[0] == srng3[0], "");
        static_assert(srng1[10] == srng3[10], "");
        static_assert(srng1[0] == srng4[0], "");
        static_assert(srng1[10] == srng4[10], "");
    }
#endif

    return ::test_result();
}
