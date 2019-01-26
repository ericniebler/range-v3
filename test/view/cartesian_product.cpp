/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014.
//  Copyright Casey Carter 2017.
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <iostream>
#include <range/v3/range/access.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range_for.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/filter.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

RANGES_DIAGNOSTIC_IGNORE_RANGE_LOOP_ANALYSIS

using namespace ranges;

struct printer {
    std::ostream &os_;
    bool &first_;

    template<typename T>
    void operator()(T const &t) const
    {
        if (first_) first_ = false;
        else os_ << ',';
        os_ << t;
    }
};

namespace std {
    template<typename... Ts>
    std::ostream &operator<<(std::ostream &os, std::tuple<Ts...> const &t)
    {
        os << '(';
        auto first = true;
        ::ranges::tuple_for_each(t, ::printer{os, first});
        os << ')';
        return os;
    }
}

void test_empty_set()
{
    auto rng = view::cartesian_product();
    using Rng = decltype(rng);
    CPP_assert(range_cardinality<Rng>::value ==
        static_cast<cardinality>(0));

    CPP_assert(RandomAccessView<Rng>);
    CPP_assert(CommonRange<Rng>);
    CPP_assert(SizedRange<Rng>);
    CHECK(size(rng) == 0u);
    CHECK(empty(rng));

    CPP_assert(std::is_same<
        range_value_t<Rng>,
        std::tuple<>>());
    CPP_assert(std::is_same<
        range_reference_t<Rng>,
        std::tuple<>&>());

    std::initializer_list<common_tuple<>> control{};
    ::check_equal(rng, control);
    ::check_equal(view::reverse(rng), view::reverse(control));

    auto const first = begin(rng);
    auto const last = end(rng);
    CHECK(decltype(size(rng))(last - first) == size(rng));
    for(auto i = 0; i <= distance(rng); ++i)
    {
        for(auto j = 0; j <= distance(rng); ++j)
        {
            CHECK((next(first, i) - next(first, j)) == i - j);
        }
    }
}

void test_empty_range()
{
    int some_ints[] = {0,1,2,3};
    auto e = view::empty<char>;
    auto rng = view::cartesian_product(
        span<int, size(some_ints)>{some_ints},
        e
    );
    using Rng = decltype(rng);
    CPP_assert(range_cardinality<Rng>::value ==
        static_cast<cardinality>(0));

    CPP_assert(RandomAccessView<Rng>);
    CPP_assert(CommonRange<Rng>);
    CPP_assert(SizedRange<Rng>);
    CHECK(size(rng) == 0u);

    CPP_assert(std::is_same<
        range_value_t<Rng>,
        std::tuple<int, char>>());
    CPP_assert(std::is_same<
        range_reference_t<Rng>,
        common_tuple<int &, char &>>());

    using CT = common_tuple<int, char>;
    std::initializer_list<CT> control = {};

    ::check_equal(rng, control);
    ::check_equal(view::reverse(rng), view::reverse(control));

    auto const first = begin(rng);
    auto const last = end(rng);
    CHECK((last - first) == size(rng));
    for(auto i = 0; i <= distance(rng); ++i)
    {
        for(auto j = 0; j <= distance(rng); ++j)
        {
            CHECK((next(first, i) - next(first, j)) == i - j);
        }
    }
}

void test_bug_820()
{
    // https://github.com/ericniebler/range-v3/issues/820
    using CT = common_tuple<int, int>;
    std::initializer_list<CT> control = {
        CT{0, 0}, CT{0, 1}, CT{0, 2},
        CT{1, 0}, CT{1, 1}, CT{1, 2},
        CT{2, 0}, CT{2, 1}, CT{2, 2}
    };

    auto x = ranges::view::iota(0) | ranges::view::take_exactly(3);
    auto y = ranges::view::cartesian_product(x, x);
    ::check_equal(y, control);
}

