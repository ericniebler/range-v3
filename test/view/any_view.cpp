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

#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/any_view.hpp>
#include <range/v3/utility/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace
{
    template<typename S, typename T, typename = void>
    struct can_convert_to : std::false_type {};
    template<typename S, typename T>
    struct can_convert_to<S, T, meta::void_<
        decltype(ranges::polymorphic_downcast<T>(std::declval<S>()))>>
      : std::true_type {};

    void test_polymorphic_downcast()
    {
        struct A { virtual ~A() = default; };
        struct B : A {};
        struct unrelated {};
        struct incomplete;

        CONCEPT_ASSERT(can_convert_to<B *, void *>());
        CONCEPT_ASSERT(can_convert_to<A *, void *>());
        CONCEPT_ASSERT(can_convert_to<B *, A *>());
        CONCEPT_ASSERT(can_convert_to<A *, B *>());
        CONCEPT_ASSERT(!can_convert_to<int, int>());
        CONCEPT_ASSERT(!can_convert_to<A const *, A *>());
        CONCEPT_ASSERT(!can_convert_to<A *, unrelated *>());
        CONCEPT_ASSERT(!can_convert_to<unrelated *, A *>());
        CONCEPT_ASSERT(!can_convert_to<incomplete *, incomplete *>());

        CONCEPT_ASSERT(can_convert_to<B &, A &>());
        CONCEPT_ASSERT(can_convert_to<A &, B &>());
        CONCEPT_ASSERT(!can_convert_to<A &, unrelated &>());
        CONCEPT_ASSERT(!can_convert_to<unrelated &, A &>());
        CONCEPT_ASSERT(!can_convert_to<incomplete &, incomplete &>());

#if !defined(__GNUC__) || defined(__clang__) || __GNUC__ > 4
        CONCEPT_ASSERT(can_convert_to<B &&, A &&>());
        CONCEPT_ASSERT(can_convert_to<B &, A &&>());
#endif // old GCC dynamic_cast bug
        CONCEPT_ASSERT(!can_convert_to<B &&, A &>());
        CONCEPT_ASSERT(can_convert_to<A &&, B &&>());
        CONCEPT_ASSERT(!can_convert_to<A &&, B &>());
        CONCEPT_ASSERT(can_convert_to<A &, B &&>());
        CONCEPT_ASSERT(!can_convert_to<A &&, unrelated &&>());
        CONCEPT_ASSERT(!can_convert_to<A &&, unrelated &>());
        CONCEPT_ASSERT(!can_convert_to<A &, unrelated &&>());
        CONCEPT_ASSERT(!can_convert_to<unrelated &&, A &&>());
        CONCEPT_ASSERT(!can_convert_to<unrelated &&, A &>());
        CONCEPT_ASSERT(!can_convert_to<unrelated &, A &&>());
        CONCEPT_ASSERT(!can_convert_to<incomplete &&, incomplete &&>());
        CONCEPT_ASSERT(!can_convert_to<incomplete &&, incomplete &>());
        CONCEPT_ASSERT(!can_convert_to<incomplete &, incomplete &&>());
    }
} // unnamed namespace

int main()
{
    using namespace ranges;
    auto const ten_ints = {0,1,2,3,4,5,6,7,8,9};

    {
        any_view<int> ints = view::ints;
        CONCEPT_ASSERT(InputView<decltype(ints)>());
        CONCEPT_ASSERT(!ForwardView<decltype(ints)>());
        ::check_equal(std::move(ints) | view::take(10), ten_ints);
    }
    {
        any_view<int> ints2 = view::ints | view::take(10);
        ::check_equal(ints2, ten_ints);
        ::check_equal(ints2, ten_ints);
    }
    {
        any_random_access_view<int> ints3 = view::ints | view::take(10);
        ::models<concepts::RandomAccessView>(aux::copy(ints3));
        ::check_equal(ints3, ten_ints);
        ::check_equal(ints3, ten_ints);
        ::check_equal(aux::copy(ints3), ten_ints);
        ::check_equal(ints3 | view::reverse, {9,8,7,6,5,4,3,2,1,0});
    }
    {
        any_view<int&> e;
        CHECK(e.begin() == e.begin());
        CHECK(e.begin() == e.end());
    }
    {
        iterator_t<any_random_access_view<int&>> i{},j{};
        sentinel_t<any_random_access_view<int&>> k{};
        CHECK(i == j);
        CHECK(i == k);
        CHECK((i - j) == 0);
    }

    // Regression test for #446
    {
        auto vec = std::vector<short>{begin(ten_ints), end(ten_ints)};
        ::check_equal(any_view<int>{vec}, ten_ints);
        ::check_equal(any_view<int>{ranges::detail::as_const(vec)}, ten_ints);

        struct Int
        {
            int i_;

            Int(int i) : i_{i} {}
            operator int() const { return i_; }
        };
        auto vec2 = std::vector<Int>{begin(ten_ints), end(ten_ints)};
        ::check_equal(any_view<int>{vec2}, ten_ints);
    }

    {
        auto v = any_view<int>{debug_input_view<int const>{
            ten_ints.begin(), std::ptrdiff_t(ten_ints.size())
        }};
        ::check_equal(v, ten_ints);
    }

    test_polymorphic_downcast();

    return test_result();
}
