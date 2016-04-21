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
//

#include <range/v3/core.hpp>
#include <range/v3/view/ints.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/indirect.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

int main()
{
    using namespace ranges;

    ::check_equal(view::ints | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(0) | view::take(10), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(0,9), {0,1,2,3,4,5,6,7,8});
    ::check_equal(view::closed_ints(0,9), {0,1,2,3,4,5,6,7,8,9});
    ::check_equal(view::ints(1,10), {1,2,3,4,5,6,7,8,9});
    ::check_equal(view::closed_ints(1,10), {1,2,3,4,5,6,7,8,9,10});

    auto chars = view::ints(std::numeric_limits<char>::min(),
                            std::numeric_limits<char>::max());
    static_assert(Same<int, range_difference_t<decltype(chars)>>(), "");
    ::models<concepts::RandomAccessView>(chars);
    models<concepts::BoundedView>(chars);

    auto shorts = view::ints(std::numeric_limits<unsigned short>::min(),
                             std::numeric_limits<unsigned short>::max());
    models<concepts::BoundedView>(shorts);
    static_assert(Same<int, range_difference_t<decltype(shorts)>>(), "");

    auto uints = view::closed_ints(
        std::numeric_limits<std::uint32_t>::min(),
        std::numeric_limits<std::uint32_t>::max());
    models<concepts::BoundedView>(uints);
    static_assert(Same<std::int64_t, range_difference_t<decltype(uints)>>(), "");
    static_assert(Same<std::uint64_t, range_size_t<decltype(uints)>>(), "");
    CHECK(uints.size() == (static_cast<uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1));

    auto ints = view::closed_ints(
        std::numeric_limits<std::int32_t>::min(),
        std::numeric_limits<std::int32_t>::max());
    static_assert(Same<std::int64_t, range_difference_t<decltype(ints)>>(), "");
    static_assert(Same<std::uint64_t, range_size_t<decltype(ints)>>(), "");
    CHECK(ints.size() == (static_cast<uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1));

    return ::test_result();
}
