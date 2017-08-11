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

#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/intersperse.hpp>
#include <range/v3/view/delimit.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/to_container.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

template<std::size_t N>
ranges::iterator_range<char const*> c_str(char const (&sz)[N])
{
    return {&sz[0], &sz[N-1]};
}

ranges::delimit_view<ranges::iterator_range<char const *, ranges::unreachable>, char>
c_str_(char const *sz)
{
    return ranges::view::delimit(sz, '\0');
}

int main()
{
    using namespace ranges;

    {
        auto r0 = view::intersperse(c_str("abcde"), ',');
        models<concepts::BoundedRange>(r0);
        CHECK((r0.end() - r0.begin()) == 9);
        CHECK(std::string(r0) == "a,b,c,d,e");
        CHECK(r0.size() == 9u);

        auto r1 = view::intersperse(c_str(""), ',');
        models<concepts::BoundedRange>(r1);
        CHECK(std::string(r1) == "");
        CHECK(r1.size() == 0u);

        auto r2 = view::intersperse(c_str("a"), ',');
        models<concepts::BoundedRange>(r2);
        CHECK(std::string(r2) == "a");
        CHECK(r2.size() == 1u);

        auto r3 = view::intersperse(c_str("ab"), ',');
        models<concepts::BoundedRange>(r3);
        CHECK(std::string(r3) == "a,b");
        CHECK(r3.size() == 3u);
    }

    {
        auto r0 = view::intersperse(c_str("abcde"), ',') | view::reverse;
        models<concepts::BoundedRange>(r0);
        CHECK(std::string(r0) == "e,d,c,b,a");

        auto r1 = view::intersperse(c_str(""), ',') | view::reverse;
        models<concepts::BoundedRange>(r1);
        CHECK(std::string(r1) == "");

        auto r2 = view::intersperse(c_str("a"), ',') | view::reverse;
        models<concepts::BoundedRange>(r2);
        CHECK(std::string(r2) == "a");

        auto r3 = view::intersperse(c_str("ab"), ',') | view::reverse;
        models<concepts::BoundedRange>(r3);
        CHECK(std::string(r3) == "b,a");
    }

    {
        auto r0 = view::intersperse(c_str_("abcde"), ',');
        models_not<concepts::BoundedRange>(r0);
        CHECK(std::string(r0) == "a,b,c,d,e");

        auto r1 = view::intersperse(c_str_(""), ',');
        models_not<concepts::BoundedRange>(r1);
        CHECK(std::string(r1) == "");

        auto r2 = view::intersperse(c_str_("a"), ',');
        models_not<concepts::BoundedRange>(r2);
        CHECK(std::string(r2) == "a");

        auto r3 = view::intersperse(c_str_("ab"), ',');
        models_not<concepts::BoundedRange>(r3);
        CHECK(std::string(r3) == "a,b");
    }

    {
        auto r0 = view::intersperse(c_str("abcde"), ',');
        auto it = r0.begin();
        CHECK(*(it+0) == 'a');
        CHECK(*(it+1) == ',');
        CHECK(*(it+2) == 'b');
        CHECK(*(it+3) == ',');
        CHECK(*(it+4) == 'c');
        CHECK(*(it+5) == ',');
        CHECK(*(it+6) == 'd');
        CHECK(*(it+7) == ',');
        CHECK(*(it+8) == 'e');
        CHECK((it+9) == r0.end());

        it = r0.end();
        CHECK(*(it-9) == 'a');
        CHECK(*(it-8) == ',');
        CHECK(*(it-7) == 'b');
        CHECK(*(it-6) == ',');
        CHECK(*(it-5) == 'c');
        CHECK(*(it-4) == ',');
        CHECK(*(it-3) == 'd');
        CHECK(*(it-2) == ',');
        CHECK(*(it-1) == 'e');

        it = r0.begin();
        CHECK(((it+0)-it) == 0);
        CHECK(((it+1)-it) == 1);
        CHECK(((it+2)-it) == 2);
        CHECK(((it+3)-it) == 3);
        CHECK(((it+4)-it) == 4);
        CHECK(((it+5)-it) == 5);
        CHECK(((it+6)-it) == 6);
        CHECK(((it+7)-it) == 7);
        CHECK(((it+8)-it) == 8);
        CHECK(((it+9)-it) == 9);

        CHECK((it-(it+0)) == 0);
        CHECK((it-(it+1)) == -1);
        CHECK((it-(it+2)) == -2);
        CHECK((it-(it+3)) == -3);
        CHECK((it-(it+4)) == -4);
        CHECK((it-(it+5)) == -5);
        CHECK((it-(it+6)) == -6);
        CHECK((it-(it+7)) == -7);
        CHECK((it-(it+8)) == -8);
        CHECK((it-(it+9)) == -9);
    }

    {
        std::stringstream str{"1 2 3 4 5"};
        auto r0 = istream<int>(str) | view::intersperse(42);
        check_equal(r0, {1,42,2,42,3,42,4,42,5});
    }

    {
        int const some_ints[] = {1,2,3,4,5};
        auto rng = debug_input_view<int const>{some_ints} | view::intersperse(42);
        check_equal(rng, {1,42,2,42,3,42,4,42,5});
    }

    return test_result();
}
