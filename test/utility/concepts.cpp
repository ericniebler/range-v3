// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

//#define RANGES_USE_LEGACY_CONCEPTS 1

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

struct NotDestructible
{
    ~NotDestructible() = delete;
};

struct IntComparable
{
    operator int() const;

    friend bool operator==(IntComparable, IntComparable);
    friend bool operator!=(IntComparable, IntComparable);

    friend bool operator<(IntComparable, IntComparable);
    friend bool operator>(IntComparable, IntComparable);
    friend bool operator<=(IntComparable, IntComparable);
    friend bool operator>=(IntComparable, IntComparable);

    friend bool operator==(int, IntComparable);
    friend bool operator!=(int, IntComparable);
    friend bool operator==(IntComparable, int);
    friend bool operator!=(IntComparable, int);

    friend bool operator<(int, IntComparable);
    friend bool operator<(IntComparable, int);
    friend bool operator>(int, IntComparable);
    friend bool operator>(IntComparable, int);
    friend bool operator<=(int, IntComparable);
    friend bool operator<=(IntComparable, int);
    friend bool operator>=(int, IntComparable);
    friend bool operator>=(IntComparable, int);
};

struct IntSwappable
{
    operator int() const;

    friend void swap(int &, IntSwappable);
    friend void swap(IntSwappable, int &);
    friend void swap(IntSwappable, IntSwappable);
};

static_assert(ranges::Destructible<int>, "");
static_assert(ranges::Destructible<const int>, "");
static_assert(!ranges::Destructible<void>, "");
static_assert(ranges::Destructible<int&>, "");
static_assert(!ranges::Destructible<void()>, "");
static_assert(ranges::Destructible<void(*)()>, "");
static_assert(ranges::Destructible<void(&)()>, "");
static_assert(!ranges::Destructible<int[]>, "");
static_assert(ranges::Destructible<int[2]>, "");
static_assert(ranges::Destructible<int(*)[2]>, "");
static_assert(ranges::Destructible<int(&)[2]>, "");
static_assert(ranges::Destructible<moveonly>, "");
static_assert(ranges::Destructible<nonmovable>, "");
static_assert(!ranges::Destructible<NotDestructible>, "");

static_assert(ranges::Constructible<int>, "");
static_assert(ranges::Constructible<int const>, "");
static_assert(!ranges::Constructible<void>, "");
static_assert(!ranges::Constructible<int const &>, "");
static_assert(!ranges::Constructible<int ()>, "");
static_assert(!ranges::Constructible<int(&)()>, "");
static_assert(!ranges::Constructible<int[]>, "");
static_assert(ranges::Constructible<int[5]>, "");
static_assert(!ranges::Constructible<nondefaultconstructible>, "");
static_assert(ranges::Constructible<int const(&)[5], int(&)[5]>, "");
static_assert(!ranges::Constructible<int, int(&)[3]>, "");

static_assert(ranges::Constructible<int, int>, "");
static_assert(ranges::Constructible<int, int&>, "");
static_assert(ranges::Constructible<int, int&&>, "");
static_assert(ranges::Constructible<int, const int>, "");
static_assert(ranges::Constructible<int, const int&>, "");
static_assert(ranges::Constructible<int, const int&&>, "");

static_assert(!ranges::Constructible<int&, int>, "");
static_assert(ranges::Constructible<int&, int&>, "");
static_assert(!ranges::Constructible<int&, int&&>, "");
static_assert(!ranges::Constructible<int&, const int>, "");
static_assert(!ranges::Constructible<int&, const int&>, "");
static_assert(!ranges::Constructible<int&, const int&&>, "");

static_assert(ranges::Constructible<const int&, int>, "");
static_assert(ranges::Constructible<const int&, int&>, "");
static_assert(ranges::Constructible<const int&, int&&>, "");
static_assert(ranges::Constructible<const int&, const int>, "");
static_assert(ranges::Constructible<const int&, const int&>, "");
static_assert(ranges::Constructible<const int&, const int&&>, "");

static_assert(ranges::Constructible<int&&, int>, "");
static_assert(!ranges::Constructible<int&&, int&>, "");
static_assert(ranges::Constructible<int&&, int&&>, "");
static_assert(!ranges::Constructible<int&&, const int>, "");
static_assert(!ranges::Constructible<int&&, const int&>, "");
static_assert(!ranges::Constructible<int&&, const int&&>, "");

static_assert(ranges::Constructible<const int&&, int>, "");
static_assert(!ranges::Constructible<const int&&, int&>, "");
static_assert(ranges::Constructible<const int&&, int&&>, "");
static_assert(ranges::Constructible<const int&&, const int>, "");
static_assert(!ranges::Constructible<const int&&, const int&>, "");
static_assert(ranges::Constructible<const int&&, const int&&>, "");

struct XXX
{
    XXX() = default;
    XXX(XXX&&) = delete;
    explicit XXX(int) {}
};

static_assert(ranges::Constructible<XXX, int>, "");

static_assert(ranges::DefaultConstructible<int>, "");
static_assert(ranges::DefaultConstructible<int const>, "");
static_assert(!ranges::DefaultConstructible<int const &>, "");
static_assert(!ranges::DefaultConstructible<int ()>, "");
static_assert(!ranges::DefaultConstructible<int(&)()>, "");
static_assert(!ranges::DefaultConstructible<int[]>, "");
static_assert(ranges::DefaultConstructible<int[5]>, "");
static_assert(!ranges::DefaultConstructible<nondefaultconstructible>, "");

