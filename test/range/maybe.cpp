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

#include "range/v3/range/maybe.hpp"

#include <memory>
#include <type_traits>

#include "../simple_test.hpp"
#include "range/v3/utility/optional.hpp"
#include "range/v3/utility/variant.hpp"

namespace
{
    using namespace ranges;

    void test_raw_pointers()
    {
        CPP_assert(ranges::maybe<int *>);
        CPP_assert(ranges::maybe<const double *>);

        int i = 5;
        int * maybe_i = &i;
        CHECK(is_just(maybe_i));
        CPP_assert(std::is_same<decltype(ranges::get_just(maybe_i)), int &>::value);
        CHECK(&(get_just(maybe_i)) == &i);

        int * nothing = nullptr;
        CHECK(!is_just(nothing));
    }

    void test_smart_pointers()
    {
        CPP_assert(ranges::maybe<std::unique_ptr<int>>);
        CPP_assert(ranges::maybe<std::shared_ptr<const double>>);

        auto maybe_i = std::make_unique<int>(5);
        CHECK(is_just(maybe_i));
        CPP_assert(std::is_same<decltype(ranges::get_just(maybe_i)), int &>::value);
        CHECK(&(get_just(maybe_i)) == maybe_i.get());

        std::unique_ptr<int> nothing{};
        CHECK(!is_just(nothing));
    }

    void test_optional()
    {
        CPP_assert(ranges::maybe<ranges::optional<int>>);
        ranges::optional<int> maybe_i{5};
        CHECK(is_just(maybe_i));
        CPP_assert(std::is_same<decltype(ranges::get_just(maybe_i)), int &>::value);
        CHECK(&(get_just(maybe_i)) == &(maybe_i.value()));

        ranges::optional<int> nothing = ranges::nullopt;
        CHECK(!is_just(nothing));
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
        CPP_assert(ranges::maybe<user::MyObject>);
        user::MyObject o{emplaced_index<1U>};
        CHECK(ranges::is_just(o));
        CPP_assert(std::is_same<decltype(ranges::get_just(o)), user::MyObject &>::value);
        CHECK(&ranges::get_just(o) == &o);

        const user::MyObject o2{emplaced_index<2U>};
        CPP_assert(
            std::is_same<decltype(ranges::get_just(o2)), const user::MyObject &>::value);
        CHECK(&ranges::get_just(o2) == &o2);

        user::MyObject nothing{};
        CHECK(!is_just(nothing));
    }

    void non_maybe_types()
    {
        CPP_assert(!ranges::maybe<int>);
        CPP_assert(!ranges::maybe<bool>);
        CPP_assert(!ranges::maybe<std::nullptr_t>);
    }

} // namespace

int main()
{
    test_raw_pointers();
    test_smart_pointers();
    test_optional();
    test_custom_types();
    non_maybe_types();
    return ::test_result();
}
