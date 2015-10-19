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

#include <range/v3/utility/functional.hpp>
#include <range/v3/view/filter.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

struct Integer
{
    int i;
    operator int() const { return i; }
    bool odd() const { return (i % 2) != 0; }
};

int main()
{
    // Check the forward_ref function
    using namespace ranges;
    int i = 0;
    int const ci = 0;
    reference_wrapper<int> ri = forward_ref<int &>(i);
    reference_wrapper<int const> ri1 = forward_ref<int const &>(ci);
    reference_wrapper<int, true> ri2 = forward_ref<int &&>(i);
    reference_wrapper<int const, true> ri3 = forward_ref<int const &&>(ci);
    int j = forward_ref<int>(i);

    static_assert(std::is_same<decltype(forward_ref<int &>(i)), reference_wrapper<int>>::value, "");
    static_assert(std::is_same<decltype(forward_ref<int const &>(ci)), reference_wrapper<int const>>::value, "");
    static_assert(std::is_same<decltype(forward_ref<int &&>(i)), reference_wrapper<int, true>>::value, "");
    static_assert(std::is_same<decltype(forward_ref<int const &&>(ci)), reference_wrapper<int const, true>>::value, "");

    static_assert(std::is_same<referent_of_t<decltype(ri)>, int>::value, "");
    static_assert(std::is_same<referent_of_t<decltype(ri1)>, int const>::value, "");
    static_assert(std::is_same<referent_of_t<decltype(ri2)>, int>::value, "");
    static_assert(std::is_same<referent_of_t<decltype(ri3)>, int const>::value, "");

    static_assert(std::is_same<reference_of_t<decltype(ri)>, int &>::value, "");
    static_assert(std::is_same<reference_of_t<decltype(ri1)>, int const &>::value, "");
    static_assert(std::is_same<reference_of_t<decltype(ri2)>, int &&>::value, "");
    static_assert(std::is_same<reference_of_t<decltype(ri3)>, int const &&>::value, "");

    int & ri_ = ri;
    int const & ri1_ = ri1;
    int && ri2_ = ri2;
    int const && ri3_ = ri3;

    CHECK(&ri_ == &i);
    CHECK(&ri1_ == &ci);
    CHECK(&ri2_ == &i);
    CHECK(&ri3_ == &ci);

    detail::ignore_unused(ri_, ri1_, ri2_, ri3_, j);

    {
      // Check that not_ works with callables
      Integer some_ints[] = {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}};
      ::check_equal(some_ints | view::filter(ranges::not_(&Integer::odd)),
                    {0,2,4,6});
    }

    return test_result();
}