static_assert(ranges::MoveConstructible<int>, "");
static_assert(ranges::MoveConstructible<const int>, "");
static_assert(ranges::MoveConstructible<int &>, "");
static_assert(ranges::MoveConstructible<int &&>, "");
static_assert(ranges::MoveConstructible<const int &>, "");
static_assert(ranges::MoveConstructible<const int &&>, "");
static_assert(ranges::Destructible<moveonly>, "");
static_assert(ranges::Constructible<moveonly, moveonly>, "");
static_assert(ranges::MoveConstructible<moveonly>, "");
static_assert(!ranges::MoveConstructible<nonmovable>, "");
static_assert(ranges::MoveConstructible<nonmovable &>, "");
static_assert(ranges::MoveConstructible<nonmovable &&>, "");
static_assert(ranges::MoveConstructible<const nonmovable &>, "");
static_assert(ranges::MoveConstructible<const nonmovable &&>, "");

static_assert(ranges::CopyConstructible<int>, "");
static_assert(ranges::CopyConstructible<const int>, "");
static_assert(ranges::CopyConstructible<int &>, "");
static_assert(!ranges::CopyConstructible<int &&>, "");
static_assert(ranges::CopyConstructible<const int &>, "");
static_assert(!ranges::CopyConstructible<const int &&>, "");
static_assert(!ranges::CopyConstructible<moveonly>, "");
static_assert(!ranges::CopyConstructible<nonmovable>, "");
static_assert(ranges::CopyConstructible<nonmovable &>, "");
static_assert(!ranges::CopyConstructible<nonmovable &&>, "");
static_assert(ranges::CopyConstructible<const nonmovable &>, "");
static_assert(!ranges::CopyConstructible<const nonmovable &&>, "");

static_assert(ranges::Movable<int>, "");
static_assert(!ranges::Movable<int const>, "");
static_assert(ranges::Movable<moveonly>, "");
static_assert(!ranges::Movable<nonmovable>, "");

static_assert(ranges::Copyable<int>, "");
static_assert(!ranges::Copyable<int const>, "");
static_assert(!ranges::Copyable<moveonly>, "");
static_assert(!ranges::Copyable<nonmovable>, "");

static_assert(ranges::InputIterator<int*>, "");
static_assert(!ranges::InputIterator<int>, "");

static_assert(ranges::ForwardIterator<int*>, "");
static_assert(!ranges::ForwardIterator<int>, "");

static_assert(ranges::BidirectionalIterator<int*>, "");
static_assert(!ranges::BidirectionalIterator<int>, "");

static_assert(ranges::RandomAccessIterator<int*>, "");
static_assert(!ranges::RandomAccessIterator<int>, "");

static_assert(ranges::View<ranges::istream_range<int>>, "");
static_assert(ranges::InputIterator<ranges::iterator_t<ranges::istream_range<int>>>, "");
static_assert(!ranges::View<int>, "");

static_assert(ranges::CommonRange<std::vector<int> >, "");
static_assert(!ranges::CommonView<std::vector<int>>, "");
static_assert(!ranges::CommonView<std::vector<int> &>, "");
static_assert(ranges::RandomAccessIterator<ranges::iterator_t<std::vector<int> const &>>, "");
static_assert(!ranges::CommonView<ranges::istream_range<int>>, "");

static_assert(ranges::Predicate<std::less<int>, int, int>, "");
static_assert(!ranges::Predicate<std::less<int>, char*, int>, "");

static_assert(ranges::OutputIterator<int *, int>, "");
static_assert(!ranges::OutputIterator<int const *, int>, "");

static_assert(ranges::Swappable<int &>, "");
static_assert(ranges::Swappable<int>, "");
static_assert(!ranges::Swappable<int const &>, "");
static_assert(ranges::Swappable<IntSwappable>, "");
static_assert(ranges::SwappableWith<IntSwappable, int &>, "");
static_assert(!ranges::SwappableWith<IntSwappable, int const &>, "");

static_assert(ranges::StrictTotallyOrdered<int>, "");
static_assert(ranges::Common<int, IntComparable>, "");
static_assert(ranges::CommonReference<int &, IntComparable &>, "");
static_assert(ranges::StrictTotallyOrderedWith<int, IntComparable>, "");
static_assert(ranges::StrictTotallyOrderedWith<IntComparable, int>, "");
static_assert(ranges::WeaklyEqualityComparableWith<int, int>, "");
static_assert(ranges::EqualityComparable<int>, "");
static_assert(ranges::EqualityComparableWith<int, int>, "");
static_assert(ranges::EqualityComparableWith<int, IntComparable>, "");
static_assert(ranges::EqualityComparableWith<int &, IntComparable &>, "");

static_assert(
    std::is_same<
        ranges::common_range_tag_of<std::vector<int>>,
        ranges::common_range_tag
    >::value, "");

static_assert(
    std::is_same<
        ranges::sized_range_tag_of<std::vector<int>>,
        ranges::sized_range_tag
    >::value, "");

static_assert(
    std::is_same<
        ranges::common_view_tag_of<ranges::istream_range<int>>,
        ranges::view_tag
    >::value, "");

static_assert(
    std::is_same<
        ranges::sized_view_tag_of<ranges::istream_range<int>>,
        ranges::view_tag
    >::value, "");

struct myview {
    const char *begin();
    const char *end();
};
CPP_assert(ranges::View<myview>);

int main()
{
    return test_result();
}
