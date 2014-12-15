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

#include <string>
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/bounded.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <range/v3/utility/iterator.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<std::string> const vs{"hello", "goodbye", "hello", "goodbye"};

    // All ranges
    {
        std::stringstream str{"john paul george ringo"};
        using V = std::tuple<int, std::string, std::string>;
        auto && rng = view::zip(vi, vs, istream<std::string>(str) | view::bounded);
        ::models<concepts::BoundedRange>(rng);
        ::models_not<concepts::SizedRange>(rng);
        ::models<concepts::InputIterator>(begin(rng));
        ::models_not<concepts::ForwardIterator>(begin(rng));
        std::vector<V> expected(begin(rng), end(rng));
        ::check_equal(expected, {V{0, "hello", "john"},
                                 V{1, "goodbye", "paul"},
                                 V{2, "hello", "george"},
                                 V{3, "goodbye", "ringo"}});
    }

    // Mixed ranges and bounded ranges
    {
        std::stringstream str{"john paul george ringo"};
        using V = std::tuple<int, std::string, std::string>;
        auto && rng = view::zip(vi, vs, istream<std::string>(str));
        ::models<concepts::Range>(rng);
        ::models_not<concepts::SizedRange>(rng);
        ::models_not<concepts::BoundedRange>(rng);
        ::models<concepts::InputIterator>(begin(rng));
        ::models_not<concepts::ForwardIterator>(begin(rng));
        std::vector<V> expected;
        copy(rng, ranges::back_inserter(expected));
        ::check_equal(expected, {V{0, "hello", "john"},
                                 V{1, "goodbye", "paul"},
                                 V{2, "hello", "george"},
                                 V{3, "goodbye", "ringo"}});
    }

    auto rnd_rng = view::zip(vi, vs);
    using Ref = range_reference_t<decltype(rnd_rng)>;
    static_assert(std::is_same<Ref, std::pair<int &,std::string const &>>::value, "");
    ::models<concepts::BoundedRange>(rnd_rng);
    ::models<concepts::SizedRange>(rnd_rng);
    ::models<concepts::RandomAccessIterator>(begin(rnd_rng));
    auto tmp = cbegin(rnd_rng) + 3;
    CHECK(std::get<0>(*tmp) == 3);
    CHECK(std::get<1>(*tmp) == "goodbye");

    CHECK((rnd_rng.end() - rnd_rng.begin()) == 4);
    CHECK((rnd_rng.begin() - rnd_rng.end()) == -4);
    CHECK(rnd_rng.size() == 4u);

    // zip_with
    {
        std::vector<std::string> v0{"a","b","c"};
        std::vector<std::string> v1{"x","y","z"};

        auto rng = view::zip_with(std::plus<std::string>{}, v0, v1);
        std::vector<std::string> expected;
        copy(rng, ranges::back_inserter(expected));
        ::check_equal(expected, {"ax","by","cz"});
    }

    return test_result();
}
