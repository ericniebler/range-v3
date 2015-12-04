// Range v3 library
//
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <iostream>
#include <sstream>
#include <range/v3/utility/variant.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    // Simple variant and access.
    {
        variant<int, short> v;
        CHECK(v.index() == 0u);
        auto v2 = v;
        CHECK(v2.index() == 0u);
        v.emplace<1>((short)2);
        CHECK(v.index() == 1u);
        CHECK(get<1>(v) == (short)2);
        try
        {
            get<0>(v);
            CHECK(false);
        }
        catch(bad_variant_access)
        {}
        catch(...)
        {
            CHECK(!(bool)"unknown exception");
        }
        v = v2;
        CHECK(v.index() == 0u);
    }

    // variant of void
    {
        variant<void, void> v;
        CHECK(v.index() == 0u);
        v.emplace<0>();
        CHECK(v.index() == 0u);
        try
        {
            // Will only compile if get returns void
            v.index() == 0 ? void() : get<0>(v);
        }
        catch(...)
        {
            CHECK(false);
        }
        v.emplace<1>();
        CHECK(v.index() == 1u);
        try
        {
            get<0>(v);
            CHECK(false);
        }
        catch(bad_variant_access)
        {}
        catch(...)
        {
            CHECK(!(bool)"unknown exception");
        }
    }

    // variant of references
    {
        int i = 42;
        std::string s = "hello world";
        variant<int&, std::string&> v{emplaced_index<0>, i};
        CONCEPT_ASSERT(!DefaultConstructible<variant<int&, std::string&>>());
        CHECK(v.index() == 0u);
        CHECK(get<0>(v) == 42);
        CHECK(&get<0>(v) == &i);
        auto const & cv = v;
        get<0>(cv) = 24;
        CHECK(i == 24);
        v.emplace<1>(s);
        CHECK(v.index() == 1u);
        CHECK(get<1>(v) == "hello world");
        CHECK(&get<1>(v) == &s);
        get<1>(cv) = "goodbye";
        CHECK(s == "goodbye");
    }

    // Move test 1
    {
        variant<int, std::string> v{emplaced_index<1>, "hello world"};
        CHECK(get<1>(v) == "hello world");
        std::string s = get<1>(std::move(v));
        CHECK(s == "hello world");
        CHECK(get<1>(v) == "");
        v.emplace<1>("goodbye");
        CHECK(get<1>(v) == "goodbye");
        auto v2 = std::move(v);
        CHECK(get<1>(v2) == "goodbye");
        CHECK(get<1>(v) == "");
        v = std::move(v2);
        CHECK(get<1>(v) == "goodbye");
        CHECK(get<1>(v2) == "");
    }

    // Move test 2
    {
        std::string s = "hello world";
        variant<std::string&> v{emplaced_index<0>, s};
        CHECK(get<0>(v) == "hello world");
        std::string &s2 = get<0>(std::move(v));
        CHECK(&s2 == &s);
    }

    // Apply test 1
    {
        std::stringstream sout;
        variant<int, std::string> v{emplaced_index<1>, "hello"};
        auto fun = overload(
            [&sout](int&) {sout << "int";},
            [&sout](std::string&)->int {sout << "string"; return 42;});
        variant<void, int> x = v.visit(fun);
        CHECK(sout.str() == "string");
        CHECK(x.index() == 1u);
        CHECK(get<1>(x) == 42);
    }

    // Apply test 2
    {
        std::stringstream sout;
        std::string s = "hello";
        variant<int, std::string&> const v{emplaced_index<1>, s};
        auto fun = overload(
            [&sout](int const&) {sout << "int";},
            [&sout](std::string&)->int {sout << "string"; return 42;});
        variant<void, int> x = v.visit(fun);
        CHECK(sout.str() == "string");
        CHECK(x.index() == 1u);
        CHECK(get<1>(x) == 42);
    }

    return ::test_result();
}
