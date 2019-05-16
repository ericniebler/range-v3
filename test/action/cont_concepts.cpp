// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <vector>
#include <memory>
#include <range/v3/core.hpp>
#include <range/v3/action/concepts.hpp>
#include <range/v3/view/ref.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    int rgi[6];
    ::models<RangeConcept>(rgi);
    ::models_not<SemiContainerConcept>(rgi);

    std::array<int, 6> a;
    ::models<SemiContainerConcept>(a);
    ::models_not<ContainerConcept>(a);

    std::vector<int> v;
    ::models<ContainerConcept>(v);

    std::vector<std::unique_ptr<int>> v2;
    ::models<ContainerConcept>(v2);

    ::models<LvalueContainerLikeConcept>(v2);
    ::models_not<LvalueContainerLikeConcept>(std::move(v2));

    ::models<LvalueContainerLikeConcept>(view::ref(v2));

    return ::test_result();
}
