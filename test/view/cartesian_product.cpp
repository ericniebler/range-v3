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
#include <vector>

#include <range/v3/range/access.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/range/primitives.hpp>
#include <range/v3/range_for.hpp>
#include <range/v3/view/span.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>
#include <range/v3/view/cartesian_product.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/linear_distribute.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/take_exactly.hpp>
#include <range/v3/view/transform.hpp>

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
    auto rng = views::cartesian_product();
    using Rng = decltype(rng);
    CPP_assert(range_cardinality<Rng>::value ==
        static_cast<cardinality>(0));

    CPP_assert(random_access_range<Rng> && view_<Rng>);
    CPP_assert(common_range<Rng>);
    CPP_assert(sized_range<Rng>);
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
    ::check_equal(views::reverse(rng), views::reverse(control));

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
    auto e = views::empty<char>;
    auto rng = views::cartesian_product(
        span<int, size(some_ints)>{some_ints},
        e
    );
    using Rng = decltype(rng);
    CPP_assert(range_cardinality<Rng>::value ==
        static_cast<cardinality>(0));

    CPP_assert(random_access_range<Rng> && view_<Rng>);
    CPP_assert(common_range<Rng>);
    CPP_assert(sized_range<Rng>);
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
    ::check_equal(views::reverse(rng), views::reverse(control));

    auto const first = begin(rng);
    auto const last = end(rng);
    CHECK((last - first) == (std::intmax_t) size(rng));
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

    auto x = ranges::views::iota(0) | ranges::views::take_exactly(3);
    auto y = ranges::views::cartesian_product(x, x);
    ::check_equal(y, control);
}

void test_bug_823()
{
    // https://github.com/ericniebler/range-v3/issues/823
    auto three = ranges::views::iota(0) | ranges::views::take_exactly(3);
    CPP_assert(ranges::random_access_range<decltype(three)> && ranges::view_<decltype(three)>);
    CPP_assert(!(ranges::random_access_range<const decltype(three)> && ranges::view_<const decltype(three)>));

    auto prod = ranges::views::cartesian_product(three, three);
    CPP_assert(ranges::random_access_range<decltype(prod)> && ranges::view_<decltype(prod)>);
    CPP_assert(!(ranges::random_access_range<const decltype(prod)> && ranges::view_<const decltype(prod)>));
    CPP_assert(ranges::sized_range<decltype(prod)>);
    CHECK(ranges::size(prod) == 9u);

    {
        int i = 0;
        RANGES_FOR(auto&& x, prod) {
            (void)x;
            RANGES_ENSURE(i++ < 3 * 3);
        }
        CHECK(i == 3 * 3);
    }

    auto twoD = prod | ranges::views::chunk(3);
    CPP_assert(ranges::random_access_range<decltype(twoD)> && ranges::view_<decltype(twoD)>);
    CPP_assert(!(ranges::random_access_range<const decltype(twoD)> && ranges::view_<const decltype(twoD)>));

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
    auto rng = views::cartesian_product(
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
    ranges::views::cartesian_product(
        data | ranges::views::filter([](int){ return true; }),
        data
    );
}

template<typename ...T>
auto cp(const std::vector<T>& ...a)
{
    return ranges::views::cartesian_product(a...);
}

std::vector<double> distribute(double len, unsigned n)
{
    return ranges::views::linear_distribute(0.0, len, n) | ranges::to<std::vector>();
}

// https://github.com/ericniebler/range-v3/issues/1269
void test_bug_1269_(unsigned size_x, unsigned size_y, unsigned size_z)
{
    auto x = distribute(1.0, size_x);
    CHECK(x.size() == size_x);
    CHECK((int)x.back() == 1);

    auto y = distribute(2.0, size_y);
    CHECK(y.size() == size_y);
    CHECK((int)y.back() == 2);

    auto z = distribute(3.0, size_z);
    CHECK(z.size() == size_z);
    CHECK((int)z.back() == 3);

    auto xyz = cp(x, y, z);
    auto sz = x.size() * y.size() * z.size();

    CHECK(xyz.size() == sz);

    auto t =
        ranges::views::transform([&](const auto& location) {
            return std::get<0>(location) * std::get<1>(location) * std::get<2>(location);
        });
    auto loc = xyz | t | ranges::to<std::vector>();

    CHECK(loc.size() == sz);
    CHECK((int)loc.back() == 6);
}

void test_bug_1269()
{
    test_bug_1269_(5, 4, 6);
    test_bug_1269_(2, 2, 3);
    test_bug_1269_(2, 3, 3);
    test_bug_1269_(3, 3, 4);
    test_bug_1269_(3, 4, 4);
    test_bug_1269_(4, 4, 5);
    test_bug_1269_(4, 5, 5);

    test_bug_1269_(3, 2, 2);
    test_bug_1269_(4, 3, 3);
    test_bug_1269_(5, 4, 4);
    test_bug_1269_(6, 5, 5);
}

int main()
{
    int some_ints[] = {0,1,2,3};
    char const * some_strings[] = {"John", "Paul", "George", "Ringo"};
    auto rng = views::cartesian_product(
        span<int, size(some_ints)>{some_ints},
        span<char const*, size(some_strings)>{some_strings}
    );
    using Rng = decltype(rng);
    CPP_assert(range_cardinality<Rng>::value ==
        range_cardinality<decltype(some_ints)>::value *
        range_cardinality<decltype(some_strings)>::value);

    CPP_assert(random_access_range<Rng> && view_<Rng>);
    CPP_assert(common_range<Rng>);
    CPP_assert(sized_range<Rng>);
    CHECK(size(rng) == size(some_ints) * size(some_strings));

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
    ::check_equal(views::reverse(rng), views::reverse(control));

    auto const first = begin(rng);
    auto const last = end(rng);
    CHECK((last - first) == (std::intmax_t) size(rng));
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
    test_bug_1269();

    return test_result();
}
