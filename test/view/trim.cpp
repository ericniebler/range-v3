// Range v3 library
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <list>

#include <range/v3/range/concepts.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/view/counted.hpp>
#include <range/v3/view/trim.hpp>

#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    auto is_space = [](char c) {
        return c == ' ' || c == '\t' || c == '\0';
    };
    auto _42 = view::trim(" \t42\t  \t", is_space);

    check_equal(_42, {'4', '2'});
    // Always models the most refined range concept
    // modeled by the underlying range.
    models<ContiguousViewConcept>(aux::copy(_42));
    std::list<char> l;
    models<BidirectionalViewConcept>(view::trim(l, is_space));
    models_not<RandomAccessViewConcept>(view::trim(l, is_space));
    // Always models `CommonRange`.
    models<CommonRangeConcept>(aux::copy(_42));
    models<CommonRangeConcept>(view::trim(view::counted("", 0), is_space));
    // Never models `SizedRange`.
    models_not<SizedRangeConcept>(aux::copy(_42));

    return test_result();
}
