//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/

#include <sstream>
#include <vector>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/istream_range.hpp>
#include <range/v3/utility/common_range_iterator.hpp>
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

static_assert(ranges::Iterable<ranges::istream_iterable<int>>(), "");
static_assert(ranges::InputIterator<ranges::range_iterator_t<ranges::istream_iterable<int>>>(), "");
static_assert(!ranges::Iterable<int>(), "");

static_assert(ranges::Range<std::vector<int> const &>(), "");
static_assert(ranges::RandomAccessIterator<ranges::range_iterator_t<std::vector<int> const &>>(), "");
static_assert(!ranges::Range<ranges::istream_iterable<int>>(), "");

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
}

static_assert(ranges::WeaklyOrdered<int>(), "");
static_assert(ranges::Common<int, IntComparable>(), "");
static_assert(ranges::WeaklyOrdered<int, IntComparable>(), "");
static_assert(ranges::WeaklyOrdered<IntComparable, int>(), "");

static_assert(
    std::is_same<
        ranges::range_concept_t<std::vector<int>>,
        ranges::concepts::Range
    >::value, "");

static_assert(
    std::is_same<
        ranges::sized_iterable_concept_t<std::vector<int>>,
        ranges::concepts::SizedIterable
    >::value, "");

static_assert(
    std::is_same<
        ranges::range_concept_t<ranges::istream_iterable<int>>,
        ranges::concepts::Iterable
    >::value, "");

static_assert(
    std::is_same<
        ranges::sized_iterable_concept_t<ranges::istream_iterable<int>>,
        ranges::concepts::Iterable
    >::value, "");

int main()
{
    return test_result();
}
