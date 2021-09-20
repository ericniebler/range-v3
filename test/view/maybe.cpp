/// \file
// Range v3 library
//
//  Copyright Hui Xie 2021
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include "range/v3/view/maybe.hpp"

#include <memory>
#include <type_traits>

#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "range/v3/utility/optional.hpp"
#include "range/v3/utility/variant.hpp"

namespace
{
    using namespace ranges;

    void test_types()
    {
        using maybe_int_view = ranges::maybe_view<int *>;
        CPP_assert(common_range<maybe_int_view>);
        CPP_assert(sized_range<maybe_int_view>);
        CPP_assert(view_<maybe_int_view>);
        CPP_assert(common_range<maybe_int_view>);
        CPP_assert(sized_range<maybe_int_view>);
        CPP_assert(random_access_iterator<decltype(maybe_int_view{}.begin())>);
    }

    void test_raw_pointers()
    {
        int i = 5;
        int * maybe_i = &i;
        auto v = views::maybe(maybe_i);
        CHECK(ranges::size(v) == 1u);
        check_equal(v, {5});
        auto it = ranges::begin(v);
        CHECK(detail::addressof(*it) == &i);
        CHECK(++it == ranges::end(v));

        int * nothing = nullptr;
        auto v2 = views::maybe(nothing);
        CHECK(ranges::size(v2) == 0u);
        CHECK(ranges::begin(v2) == ranges::end(v2));
    }

    void test_optional()
    {
        ranges::optional<int> maybe_i = 5;
        auto v = views::maybe(maybe_i);
        CHECK(ranges::size(v) == 1u);
        check_equal(v, {5});
        auto it = ranges::begin(v);
        CHECK(++it == ranges::end(v));

        ranges::optional<int> nothing{nullopt};
        auto v2 = views::maybe(nothing);
        CHECK(ranges::size(v2) == 0u);
        CHECK(ranges::begin(v2) == ranges::end(v2));
    }

    namespace user
    {
        struct Foo
        {};
        struct Bar
        {};
        struct monostate
        {};

        using MyObject = ranges::variant<monostate, Foo, Bar>;

        bool is_just(const MyObject & o)
        {
            return o.valid() && o.index() != 0;
        }

        decltype(auto) get_just(MyObject & o)
        {
            return (o);
        }

        decltype(auto) get_just(const MyObject & o)
        {
            return (o);
        }
    } // namespace user

    void test_custom_types()
    {
        user::MyObject o{emplaced_index<1u>};
        auto v = views::maybe(o);
        CHECK(ranges::size(v) == 1u);
        auto it = ranges::begin(v);
        CPP_assert(std::is_same<decltype(*it), user::MyObject &>::value);
        CHECK(++it == ranges::end(v));

        user::MyObject o2{emplaced_index<2u>};
        const auto v2 = views::maybe(o2);
        CHECK(ranges::size(v) == 1u);
        auto it2 = ranges::begin(v2);
        CPP_assert(std::is_same<decltype(*it2), user::MyObject const &>::value);
        CHECK(++it2 == ranges::end(v2));

        user::MyObject nothing{emplaced_index<0u>};
        auto v3 = views::maybe(nothing);
        CHECK(ranges::size(v3) == 0u);
        CHECK(ranges::begin(v3) == ranges::end(v3));
    }

} // namespace

int main()
{
    test_types();
    test_raw_pointers();
    test_optional();
    test_custom_types();
    return ::test_result();
}
