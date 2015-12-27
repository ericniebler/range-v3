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

#include <vector>
#include <sstream>
#include <iostream>
#include <range/v3/utility/variant.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

#include <range/v3/utility/fixed_vector.hpp>
#include <range/v3/view/repeat_n.hpp>

struct S
{
    static std::size_t s_instances;
    S() {++s_instances;}
    S(S const &) {++s_instances;}
    S(S &&) {++s_instances;}
    S &operator=(S &&) = default;
    S &operator=(S const &) = default;
    ~S() {--s_instances;}
};

std::size_t S::s_instances = 0;

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

    // constexpr variant
    {
        constexpr variant<int, short> v{emplaced_index<1>, (short)2};
        static_assert(v.index() == 1,"");
        static_assert(v.valid(),"");
    }

    // Variant and arrays
    {
        variant<int[5], std::vector<int>> v{emplaced_index<0>, {1,2,3,4,5}};
        int (&rgi)[5] = get<0>(v);
        check_equal(rgi, {1,2,3,4,5});

        variant<int[5], std::vector<int>> v2{emplaced_index<0>, {}};
        int (&rgi2)[5] = get<0>(v2);
        check_equal(rgi2, {0,0,0,0,0});

        v2 = v;
        check_equal(rgi2, {1,2,3,4,5});

        struct T
        {
            T() = delete;
            T(int) {}
            T(T const &) = default;
        };

        // Should compile and not assert at runtime.
        variant<T[5]> vrgt{emplaced_index<0>, {T{42},T{42},T{42},T{42},T{42}}};
        (void) vrgt;
    }

    // fixed_vector
    {
        fixed_vector<S, 10> v{};
        CHECK(v.size() == 0u);
        CHECK(S::s_instances == 0u);
        v = view::repeat_n(S{},5);
        CHECK(v.size() == 5u);
        CHECK(S::s_instances == 5u);
        fixed_vector<S, 10> v2 = view::repeat_n(S{}, 6);
        CHECK(v2.size() == 6u);
        CHECK(S::s_instances == 11u);
        v = v2;
        CHECK(v.size() == 6u);
        CHECK(S::s_instances == 12u);
        v2 = view::repeat_n(S{}, 2);
        CHECK(v2.size() == 2u);
        CHECK(S::s_instances == 8u);

        fixed_vector<int, 100> vi{1,2,3,4,5};
        vi.insert(vi.begin() + 2, 42);
        check_equal(vi, {1,2,42,3,4,5});
        vi.insert(vi.end()-1,3u,55);
        check_equal(vi, {1,2,42,3,4,55,55,55,5});

        std::vector<int> stdvi{99,98,97,96};
        vi.insert(vi.end()-3, stdvi.begin(), stdvi.end());
        check_equal(vi, {1,2,42,3,4,55,99,98,97,96,55,55,5});

        std::stringstream str{"11 22 33 44"};
        vi.insert(vi.end()-2, std::istream_iterator<int>(str), std::istream_iterator<int>());
        check_equal(vi, {1,2,42,3,4,55,99,98,97,96,55,11,22,33,44,55,5});
    }

    return ::test_result();
}
