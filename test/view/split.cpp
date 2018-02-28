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

#include <string>
#include <cctype>
#include <range/v3/core.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/c_str.hpp>
#include <range/v3/view/empty.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/split.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_SIGN_CONVERSION

namespace
{
    struct starts_with_g
    {
        template<typename I, typename S>
        std::pair<bool, I> operator()(I b, S) const
        {
            return {*b == 'g', b};
        }
    };

    template<std::size_t N>
    ranges::iterator_range<char const*> c_str(char const (&sz)[N])
    {
        return {&sz[0], &sz[N-1]};
    }
}

int main()
{
    using namespace ranges;

    {
        std::string str("Now is the time for all good men to come to the aid of their country.");
        auto rng = view::split(str, ' ');
        CHECK(distance(rng) == 16);
        if(distance(rng) == 16)
        {
            check_equal(*(next(begin(rng),0)), c_str("Now"));
            check_equal(*(next(begin(rng),1)), c_str("is"));
            check_equal(*(next(begin(rng),2)), c_str("the"));
            check_equal(*(next(begin(rng),3)), c_str("time"));
            check_equal(*(next(begin(rng),4)), c_str("for"));
            check_equal(*(next(begin(rng),5)), c_str("all"));
            check_equal(*(next(begin(rng),6)), c_str("good"));
            check_equal(*(next(begin(rng),7)), c_str("men"));
            check_equal(*(next(begin(rng),8)), c_str("to"));
            check_equal(*(next(begin(rng),9)), c_str("come"));
            check_equal(*(next(begin(rng),10)), c_str("to"));
            check_equal(*(next(begin(rng),11)), c_str("the"));
            check_equal(*(next(begin(rng),12)), c_str("aid"));
            check_equal(*(next(begin(rng),13)), c_str("of"));
            check_equal(*(next(begin(rng),14)), c_str("their"));
            check_equal(*(next(begin(rng),15)), c_str("country."));
        }
    }

    {
        std::string str("Now is the time for all good men to come to the aid of their country.");
        auto rng = view::split(str, c_str(" "));
        CHECK(distance(rng) == 16);
        if(distance(rng) == 16)
        {
            check_equal(*(next(begin(rng),0)), c_str("Now"));
            check_equal(*(next(begin(rng),1)), c_str("is"));
            check_equal(*(next(begin(rng),2)), c_str("the"));
            check_equal(*(next(begin(rng),3)), c_str("time"));
            check_equal(*(next(begin(rng),4)), c_str("for"));
            check_equal(*(next(begin(rng),5)), c_str("all"));
            check_equal(*(next(begin(rng),6)), c_str("good"));
            check_equal(*(next(begin(rng),7)), c_str("men"));
            check_equal(*(next(begin(rng),8)), c_str("to"));
            check_equal(*(next(begin(rng),9)), c_str("come"));
            check_equal(*(next(begin(rng),10)), c_str("to"));
            check_equal(*(next(begin(rng),11)), c_str("the"));
            check_equal(*(next(begin(rng),12)), c_str("aid"));
            check_equal(*(next(begin(rng),13)), c_str("of"));
            check_equal(*(next(begin(rng),14)), c_str("their"));
            check_equal(*(next(begin(rng),15)), c_str("country."));
        }
    }

    {
        std::string str("Now is the time for all ggood men to come to the aid of their country.");
        auto rng = view::split(str, starts_with_g{});
        CHECK(distance(rng) == 3);
        if(distance(rng) == 3)
        {
            check_equal(*begin(rng), c_str("Now is the time for all "));
            check_equal(*next(begin(rng)), c_str("g"));
            check_equal(*(next(begin(rng),2)), c_str("good men to come to the aid of their country."));
        }
    }

    {
        std::string str("Now is the time for all ggood men to come to the aid of their country.");
        forward_iterator<std::string::iterator> i {str.begin()};
        auto rng = view::counted(i, str.size()) | view::split(starts_with_g{});
        CHECK(distance(rng) == 3);
        if(distance(rng) == 3)
        {
            check_equal(*begin(rng), c_str("Now is the time for all "));
            check_equal(*next(begin(rng)), c_str("g"));
            check_equal(*(next(begin(rng),2)), c_str("good men to come to the aid of their country."));
        }
    }

    {
        std::string str("meow");
        auto rng = view::split(str, view::empty<char>());
        CHECK(distance(rng) == 4);
        if(distance(rng) == 4)
        {
            check_equal(*(next(begin(rng),0)), c_str("m"));
            check_equal(*(next(begin(rng),1)), c_str("e"));
            check_equal(*(next(begin(rng),2)), c_str("o"));
            check_equal(*(next(begin(rng),3)), c_str("w"));
        }
    }

    {
      int a[] = {0, 2, 3, 1, 4, 5, 1, 6, 7};
      auto rng = a | view::remove_if([](int i) { return i % 2 == 0; });
      auto srng = view::split(rng, 1);
      CHECK(distance(srng) == 3);
      check_equal(*begin(srng), {3});
      check_equal(*next(begin(srng), 1), {5});
      check_equal(*next(begin(srng), 2), {7});
    }

    {
        std::string str("now  is \t the\ttime");
        auto rng = view::split(str, (int(*)(int))&std::isspace);
        CHECK(distance(rng) == 4);
        if(distance(rng) == 4)
        {
            check_equal(*(next(begin(rng),0)), c_str("now"));
            check_equal(*(next(begin(rng),1)), c_str("is"));
            check_equal(*(next(begin(rng),2)), c_str("the"));
            check_equal(*(next(begin(rng),3)), c_str("time"));
        }
    }

    {   // Regression test for https://stackoverflow.com/questions/49015671
        auto const str = "quick brown fox";
        auto rng = view::c_str(str) | view::split(' ');
        CONCEPT_ASSERT(ForwardRange<decltype(rng)>());
    }

    return test_result();
}
