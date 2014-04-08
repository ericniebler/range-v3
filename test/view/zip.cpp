//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <string>
#include <vector>
#include <sstream>
#include <range/v3/core.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/as_range.hpp>
#include <range/v3/algorithm/copy.hpp>
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
        auto && rng = view::zip(vi, vs, istream<std::string>(str) | view::as_range);
        ::models<concepts::Range>(rng);
        ::models_not<concepts::SizedRange>(rng);
        ::models<concepts::InputIterator>(begin(rng));
        ::models_not<concepts::ForwardIterator>(begin(rng));
        std::vector<V> expected(begin(rng), end(rng));
        CHECK(::range_equal(expected, {V{0, "hello", "john"},
                                       V{1, "goodbye", "paul"},
                                       V{2, "hello", "george"},
                                       V{3, "goodbye", "ringo"}}));
    }

    // Mixed ranges and iterables
    {
        std::stringstream str{"john paul george ringo"};
        using V = std::tuple<int, std::string, std::string>;
        auto && rng = view::zip(vi, vs, istream<std::string>(str));
        ::models<concepts::Iterable>(rng);
        ::models_not<concepts::SizedIterable>(rng);
        ::models_not<concepts::Range>(rng);
        ::models<concepts::InputIterator>(begin(rng));
        ::models_not<concepts::ForwardIterator>(begin(rng));
        std::vector<V> expected;
        copy(rng, ranges::back_inserter(expected));
        CHECK(::range_equal(expected, {V{0, "hello", "john"},
                                       V{1, "goodbye", "paul"},
                                       V{2, "hello", "george"},
                                       V{3, "goodbye", "ringo"}}));
    }

    auto rnd_rng = view::zip(vi, vs);
    ::models<concepts::SizedRange>(rnd_rng);
    ::models<concepts::RandomAccessIterator>(begin(rnd_rng));
    auto tmp = begin(rnd_rng) + 3;
    CHECK(std::get<0>(*tmp) == 3);
    CHECK(std::get<1>(*tmp) == "goodbye");

    CHECK((rnd_rng.end() - rnd_rng.begin()) == 4);
    CHECK((rnd_rng.begin() - rnd_rng.end()) == -4);
    CHECK(rnd_rng.size() == 4);

    return test_result();
}