void test_bug_823()
{
    // https://github.com/ericniebler/range-v3/issues/823
    auto three = ranges::view::iota(0) | ranges::view::take_exactly(3);
    CPP_assert(ranges::RandomAccessView<decltype(three)>);
    CPP_assert(!ranges::RandomAccessView<const decltype(three)>);

    auto prod = ranges::view::cartesian_product(three, three);
    CPP_assert(ranges::RandomAccessView<decltype(prod)>);
    CPP_assert(!ranges::RandomAccessView<const decltype(prod)>);
    CPP_assert(ranges::SizedRange<decltype(prod)>);
    CHECK(ranges::size(prod) == 9u);

    {
        int i = 0;
        RANGES_FOR(auto&& x, prod) {
            (void)x;
            RANGES_ENSURE(i++ < 3 * 3);
        }
        CHECK(i == 3 * 3);
    }

    auto twoD = prod | ranges::view::chunk(3);
    CPP_assert(ranges::RandomAccessView<decltype(twoD)>);
    CPP_assert(!ranges::RandomAccessView<const decltype(twoD)>);

    {
        int i = 0;
        RANGES_FOR(auto&& row, twoD) {
            (void)row;
            RANGES_ENSURE(i++ < 3);
        }
        CHECK(i == 3);
    }

    {
        int i = 0;
        RANGES_FOR(auto&& row, twoD) {
            RANGES_ENSURE(i++ < 3);
            int j = 0;
            RANGES_FOR(auto&& col, row) {
                (void)col;
                RANGES_ENSURE(j++ < 3);
            }
            CHECK(j == 3);
        }
        CHECK(i == 3);
    }
}

void test_bug_919()
{
    // https://github.com/ericniebler/range-v3/issues/919
    int some_ints[] = {0,1,2,3};
    char const * some_strings[] = {"John", "Paul", "George", "Ringo"};
    auto rng = view::cartesian_product(
        span<int, size(some_ints)>{some_ints},
        span<char const*, size(some_strings)>{some_strings}
    );
    constexpr std::intmax_t n = size(rng);
    static_assert(n == 16, "");

    for (std::intmax_t i = 0; i <= n; ++i) {
        auto const x = rng.begin() + i;
        CHECK((x == rng.end() - (n - i)));
        for (std::intmax_t j = 0; j <= n; ++j)
            CHECK((rng.begin() + j == x + (j - i)));
    }
}

// https://github.com/ericniebler/range-v3/issues/978
void test_bug_978()
{
    int data[] = {1};
    ranges::view::cartesian_product(
        data | ranges::view::filter([](int){ return true; }),
        data
    );
}

int main()
{
    int some_ints[] = {0,1,2,3};
    char const * some_strings[] = {"John", "Paul", "George", "Ringo"};
    auto rng = view::cartesian_product(
        span<int, size(some_ints)>{some_ints},
        span<char const*, size(some_strings)>{some_strings}
    );
    using Rng = decltype(rng);
    CPP_assert(range_cardinality<Rng>::value ==
        range_cardinality<decltype(some_ints)>::value *
        range_cardinality<decltype(some_strings)>::value);

    CPP_assert(RandomAccessView<Rng>);
    CPP_assert(CommonRange<Rng>);
    CPP_assert(SizedRange<Rng>);
    CHECK(size(rng) == static_cast<std::intmax_t>(size(some_ints) * size(some_strings)));

    CPP_assert(std::is_same<
        range_value_t<Rng>,
        std::tuple<int, char const *>>());
    CPP_assert(std::is_same<
        range_reference_t<Rng>,
        common_tuple<int &, char const * &>>());

    using CT = common_tuple<int, std::string>;
    std::initializer_list<CT> control = {
        CT{0, "John"}, CT{0, "Paul"}, CT{0, "George"}, CT{0, "Ringo"},
        CT{1, "John"}, CT{1, "Paul"}, CT{1, "George"}, CT{1, "Ringo"},
        CT{2, "John"}, CT{2, "Paul"}, CT{2, "George"}, CT{2, "Ringo"},
        CT{3, "John"}, CT{3, "Paul"}, CT{3, "George"}, CT{3, "Ringo"}
    };

    ::check_equal(rng, control);
    ::check_equal(view::reverse(rng), view::reverse(control));

    auto const first = begin(rng);
    auto const last = end(rng);
    CHECK((last - first) == size(rng));
    for(auto i = 0; i <= distance(rng); ++i)
    {
        for(auto j = 0; j <= distance(rng); ++j)
        {
            CHECK((next(first, i) - next(first, j)) == i - j);
        }
    }

    test_empty_set();
    test_empty_range();
    test_bug_820();
    test_bug_823();
    test_bug_919();
    test_bug_978();

    return test_result();
}
