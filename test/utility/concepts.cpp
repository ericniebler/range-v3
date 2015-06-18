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

struct moveonly
{
    moveonly(moveonly&&) = default;
    moveonly& operator=(moveonly&&) = default;
};

struct nonmovable
{
    nonmovable(nonmovable const &) = delete;
    nonmovable& operator=(nonmovable const &) = delete;
};

struct nondefaultconstructible
{
    nondefaultconstructible(int) {}
};

static_assert(ranges::MoveConstructible<int>(), "");
static_assert(ranges::MoveConstructible<const int>(), "");
static_assert(ranges::MoveConstructible<moveonly>(), "");
static_assert(!ranges::MoveConstructible<nonmovable>(), "");

static_assert(ranges::CopyConstructible<int>(), "");
static_assert(ranges::CopyConstructible<const int>(), "");
static_assert(!ranges::CopyConstructible<moveonly>(), "");
static_assert(!ranges::CopyConstructible<nonmovable>(), "");

static_assert(ranges::Movable<int>(), "");
static_assert(!ranges::Movable<int const>(), "");
static_assert(ranges::Movable<moveonly>(), "");
static_assert(!ranges::Movable<nonmovable>(), "");

static_assert(ranges::Copyable<int>(), "");
static_assert(!ranges::Copyable<int const>(), "");
static_assert(!ranges::Copyable<moveonly>(), "");
static_assert(!ranges::Copyable<nonmovable>(), "");

static_assert(ranges::Constructible<int>(), "");
static_assert(!ranges::Constructible<nondefaultconstructible>(), "");

static_assert(ranges::InputIterator<int*>(), "");
static_assert(!ranges::InputIterator<int>(), "");

static_assert(ranges::ForwardIterator<int*>(), "");
static_assert(!ranges::ForwardIterator<int>(), "");

static_assert(ranges::BidirectionalIterator<int*>(), "");
static_assert(!ranges::BidirectionalIterator<int>(), "");

static_assert(ranges::RandomAccessIterator<int*>(), "");
static_assert(!ranges::RandomAccessIterator<int>(), "");

static_assert(ranges::View<ranges::istream_range<int>>(), "");
static_assert(ranges::InputIterator<ranges::range_iterator_t<ranges::istream_range<int>>>(), "");
static_assert(!ranges::View<int>(), "");

static_assert(ranges::BoundedRange<std::vector<int> >(), "");
static_assert(!ranges::BoundedView<std::vector<int>>(), "");
static_assert(!ranges::BoundedView<std::vector<int> &>(), "");
static_assert(ranges::RandomAccessIterator<ranges::range_iterator_t<std::vector<int> const &>>(), "");
static_assert(!ranges::BoundedView<ranges::istream_range<int>>(), "");

static_assert(ranges::Predicate<std::less<int>, int, int>(), "");
static_assert(!ranges::Predicate<std::less<int>, char*, int>(), "");

static_assert(ranges::OutputIterator<int *, int>(), "");
static_assert(!ranges::OutputIterator<int const *, int>(), "");

struct NotDestructible
{
    ~NotDestructible() = delete;
};

static_assert(ranges::Destructible<int>(), "");
static_assert(ranges::Destructible<const int>(), "");
static_assert(!ranges::Destructible<void>(), "");
static_assert(!ranges::Destructible<int&>(), "");
static_assert(!ranges::Destructible<void()>(), "");
static_assert(ranges::Destructible<void(*)()>(), "");
static_assert(!ranges::Destructible<void(&)()>(), "");
static_assert(!ranges::Destructible<int[2]>(), "");
static_assert(ranges::Destructible<int(*)[2]>(), "");
static_assert(!ranges::Destructible<int(&)[2]>(), "");
static_assert(ranges::Destructible<moveonly>(), "");
static_assert(ranges::Destructible<nonmovable>(), "");
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
        ranges::bounded_range_concept_t<std::vector<int>>,
        ranges::concepts::BoundedRange
    >::value, "");

static_assert(
    std::is_same<
        ranges::sized_range_concept_t<std::vector<int>>,
        ranges::concepts::SizedRange
    >::value, "");

static_assert(
    std::is_same<
        ranges::bounded_view_concept_t<ranges::istream_range<int>>,
        ranges::concepts::View
    >::value, "");

static_assert(
    std::is_same<
        ranges::sized_view_concept_t<ranges::istream_range<int>>,
        ranges::concepts::View
    >::value, "");

int main()
{
    return test_result();
}
