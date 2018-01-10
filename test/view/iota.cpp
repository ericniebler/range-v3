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

#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/indices.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/c_str.hpp>
#include <range/v3/utility/copy.hpp>
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

template<typename Integral>
void test_iota_minus() {
    using namespace ranges;
    using D = detail::iota_difference_t<Integral>;
    using I = Integral;
    Integral max = std::numeric_limits<Integral>::max();

    CHECK(detail::iota_minus_(I(0), I(0)) == D(0));
    CHECK(detail::iota_minus_(I(0), I(1)) == D(-1));
    CHECK(detail::iota_minus_(I(1), I(0)) ==  D(1));
    CHECK(detail::iota_minus_(I(1), I(1)) == D(0));

    CHECK(detail::iota_minus_(I(max - I(1)), I(max - I(1))) == D(0));
    CHECK(detail::iota_minus_(I(max - I(1)), I(max)) == D(-1));
    CHECK(detail::iota_minus_(I(max), I(max - I(1))) == D(1));
    CHECK(detail::iota_minus_(I(max), I(max)) == D(0));
}

template <typename U>
void test_wrap_around() {
    using namespace ranges;
    auto max = std::numeric_limits<U>::max();
    U from = max - 2;
    U to = 2;

    auto rng = view::ints(from, to);
    static_assert(RandomAccessRange<decltype(rng)>(), "");
    static_assert(SizedRange<decltype(rng)>(), "");
    models<concepts::RandomAccessView>(aux::copy(rng));
    models<concepts::BoundedView>(aux::copy(rng));
    CHECK(ranges::size(rng) == std::size_t(5));
    ::check_equal(rng, {U(max - 2), U(max - 1), max, U(0), U(1)});

    auto rng2 = view::indices(from, to);
    static_assert(RandomAccessRange<decltype(rng2)>(), "");
    static_assert(SizedRange<decltype(rng2)>(), "");
    models<concepts::RandomAccessView>(aux::copy(rng2));
    models<concepts::BoundedView>(aux::copy(rng2));
    CHECK(ranges::size(rng2) == std::size_t(5));
    ::check_equal(rng2, {U(max - 2), U(max - 1), max, U(0), U(1)});

    auto rng3 = view::closed_indices(from, to);
    static_assert(RandomAccessRange<decltype(rng3)>(), "");
    static_assert(SizedRange<decltype(rng3)>(), "");
    models<concepts::RandomAccessView>(aux::copy(rng3));
    models<concepts::BoundedView>(aux::copy(rng3));
    CHECK(ranges::size(rng3) == std::size_t(6));
    ::check_equal(rng3, {U(max - 2), U(max - 1), max, U(0), U(1), U(2)});
}

int main()
{
    using namespace ranges;

    {
      static_assert(RandomAccessRange<decltype(view::iota(0))>(), "");
      static_assert(!SizedRange<decltype(view::iota(0))>(), "");
    }

    char const *sz = "hello world";
    ::check_equal(view::iota(forward_iterator<char const*>(sz)) | view::take(10) | view::indirect,
        {'h','e','l','l','o',' ','w','o','r','l'});

    ::check_equal(view::ints | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(0) | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(0,9), {0,1,2,3,4,5,6,7,8});
    ::check_equal(view::closed_indices(0,9), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(1,10), {1,2,3,4,5,6,7,8,9});
    ::check_equal(view::closed_indices(1,10), {1,2,3,4,5,6,7,8,9,10});

    auto chars = view::ints(std::numeric_limits<char>::min(),
                            std::numeric_limits<char>::max());
    static_assert(RandomAccessRange<decltype(chars)>(), "");
    static_assert(Same<int, range_difference_type_t<decltype(chars)>>(), "");
    ::models<concepts::RandomAccessView>(aux::copy(chars));
    models<concepts::BoundedView>(aux::copy(chars));

    auto shorts = view::ints(std::numeric_limits<unsigned short>::min(),
                             std::numeric_limits<unsigned short>::max());
    models<concepts::BoundedView>(aux::copy(shorts));
    static_assert(Same<int, range_difference_type_t<decltype(shorts)>>(), "");

    auto uints = view::closed_indices(
        std::numeric_limits<std::uint_least32_t>::min(),
        std::numeric_limits<std::uint_least32_t>::max() - 1);
    models<concepts::BoundedView>(aux::copy(uints));
    static_assert(Same<std::int_fast64_t, range_difference_type_t<decltype(uints)>>(), "");
    static_assert(Same<std::uint_fast64_t, range_size_type_t<decltype(uints)>>(), "");
    CHECK(uints.size() == std::numeric_limits<std::uint32_t>::max());

    auto ints = view::closed_indices(
        std::numeric_limits<std::int_least32_t>::min(),
        std::numeric_limits<std::int_least32_t>::max() - 1);
    static_assert(Same<std::int_fast64_t, range_difference_type_t<decltype(ints)>>(), "");
    static_assert(Same<std::uint_fast64_t, range_size_type_t<decltype(ints)>>(), "");
    CHECK(ints.size() == std::numeric_limits<std::uint32_t>::max());

    {
        auto ints = view::closed_iota(Int{0}, Int{10});
        ::check_equal(ints, {Int{0},Int{1},Int{2},Int{3},Int{4},Int{5},Int{6},Int{7},Int{8},Int{9},Int{10}});
        models_not<concepts::BoundedView>(aux::copy(ints));
    }

    {  // iota minus tests
        test_iota_minus<int8_t>();
        test_iota_minus<int16_t>();
        test_iota_minus<int32_t>();
        test_iota_minus<int64_t>();

        test_iota_minus<uint8_t>();
        test_iota_minus<uint16_t>();
        test_iota_minus<uint32_t>();
        test_iota_minus<uint64_t>();
    }

    {
        // https://github.com/ericniebler/range-v3/issues/506
        auto cstr = view::c_str((const char*)"hello world");
        auto cstr2 = view::iota(cstr.begin(), cstr.end()) | view::indirect;
        ::check_equal(cstr2, std::string("hello world"));
        auto i = cstr2.begin();
        i += 4;
        CHECK(*i == 'o');
        CHECK((i - cstr2.begin()) == 4);
    }

    // {   // unsigned integer wrap around:
    //     test_wrap_around<uint8_t>();
    //     test_wrap_around<uint16_t>();
    //     test_wrap_around<uint32_t>();
    //     test_wrap_around<uint64_t>();
    // }

    {  // test view::indices/closed_indices
        ::check_equal(view::indices | view::take(10), std::initializer_list<std::ptrdiff_t>{0,1,2,3,4,5,6,7,8,9});
        ::check_equal(view::closed_indices | view::take(10), std::initializer_list<std::ptrdiff_t>{0,1,2,3,4,5,6,7,8,9});

        ::check_equal(view::indices(0, 10), {0,1,2,3,4,5,6,7,8,9});
        ::check_equal(view::closed_indices(0, 10), {0,1,2,3,4,5,6,7,8,9,10});

        ::check_equal(view::indices(10), {0,1,2,3,4,5,6,7,8,9});
        ::check_equal(view::closed_indices(10), {0,1,2,3,4,5,6,7,8,9,10});
    }

    return ::test_result();
}
