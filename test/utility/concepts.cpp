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
#include <concepts/concepts.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/view/istream.hpp>
#include <range/v3/iterator/common_iterator.hpp>
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

static_assert(ranges::destructible<int>, "");
static_assert(ranges::destructible<const int>, "");
static_assert(!ranges::destructible<void>, "");
static_assert(ranges::destructible<int&>, "");
static_assert(!ranges::destructible<void()>, "");
static_assert(ranges::destructible<void(*)()>, "");
static_assert(ranges::destructible<void(&)()>, "");
static_assert(!ranges::destructible<int[]>, "");
static_assert(ranges::destructible<int[2]>, "");
static_assert(ranges::destructible<int(*)[2]>, "");
static_assert(ranges::destructible<int(&)[2]>, "");
static_assert(ranges::destructible<moveonly>, "");
static_assert(ranges::destructible<nonmovable>, "");
static_assert(!ranges::destructible<NotDestructible>, "");

static_assert(ranges::constructible_from<int>, "");
static_assert(ranges::constructible_from<int const>, "");
static_assert(!ranges::constructible_from<void>, "");
static_assert(!ranges::constructible_from<int const &>, "");
static_assert(!ranges::constructible_from<int ()>, "");
static_assert(!ranges::constructible_from<int(&)()>, "");
static_assert(!ranges::constructible_from<int[]>, "");
static_assert(ranges::constructible_from<int[5]>, "");
static_assert(!ranges::constructible_from<nondefaultconstructible>, "");
static_assert(ranges::constructible_from<int const(&)[5], int(&)[5]>, "");
static_assert(!ranges::constructible_from<int, int(&)[3]>, "");

static_assert(ranges::constructible_from<int, int>, "");
static_assert(ranges::constructible_from<int, int&>, "");
static_assert(ranges::constructible_from<int, int&&>, "");
static_assert(ranges::constructible_from<int, const int>, "");
static_assert(ranges::constructible_from<int, const int&>, "");
static_assert(ranges::constructible_from<int, const int&&>, "");

static_assert(!ranges::constructible_from<int&, int>, "");
static_assert(ranges::constructible_from<int&, int&>, "");
static_assert(!ranges::constructible_from<int&, int&&>, "");
static_assert(!ranges::constructible_from<int&, const int>, "");
static_assert(!ranges::constructible_from<int&, const int&>, "");
static_assert(!ranges::constructible_from<int&, const int&&>, "");

static_assert(ranges::constructible_from<const int&, int>, "");
static_assert(ranges::constructible_from<const int&, int&>, "");
static_assert(ranges::constructible_from<const int&, int&&>, "");
static_assert(ranges::constructible_from<const int&, const int>, "");
static_assert(ranges::constructible_from<const int&, const int&>, "");
static_assert(ranges::constructible_from<const int&, const int&&>, "");

static_assert(ranges::constructible_from<int&&, int>, "");
static_assert(!ranges::constructible_from<int&&, int&>, "");
static_assert(ranges::constructible_from<int&&, int&&>, "");
static_assert(!ranges::constructible_from<int&&, const int>, "");
static_assert(!ranges::constructible_from<int&&, const int&>, "");
static_assert(!ranges::constructible_from<int&&, const int&&>, "");

static_assert(ranges::constructible_from<const int&&, int>, "");
static_assert(!ranges::constructible_from<const int&&, int&>, "");
static_assert(ranges::constructible_from<const int&&, int&&>, "");
static_assert(ranges::constructible_from<const int&&, const int>, "");
static_assert(!ranges::constructible_from<const int&&, const int&>, "");
static_assert(ranges::constructible_from<const int&&, const int&&>, "");

struct XXX
{
    XXX() = default;
    XXX(XXX&&) = delete;
    explicit XXX(int) {}
};

static_assert(ranges::constructible_from<XXX, int>, "");

static_assert(ranges::default_constructible<int>, "");
static_assert(ranges::default_constructible<int const>, "");
static_assert(!ranges::default_constructible<int const &>, "");
static_assert(!ranges::default_constructible<int ()>, "");
static_assert(!ranges::default_constructible<int(&)()>, "");
static_assert(!ranges::default_constructible<int[]>, "");
static_assert(ranges::default_constructible<int[5]>, "");
static_assert(!ranges::default_constructible<nondefaultconstructible>, "");

