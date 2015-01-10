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

#include <sstream>
#include <vector>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/istream_range.hpp>
#include <range/v3/utility/common_iterator.hpp>
#include "../simple_test.hpp"

struct noncopyable
{
    noncopyable(noncopyable const &) = delete;
};

struct nondefaultconstructible
{
    nondefaultconstructible(int) {};
};

static_assert(ranges::CopyAssignable<int>(), "");
static_assert(!ranges::CopyAssignable<int const>(), "");

static_assert(ranges::CopyConstructible<int>(), "");
static_assert(!ranges::CopyConstructible<noncopyable>(), "");

static_assert(ranges::DefaultConstructible<int>(), "");
static_assert(!ranges::DefaultConstructible<nondefaultconstructible>(), "");

static_assert(ranges::InputIterator<int*>(), "");
static_assert(!ranges::InputIterator<int>(), "");

static_assert(ranges::ForwardIterator<int*>(), "");
static_assert(!ranges::ForwardIterator<int>(), "");

static_assert(ranges::BidirectionalIterator<int*>(), "");
static_assert(!ranges::BidirectionalIterator<int>(), "");

static_assert(ranges::RandomAccessIterator<int*>(), "");
static_assert(!ranges::RandomAccessIterator<int>(), "");

static_assert(ranges::Range<ranges::istream_range<int>>(), "");
static_assert(ranges::InputIterator<ranges::range_iterator_t<ranges::istream_range<int>>>(), "");
static_assert(!ranges::Range<int>(), "");

static_assert(ranges::BoundedIterable<std::vector<int> >(), "");
static_assert(!ranges::BoundedRange<std::vector<int>>(), "");
static_assert(!ranges::BoundedRange<std::vector<int> &>(), "");
static_assert(ranges::RandomAccessIterator<ranges::range_iterator_t<std::vector<int> const &>>(), "");
static_assert(!ranges::BoundedRange<ranges::istream_range<int>>(), "");

static_assert(ranges::Predicate<std::less<int>, int, int>(), "");
static_assert(!ranges::Predicate<std::less<int>, char*, int>(), "");

static_assert(ranges::OutputIterator<int *, int>(), "");
static_assert(!ranges::OutputIterator<int const *, int>(), "");

struct NotDestructible
{
    ~NotDestructible() = delete;
};

static_assert(ranges::Destructible<int>(), "");
static_assert(!ranges::Destructible<NotDestructible>(), "");

struct IntComparable
{
    explicit operator int() const;

    friend bool operator<(IntComparable, IntComparable);
    friend bool operator>(IntComparable, IntComparable);
    friend bool operator<=(IntComparable, IntComparable);
    friend bool operator>=(IntComparable, IntComparable);

    friend bool operator<(int, IntComparable);
    friend bool operator<(IntComparable, int);
    friend bool operator>(int, IntComparable);
    friend bool operator>(IntComparable, int);
    friend bool operator<=(int, IntComparable);
    friend bool operator<=(IntComparable, int);
    friend bool operator>=(int, IntComparable);
    friend bool operator>=(IntComparable, int);
};

namespace ranges
{
    template<> struct common_type<int, IntComparable> { using type = int; };
    template<> struct common_type<IntComparable, int> { using type = int; };
    template<typename TQual, typename UQual>
    struct common_reference_base<int, IntComparable, TQual, UQual>
        { using type = int; };
    template<typename TQual, typename UQual>
    struct common_reference_base<IntComparable, int, TQual, UQual>
        { using type = int; };
}

static_assert(ranges::WeaklyOrdered<int>(), "");
static_assert(ranges::Common<int, IntComparable>(), "");
static_assert(ranges::WeaklyOrdered<int, IntComparable>(), "");
static_assert(ranges::WeaklyOrdered<IntComparable, int>(), "");

static_assert(
    std::is_same<
        ranges::bounded_iterable_concept_t<std::vector<int>>,
        ranges::concepts::BoundedIterable
    >::value, "");

static_assert(
    std::is_same<
        ranges::sized_iterable_concept_t<std::vector<int>>,
        ranges::concepts::SizedIterable
    >::value, "");

static_assert(
    std::is_same<
        ranges::bounded_range_concept_t<ranges::istream_range<int>>,
        ranges::concepts::Range
    >::value, "");

static_assert(
    std::is_same<
        ranges::sized_range_concept_t<ranges::istream_range<int>>,
        ranges::concepts::Range
    >::value, "");

int main()
{
    return test_result();
}
