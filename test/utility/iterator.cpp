// Range v3 library
//
//  Copyright Eric Niebler 2014, 2016
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#include <list>
#include <ostream>
#include <string>
#include <vector>
#include <meta/meta.hpp>
#include <range/v3/utility/iterator.hpp>
#include <range/v3/algorithm/copy.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

struct MoveOnlyReadable
{
    using value_type = std::unique_ptr<int>;
    value_type operator*() const;
};

CONCEPT_ASSERT(Readable<MoveOnlyReadable>());

void test_insert_iterator()
{
    CONCEPT_ASSERT(OutputIterator<insert_iterator<std::vector<int>>, int&&>());
    CONCEPT_ASSERT(!EqualityComparable<insert_iterator<std::vector<int>>>());
    std::vector<int> vi{5,6,7,8};
    copy({1,2,3,4}, inserter(vi, vi.begin()+2));
    ::check_equal(vi, {5,6,1,2,3,4,7,8});
}

void test_move_iterator()
{
    std::vector<MoveOnlyString> in;
    in.emplace_back("this");
    in.emplace_back("is");
    in.emplace_back("his");
    in.emplace_back("face");
    std::vector<MoveOnlyString> out;
    auto first = ranges::make_move_iterator(in.begin());
    using I = decltype(first);
    CONCEPT_ASSERT(InputIterator<I>());
    CONCEPT_ASSERT(!ForwardIterator<I>());
    CONCEPT_ASSERT(Same<I, ranges::move_iterator<std::vector<MoveOnlyString>::iterator>>());
    auto last = ranges::make_move_sentinel(in.end());
    using S = decltype(last);
    CONCEPT_ASSERT(Sentinel<S, I>());
    CONCEPT_ASSERT(SizedSentinel<I, I>());
    CHECK((first - first) == 0);
    CONCEPT_ASSERT(SizedSentinel<S, I>());
    CHECK(static_cast<std::size_t>(last - first) == in.size());
    ranges::copy(first, last, ranges::back_inserter(out));
    ::check_equal(in, {"","","",""});
    ::check_equal(out, {"this","is","his","face"});
}

template<class I>
using RI = std::reverse_iterator<I>;

void issue_420_regression()
{
    // Verify that SizedSentinel<std::reverse_iterator<S>, std::reverse_iterator<I>>
    // properly requires SizedSentinel<I, S>
    CONCEPT_ASSERT(SizedSentinel<RI<int*>, RI<int*>>());
    CONCEPT_ASSERT(!SizedSentinel<RI<int*>, RI<float*>>());
    using BI = bidirectional_iterator<int*>;
    CONCEPT_ASSERT(!SizedSentinel<RI<BI>, RI<BI>>());
}

struct value_type_tester_thingy {};

namespace ranges {
    template<>
    struct value_type<::value_type_tester_thingy> {
        using type = int;
    };
}

template<typename T>
struct with_value_type { using value_type = T; };
template<typename T>
struct with_element_type { using element_type = T; };

// arrays of known bound
CONCEPT_ASSERT(Same<int, ranges::value_type<int[4]>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<const int[4]>::type>());
CONCEPT_ASSERT(Same<int*, ranges::value_type<int*[4]>::type>());
CONCEPT_ASSERT(Same<with_value_type<int>, ranges::value_type<with_value_type<int>[4]>::type>());

#if !defined(__GNUC__) || defined(__clang__)
// arrays of unknown bound
CONCEPT_ASSERT(Same<int, ranges::value_type<int[]>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<const int[]>::type>());
#endif

// object pointer types
CONCEPT_ASSERT(Same<int, ranges::value_type<int*>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<int*const>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<int const*>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<int const*const>::type>());
CONCEPT_ASSERT(Same<int[4], ranges::value_type<int(*)[4]>::type>());
CONCEPT_ASSERT(Same<int[4], ranges::value_type<const int(*)[4]>::type>());
struct incomplete;
CONCEPT_ASSERT(Same<incomplete, ranges::value_type<incomplete*>::type>());
static_assert(!meta::is_trait<ranges::value_type<void*>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<void const*>>::value, "");

// class types with member value_type
CONCEPT_ASSERT(Same<int, ranges::value_type<with_value_type<int>>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<with_value_type<int> const>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<value_type_tester_thingy>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<value_type_tester_thingy const>::type>());
CONCEPT_ASSERT(Same<int[4], ranges::value_type<with_value_type<int[4]>>::type>());
CONCEPT_ASSERT(Same<int[4], ranges::value_type<with_value_type<int[4]> const>::type>());
static_assert(!meta::is_trait<ranges::value_type<with_value_type<void>>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<with_value_type<int(int)>>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<with_value_type<int&>>>::value, "");

// class types with member element_type
CONCEPT_ASSERT(Same<int, ranges::value_type<with_element_type<int>>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<with_element_type<int> const>::type>());
CONCEPT_ASSERT(Same<int, ranges::value_type<with_element_type<int const>>::type>());
CONCEPT_ASSERT(Same<int[4], ranges::value_type<with_element_type<int[4]>>::type>());
CONCEPT_ASSERT(Same<int[4], ranges::value_type<with_element_type<int[4]> const>::type>());
CONCEPT_ASSERT(Same<int[4], ranges::value_type<with_element_type<int const[4]>>::type>());
static_assert(!meta::is_trait<ranges::value_type<with_element_type<void>>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<with_element_type<void const>>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<with_element_type<void> const>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<with_element_type<int(int)>>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<with_element_type<int&>>>::value, "");

// classes derived from std::ios_base
CONCEPT_ASSERT(Same<char, ranges::value_type<std::ostream>::type>());

// cv-void
static_assert(!meta::is_trait<ranges::value_type<void>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<void const>>::value, "");
// reference types
static_assert(!meta::is_trait<ranges::value_type<int&>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<int&&>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<int*&>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<int*&&>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<int(&)(int)>>::value, "");
static_assert(!meta::is_trait<ranges::value_type<std::ostream&>>::value, "");

CONCEPT_ASSERT(IndirectlySwappable<int *, int *>());
CONCEPT_ASSERT(IndirectlyMovable<int const *, int *>());
CONCEPT_ASSERT(!IndirectlySwappable<int const *, int const *>());
CONCEPT_ASSERT(!IndirectlyMovable<int const *, int const *>());

namespace Boost
{
    struct S {}; // just to have a type from Boost namespace
    template<typename I, typename D>
    void advance(I&, D)
    {}
}

// Regression test for https://github.com/ericniebler/range-v3/issues/845
void test_845()
{
    std::list<std::pair<Boost::S, int>> v = { {Boost::S{}, 0} };
    auto itr = v.begin();
    ranges::advance(itr, 1); // Should not create ambiguity
}

int main()
{
    test_insert_iterator();
    test_move_iterator();
    issue_420_regression();

    {
        struct S { using value_type = int; };
        CONCEPT_ASSERT(Same<int, ranges::value_type<S const>::type>());
    }

    return ::test_result();
}
