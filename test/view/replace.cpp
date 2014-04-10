//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <string>
#include <sstream>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/istream_range.hpp>
#include <range/v3/view/replace.hpp>
#include <range/v3/view/as_range.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;
    using namespace ranges;
    using namespace std::placeholders;

    std::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str};

    auto && rng = istream<int>(sin) | view::replace(1, 42);
    has_type<int const &>(*begin(rng));
    models<concepts::Iterable>(rng);
    models_not<concepts::SizedIterable>(rng);
    models_not<concepts::Range>(rng);
    models<concepts::InputIterator>(begin(rng));
    models_not<concepts::ForwardIterator>(begin(rng));

    auto && tmp = rng | view::as_range;
    has_type<int const &>(*begin(tmp));
    models<concepts::Range>(tmp);
    models_not<concepts::SizedRange>(tmp);
    models<concepts::InputIterator>(begin(tmp));
    models_not<concepts::ForwardIterator>(begin(tmp));
    std::vector<int> actual{begin(tmp), end(tmp)};
    ::check_equal(actual, {42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9});

    return test_result();
}