static_assert(ranges::move_constructible<int>, "");
static_assert(ranges::move_constructible<const int>, "");
static_assert(ranges::move_constructible<int &>, "");
static_assert(ranges::move_constructible<int &&>, "");
static_assert(ranges::move_constructible<const int &>, "");
static_assert(ranges::move_constructible<const int &&>, "");
static_assert(ranges::destructible<moveonly>, "");
static_assert(ranges::constructible_from<moveonly, moveonly>, "");
static_assert(ranges::move_constructible<moveonly>, "");
static_assert(!ranges::move_constructible<nonmovable>, "");
static_assert(ranges::move_constructible<nonmovable &>, "");
static_assert(ranges::move_constructible<nonmovable &&>, "");
static_assert(ranges::move_constructible<const nonmovable &>, "");
static_assert(ranges::move_constructible<const nonmovable &&>, "");

static_assert(ranges::copy_constructible<int>, "");
static_assert(ranges::copy_constructible<const int>, "");
static_assert(ranges::copy_constructible<int &>, "");
static_assert(!ranges::copy_constructible<int &&>, "");
static_assert(ranges::copy_constructible<const int &>, "");
static_assert(!ranges::copy_constructible<const int &&>, "");
static_assert(!ranges::copy_constructible<moveonly>, "");
static_assert(!ranges::copy_constructible<nonmovable>, "");
static_assert(ranges::copy_constructible<nonmovable &>, "");
static_assert(!ranges::copy_constructible<nonmovable &&>, "");
static_assert(ranges::copy_constructible<const nonmovable &>, "");
static_assert(!ranges::copy_constructible<const nonmovable &&>, "");

static_assert(ranges::movable<int>, "");
static_assert(!ranges::movable<int const>, "");
static_assert(ranges::movable<moveonly>, "");
static_assert(!ranges::movable<nonmovable>, "");

static_assert(ranges::copyable<int>, "");
static_assert(!ranges::copyable<int const>, "");
static_assert(!ranges::copyable<moveonly>, "");
static_assert(!ranges::copyable<nonmovable>, "");

static_assert(ranges::input_iterator<int*>, "");
static_assert(!ranges::input_iterator<int>, "");

static_assert(ranges::forward_iterator<int*>, "");
static_assert(!ranges::forward_iterator<int>, "");

static_assert(ranges::bidirectional_iterator<int*>, "");
static_assert(!ranges::bidirectional_iterator<int>, "");

static_assert(ranges::random_access_iterator<int*>, "");
static_assert(!ranges::random_access_iterator<int>, "");

static_assert(ranges::view_<ranges::istream_view<int>>, "");
static_assert(ranges::input_iterator<ranges::iterator_t<ranges::istream_view<int>>>, "");
static_assert(!ranges::view_<int>, "");

static_assert(ranges::common_range<std::vector<int> >, "");
static_assert(ranges::common_range<std::vector<int> &>, "");
static_assert(!ranges::view_<std::vector<int>>, "");
static_assert(!ranges::view_<std::vector<int> &>, "");
static_assert(ranges::random_access_iterator<ranges::iterator_t<std::vector<int> const &>>, "");
static_assert(!ranges::common_range<ranges::istream_view<int>>, "");

static_assert(ranges::predicate<std::less<int>, int, int>, "");
static_assert(!ranges::predicate<std::less<int>, char*, int>, "");

static_assert(ranges::output_iterator<int *, int>, "");
static_assert(!ranges::output_iterator<int const *, int>, "");

static_assert(ranges::swappable<int &>, "");
static_assert(ranges::swappable<int>, "");
static_assert(!ranges::swappable<int const &>, "");
static_assert(ranges::swappable<IntSwappable>, "");
static_assert(ranges::swappable_with<IntSwappable, int &>, "");
static_assert(!ranges::swappable_with<IntSwappable, int const &>, "");

static_assert(ranges::totally_ordered<int>, "");
static_assert(ranges::common_with<int, IntComparable>, "");
static_assert(ranges::common_reference_with<int &, IntComparable &>, "");
static_assert(ranges::totally_ordered_with<int, IntComparable>, "");
static_assert(ranges::totally_ordered_with<IntComparable, int>, "");
static_assert(ranges::detail::weakly_equality_comparable_with_<int, int>, "");
static_assert(ranges::equality_comparable<int>, "");
static_assert(ranges::equality_comparable_with<int, int>, "");
static_assert(ranges::equality_comparable_with<int, IntComparable>, "");
static_assert(ranges::equality_comparable_with<int &, IntComparable &>, "");

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

static_assert(ranges::view_<ranges::istream_view<int>>, "");
static_assert(!ranges::common_range<ranges::istream_view<int>>, "");
static_assert(!ranges::sized_range<ranges::istream_view<int>>, "");

struct myview {
    const char *begin();
    const char *end();
};
CPP_assert(ranges::view_<myview>);

int main()
{
    return test_result();
}
