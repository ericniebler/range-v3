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
#include <range/v3/utility/copy.hpp>
#include <range/v3/view/c_str.hpp>
#include <range/v3/view/indices.hpp>
#include <range/v3/view/indirect.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/drop_exactly.hpp>
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

CPP_template(typename I)(
    requires ranges::Integral<I>)
void test_iota_distance()
{
    using namespace ranges;
    using D = iter_difference_t<I>;
    I max = std::numeric_limits<I>::max();

    CHECK(detail::iota_distance_(I(0), I(0)) == D(0));
    CHECK(detail::iota_distance_(I(1), I(0)) == D(-1));
    CHECK(detail::iota_distance_(I(0), I(1)) ==  D(1));
    CHECK(detail::iota_distance_(I(1), I(1)) == D(0));

    CHECK(detail::iota_distance_(I(max - I(1)), I(max - I(1))) == D(0));
    CHECK(detail::iota_distance_(I(max), I(max - I(1))) == D(-1));
    CHECK(detail::iota_distance_(I(max - I(1)), I(max)) == D(1));
    CHECK(detail::iota_distance_(I(max), I(max)) == D(0));
}

int main()
{
    using namespace ranges;

    {
        CPP_assert(RandomAccessRange<decltype(view::iota((unsigned short)0))>);
        CPP_assert(RandomAccessRange<decltype(view::iota(0))>);
        static_assert(is_infinite<decltype(view::iota(0))>::value, "");
        static_assert(is_infinite<decltype(view::drop_exactly(view::iota(0),0))>::value, "");
        static_assert(!SizedRange<decltype(view::iota(0))>, "");
    }

    char const *sz = "hello world";
    ::check_equal(view::iota(forward_iterator<char const*>(sz)) | view::take(10) | view::indirect,
        {'h','e','l','l','o',' ','w','o','r','l'});

    ::check_equal(view::ints | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(0,unreachable) | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(0,9), {0,1,2,3,4,5,6,7,8});
    ::check_equal(view::closed_indices(0,9), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(1,10), {1,2,3,4,5,6,7,8,9});
    ::check_equal(view::closed_indices(1,10), {1,2,3,4,5,6,7,8,9,10});

    auto chars = view::ints(std::numeric_limits<signed char>::min(),
                            std::numeric_limits<signed char>::max());
    CPP_assert(RandomAccessRange<decltype(chars)>);
    CPP_assert(Same<int, range_difference_t<decltype(chars)>>);
    ::models<RandomAccessViewConcept>(aux::copy(chars));
    models<BoundedViewConcept>(aux::copy(chars));
    CHECK(distance(chars.begin(), chars.end()) == (long) CHAR_MAX - (long) CHAR_MIN);
    CHECK(chars.size() == (unsigned)((long) CHAR_MAX - (long) CHAR_MIN));

    auto ushorts = view::ints(std::numeric_limits<unsigned short>::min(),
                              std::numeric_limits<unsigned short>::max());
    models<BoundedViewConcept>(aux::copy(ushorts));
    CPP_assert(Same<int, range_difference_t<decltype(ushorts)>>);
    CPP_assert(Same<unsigned int, range_size_t<decltype(ushorts)>>);
    CHECK(distance(ushorts.begin(), ushorts.end()) == (int) USHRT_MAX);
    CHECK(ushorts.size() == (unsigned) USHRT_MAX);

    auto uints = view::closed_indices(
        std::numeric_limits<std::uint_least32_t>::min(),
        std::numeric_limits<std::uint_least32_t>::max() - 1);
    models<BoundedViewConcept>(aux::copy(uints));
    CPP_assert(Same<std::int_fast64_t, range_difference_t<decltype(uints)>>);
    CPP_assert(Same<std::uint_fast64_t, range_size_t<decltype(uints)>>);
    CHECK(uints.size() == std::numeric_limits<std::uint32_t>::max());

    auto ints = view::closed_indices(
        std::numeric_limits<std::int_least32_t>::min(),
        std::numeric_limits<std::int_least32_t>::max() - 1);
    CPP_assert(Same<std::int_fast64_t, range_difference_t<decltype(ints)>>);
    CPP_assert(Same<std::uint_fast64_t, range_size_t<decltype(ints)>>);
    CHECK(ints.size() == std::numeric_limits<std::uint32_t>::max());

    auto sints = view::ints(std::numeric_limits<int>::min(),
                            std::numeric_limits<int>::max());
    CPP_assert(RandomAccessRange<decltype(sints)>);
    CPP_assert(Same<std::int_fast64_t, range_difference_t<decltype(sints)>>);
    ::models<RandomAccessViewConcept>(aux::copy(sints));
    models<BoundedViewConcept>(aux::copy(sints));
    CHECK(distance(sints.begin(), sints.end()) == (std::int_fast64_t) INT_MAX - (std::int_fast64_t) INT_MIN);
    CHECK(sints.size() == (std::uint_fast64_t)((std::int_fast64_t) INT_MAX - (std::int_fast64_t) INT_MIN));

    {
        auto ints = view::closed_iota(Int{0}, Int{10});
        ::check_equal(ints, {Int{0},Int{1},Int{2},Int{3},Int{4},Int{5},Int{6},Int{7},Int{8},Int{9},Int{10}});
        models<BoundedViewConcept>(aux::copy(ints));
        models_not<SizedViewConcept>(aux::copy(ints));
        models<ForwardViewConcept>(aux::copy(ints));
        models_not<BidirectionalViewConcept>(aux::copy(ints));
    }

    {
        auto ints = view::closed_iota(0, 10);
        ::check_equal(ints, {0,1,2,3,4,5,6,7,8,9,10});
        models<BoundedViewConcept>(aux::copy(ints));
        models<SizedViewConcept>(aux::copy(ints));
        models<RandomAccessViewConcept>(aux::copy(ints));
        CHECK(size(ints) == 11u);
        auto it = ints.begin(), e = ints.end(), be = e;
        --be;
        using D = range_difference_t<decltype(ints)>;
        // CHECK op++ and op-
        for(D i = 0; ; ++i)
        {
            CHECK((e - it) == (11 - i));
            CHECK((it - e) == -(11 - i));
            CHECK((be - it) == (10 - i));
            CHECK((it - be) == -(10 - i));
            if(i == 11) break;
            ++it;
        }
        // CHECK op-- and op-
        for(D i = 11; ; --i)
        {
            CHECK((e - it) == (11 - i));
            CHECK((it - e) == -(11 - i));
            CHECK((be - it) == (10 - i));
            CHECK((it - be) == -(10 - i));
            if(i == 0) break;
            --it;
        }
        // CHECK op+= and op-
        for(D i = 0; ; ++i)
        {
            it = next(ints.begin(), i);
            CHECK((e - it) == (11 - i));
            CHECK((it - e) == -(11 - i));
            CHECK((be - it) == (10 - i));
            CHECK((it - be) == -(10 - i));
            if(i == 11) break;
        }
        // CHECK op-
        CHECK((e - 0) == e);
        CHECK((be - 0) == be);
        CHECK((e - 1) == be);
        CHECK((be - 1) == ints.begin() + 9);
    }

    {  // iota distance tests
        test_iota_distance<int8_t>();
        test_iota_distance<int16_t>();
        test_iota_distance<int32_t>();
        test_iota_distance<int64_t>();

        test_iota_distance<uint8_t>();
        test_iota_distance<uint16_t>();
        test_iota_distance<uint32_t>();
        test_iota_distance<uint64_t>();
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

    {  // test view::indices/closed_indices
        ::check_equal(view::indices | view::take(10), std::initializer_list<std::size_t>{0,1,2,3,4,5,6,7,8,9});

        ::check_equal(view::indices(0, 10), {0,1,2,3,4,5,6,7,8,9});
        ::check_equal(view::closed_indices(0, 10), {0,1,2,3,4,5,6,7,8,9,10});

        ::check_equal(view::indices(10), {0,1,2,3,4,5,6,7,8,9});
        ::check_equal(view::closed_indices(10), {0,1,2,3,4,5,6,7,8,9,10});
    }

    return ::test_result();
}
