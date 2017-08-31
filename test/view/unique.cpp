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

#include <iostream>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/unique.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"


#include <range/v3/view/transform.hpp>

using std::toupper;

// from http://stackoverflow.com/a/2886589/195873
struct ci_char_traits : public std::char_traits<char>
{
    static bool eq(char c1, char c2) { return toupper(c1) == toupper(c2); }
    static bool ne(char c1, char c2) { return toupper(c1) != toupper(c2); }
    static bool lt(char c1, char c2) { return toupper(c1) <  toupper(c2); }
    static int compare(const char* s1, const char* s2, size_t n)
    {
        for(; n != 0; ++s1, ++s2, --n)
        {
            if(toupper(*s1) < toupper(*s2))
                return -1;
            if(toupper(*s1) > toupper(*s2))
                return 1;
        }
        return 0;
    }
    static const char* find(const char* s, int n, char a)
    {
        for(; n-- > 0; ++s)
            if(toupper(*s) != toupper(a))
                break;
        return s;
    }
};

typedef std::basic_string<char, ci_char_traits> ci_string;


int main()
{
    using namespace ranges;

    int rgi[] = {1, 1, 1, 2, 3, 4, 4};
    std::vector<int> out;

    auto && rng = rgi | view::unique;
    has_type<int &>(*begin(rng));
    models<concepts::BoundedView>(aux::copy(rng));
    models_not<concepts::SizedView>(aux::copy(rng));
    models<concepts::ForwardIterator>(begin(rng));
    models_not<concepts::BidirectionalIterator>(begin(rng));
    copy(rng, ranges::back_inserter(out));
    ::check_equal(out, {1, 2, 3, 4});

    std::vector<ci_string> rgs{"hello", "HELLO", "bye", "Bye", "BYE"};
    auto && rng3 = rgs | view::unique;
    has_type<ci_string &>(*begin(rng3));
    models<concepts::ForwardView>(aux::copy(rng3));
    models<concepts::BoundedView>(aux::copy(rng3));
    models_not<concepts::SizedView>(aux::copy(rng3));
    models<concepts::ForwardIterator>(begin(rng3));
    models_not<concepts::BidirectionalIterator>(begin(rng3));
    auto fs = rng3 | view::transform([](ci_string s){return std::string(s.c_str());});
    ::check_equal(fs, {"hello","bye"});

    return test_result();
}
