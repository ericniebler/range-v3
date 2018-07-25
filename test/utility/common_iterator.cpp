// Range v3 library
//
//  Copyright Eric Niebler 2015-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#include <algorithm>
#include <numeric>
#include <range/v3/utility/common_iterator.hpp>
#include <range/v3/utility/unreachable.hpp>
#include "../simple_test.hpp"
#include "../test_iterators.hpp"

RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER

namespace {
    struct silly_arrow_cursor {
        int read() const { return 0; }
        void next() {}
        int arrow() const { return 42; }
    };

    int forty_two = 42;
    struct lvalue_iterator {
        using iterator_category = ranges::input_iterator_tag;
        using difference_type = int;
        using value_type = int;
        int& operator*() const { return forty_two; }
        lvalue_iterator& operator++() & { return *this; }
        lvalue_iterator operator++(int) & { return *this; }
    };
    struct xvalue_iterator : lvalue_iterator {
        int&& operator*() const { return std::move(forty_two); }
        xvalue_iterator& operator++() & { return *this; }
        xvalue_iterator operator++(int) & { return *this; }
    };

    struct proxy_cursor {
        int read() const { return 42; }
        void next() {}
    };

    void test_operator_arrow() {
        // I is a pointer type
        {
            int i = 42;
            auto ci = ranges::common_iterator<int*, ranges::unreachable>{&i};
            CONCEPT_ASSERT(ranges::Same<int*, decltype(ci.operator->())>());
            CHECK(ci.operator->() == &i);
        }
        // the expression i.operator->() is well-formed
        {
            using I = ranges::basic_iterator<silly_arrow_cursor>;
            auto ci = ranges::common_iterator<I, ranges::unreachable>{};
            CONCEPT_ASSERT(ranges::Same<I, decltype(ci.operator->())>());
            CHECK(ci.operator->().operator->() == 42);
        }
        // the expression *i is a glvalue [lvalue case]
        {
            auto ci = ranges::common_iterator<lvalue_iterator, ranges::unreachable>{};
            CONCEPT_ASSERT(ranges::Same<int*, decltype(ci.operator->())>());
            CHECK(ci.operator->() == &forty_two);
        }
        // the expression *i is a glvalue [xvalue case]
        {
            auto ci = ranges::common_iterator<xvalue_iterator, ranges::unreachable>{};
            CONCEPT_ASSERT(ranges::Same<int*, decltype(ci.operator->())>());
            CHECK(ci.operator->() == &forty_two);
        }
        // Otherwise, returns a proxy object
        {
            using I = ranges::basic_iterator<proxy_cursor>;
            auto ci = ranges::common_iterator<I, ranges::unreachable>{};
            using A = decltype(ci.operator->());
            CONCEPT_ASSERT(std::is_class<A>::value);
            CONCEPT_ASSERT(!std::is_same<I, A>::value);
            CHECK(*ci.operator->().operator->() == 42);
        }
    }
}

int main() {
    {
        CONCEPT_ASSERT(
            ranges::ForwardIterator<
                ranges::common_iterator<
                    bidirectional_iterator<const char *>,
                    sentinel<const char *>>>());
        CONCEPT_ASSERT(
            !ranges::BidirectionalIterator<
                ranges::common_iterator<
                    bidirectional_iterator<const char *>,
                    sentinel<const char *>>>());
        CONCEPT_ASSERT(
            std::is_same<
                ranges::common_reference<
                    ranges::common_iterator<
                        bidirectional_iterator<const char *>,
                        sentinel<const char *>
                    >&,
                    ranges::common_iterator<
                        bidirectional_iterator<const char *>,
                        sentinel<const char *>
                    >
                >::type,
                ranges::common_iterator<
                    bidirectional_iterator<const char *>,
                    sentinel<const char *>
                >
            >::value);
        // Sized iterator range tests
        CONCEPT_ASSERT(
            !ranges::SizedSentinel<
                ranges::common_iterator<
                    forward_iterator<int*>,
                    sentinel<int*, true> >,
                ranges::common_iterator<
                    forward_iterator<int*>,
                    sentinel<int*, true> > >());
        CONCEPT_ASSERT(
            ranges::SizedSentinel<
                ranges::common_iterator<
                    random_access_iterator<int*>,
                    sentinel<int*, true> >,
                ranges::common_iterator<
                    random_access_iterator<int*>,
                    sentinel<int*, true> > >());
        CONCEPT_ASSERT(
            !ranges::SizedSentinel<
                ranges::common_iterator<
                    random_access_iterator<int*>,
                    sentinel<int*, false> >,
                ranges::common_iterator<
                    random_access_iterator<int*>,
                    sentinel<int*, false> > >());
    }
    {
        int rgi[] {0,1,2,3,4,5,6,7,8,9};
        using CI = ranges::common_iterator<
            random_access_iterator<int*>,
            sentinel<int*>>;
        CI first{random_access_iterator<int*>{rgi}};
        CI last{sentinel<int*>{rgi+10}};
        CHECK(std::accumulate(first, last, 0, std::plus<int>{}) == 45);
    }

    test_operator_arrow();

    return test_result();
}
