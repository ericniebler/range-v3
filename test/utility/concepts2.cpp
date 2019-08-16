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

#include <concepts/concepts.hpp>
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

static_assert(concepts::same_as<int, int>, "");
static_assert(concepts::same_as<void, void>, "");
static_assert(concepts::same_as<void const, void const>, "");
static_assert(!concepts::same_as<int&, int>, "");
static_assert(!concepts::same_as<void, void const>, "");
static_assert(!concepts::same_as<void(), void(*)()>, "");

static_assert(concepts::convertible_to<int, int>, "");
static_assert(concepts::convertible_to<short&, short const&>, "");
static_assert(concepts::convertible_to<int, short>, "");
static_assert(!concepts::convertible_to<int&, short&>, "");
static_assert(!concepts::convertible_to<int, void>, "");
static_assert(!concepts::convertible_to<int, int&>, "");

static_assert(concepts::unsigned_integral<unsigned>, "");
static_assert(!concepts::unsigned_integral<int>, "");

static_assert(concepts::assignable_from<int&, int>, "");
static_assert(!concepts::assignable_from<int const&, int>, "");
static_assert(!concepts::assignable_from<int, int>, "");

static_assert(concepts::destructible<int>, "");
static_assert(concepts::destructible<const int>, "");
static_assert(!concepts::destructible<void>, "");
static_assert(concepts::destructible<int&>, "");
static_assert(!concepts::destructible<void()>, "");
static_assert(concepts::destructible<void(*)()>, "");
static_assert(concepts::destructible<void(&)()>, "");
static_assert(!concepts::destructible<int[]>, "");
static_assert(concepts::destructible<int[2]>, "");
static_assert(concepts::destructible<int(*)[2]>, "");
static_assert(concepts::destructible<int(&)[2]>, "");
static_assert(concepts::destructible<moveonly>, "");
static_assert(concepts::destructible<nonmovable>, "");
static_assert(!concepts::destructible<NotDestructible>, "");

static_assert(concepts::constructible_from<int>, "");
static_assert(concepts::constructible_from<int const>, "");
static_assert(!concepts::constructible_from<void>, "");
static_assert(!concepts::constructible_from<int const &>, "");
static_assert(!concepts::constructible_from<int ()>, "");
static_assert(!concepts::constructible_from<int(&)()>, "");
static_assert(!concepts::constructible_from<int[]>, "");
static_assert(concepts::constructible_from<int[5]>, "");
static_assert(!concepts::constructible_from<nondefaultconstructible>, "");
static_assert(concepts::constructible_from<int const(&)[5], int(&)[5]>, "");
static_assert(!concepts::constructible_from<int, int(&)[3]>, "");

static_assert(concepts::constructible_from<int, int>, "");
static_assert(concepts::constructible_from<int, int&>, "");
static_assert(concepts::constructible_from<int, int&&>, "");
static_assert(concepts::constructible_from<int, const int>, "");
static_assert(concepts::constructible_from<int, const int&>, "");
static_assert(concepts::constructible_from<int, const int&&>, "");

static_assert(!concepts::constructible_from<int&, int>, "");
static_assert(concepts::constructible_from<int&, int&>, "");
static_assert(!concepts::constructible_from<int&, int&&>, "");
static_assert(!concepts::constructible_from<int&, const int>, "");
static_assert(!concepts::constructible_from<int&, const int&>, "");
static_assert(!concepts::constructible_from<int&, const int&&>, "");

static_assert(concepts::constructible_from<const int&, int>, "");
static_assert(concepts::constructible_from<const int&, int&>, "");
static_assert(concepts::constructible_from<const int&, int&&>, "");
static_assert(concepts::constructible_from<const int&, const int>, "");
static_assert(concepts::constructible_from<const int&, const int&>, "");
static_assert(concepts::constructible_from<const int&, const int&&>, "");

static_assert(concepts::constructible_from<int&&, int>, "");
static_assert(!concepts::constructible_from<int&&, int&>, "");
static_assert(concepts::constructible_from<int&&, int&&>, "");
static_assert(!concepts::constructible_from<int&&, const int>, "");
static_assert(!concepts::constructible_from<int&&, const int&>, "");
static_assert(!concepts::constructible_from<int&&, const int&&>, "");

static_assert(concepts::constructible_from<const int&&, int>, "");
static_assert(!concepts::constructible_from<const int&&, int&>, "");
static_assert(concepts::constructible_from<const int&&, int&&>, "");
static_assert(concepts::constructible_from<const int&&, const int>, "");
static_assert(!concepts::constructible_from<const int&&, const int&>, "");
static_assert(concepts::constructible_from<const int&&, const int&&>, "");

struct XXX
{
    XXX() = default;
    XXX(XXX&&) = delete;
    explicit XXX(int) {}
};

static_assert(concepts::constructible_from<XXX, int>, "");
static_assert(!concepts::move_constructible<XXX>, "");
static_assert(!concepts::movable<XXX>, "");
static_assert(!concepts::semiregular<XXX>, "");
static_assert(!concepts::regular<XXX>, "");

static_assert(concepts::default_constructible<int>, "");
static_assert(concepts::default_constructible<int const>, "");
static_assert(!concepts::default_constructible<int const &>, "");
static_assert(!concepts::default_constructible<int ()>, "");
static_assert(!concepts::default_constructible<int(&)()>, "");
static_assert(!concepts::default_constructible<int[]>, "");
static_assert(concepts::default_constructible<int[5]>, "");
static_assert(!concepts::default_constructible<nondefaultconstructible>, "");

static_assert(concepts::move_constructible<int>, "");
static_assert(concepts::move_constructible<const int>, "");
static_assert(concepts::move_constructible<int &>, "");
static_assert(concepts::move_constructible<int &&>, "");
static_assert(concepts::move_constructible<const int &>, "");
static_assert(concepts::move_constructible<const int &&>, "");
static_assert(concepts::destructible<moveonly>, "");
static_assert(concepts::constructible_from<moveonly, moveonly>, "");
static_assert(concepts::move_constructible<moveonly>, "");
static_assert(!concepts::move_constructible<nonmovable>, "");
static_assert(concepts::move_constructible<nonmovable &>, "");
static_assert(concepts::move_constructible<nonmovable &&>, "");
static_assert(concepts::move_constructible<const nonmovable &>, "");
static_assert(concepts::move_constructible<const nonmovable &&>, "");

static_assert(concepts::copy_constructible<int>, "");
static_assert(concepts::copy_constructible<const int>, "");
static_assert(concepts::copy_constructible<int &>, "");
static_assert(!concepts::copy_constructible<int &&>, "");
static_assert(concepts::copy_constructible<const int &>, "");
static_assert(!concepts::copy_constructible<const int &&>, "");
static_assert(!concepts::copy_constructible<moveonly>, "");
static_assert(!concepts::copy_constructible<nonmovable>, "");
static_assert(concepts::copy_constructible<nonmovable &>, "");
static_assert(!concepts::copy_constructible<nonmovable &&>, "");
static_assert(concepts::copy_constructible<const nonmovable &>, "");
static_assert(!concepts::copy_constructible<const nonmovable &&>, "");

static_assert(concepts::movable<int>, "");
static_assert(!concepts::movable<int const>, "");
static_assert(concepts::movable<moveonly>, "");
static_assert(!concepts::movable<nonmovable>, "");

static_assert(concepts::copyable<int>, "");
static_assert(!concepts::copyable<int const>, "");
static_assert(!concepts::copyable<moveonly>, "");
static_assert(!concepts::copyable<nonmovable>, "");

// static_assert(concepts::predicate<std::less<int>, int, int>, "");
// static_assert(!concepts::predicate<std::less<int>, char*, int>, "");

static_assert(concepts::swappable<int &>, "");
static_assert(concepts::swappable<int>, "");
static_assert(!concepts::swappable<int const &>, "");
static_assert(concepts::swappable<IntSwappable>, "");
static_assert(concepts::swappable_with<IntSwappable, int &>, "");
static_assert(!concepts::swappable_with<IntSwappable, int const &>, "");

static_assert(concepts::totally_ordered<int>, "");
static_assert(concepts::common_with<int, IntComparable>, "");
static_assert(concepts::common_reference_with<int &, IntComparable &>, "");
static_assert(concepts::totally_ordered_with<int, IntComparable>, "");
static_assert(concepts::totally_ordered_with<IntComparable, int>, "");
static_assert(concepts::detail::weakly_equality_comparable_with_<int, int>, "");
static_assert(concepts::equality_comparable<int>, "");
static_assert(concepts::equality_comparable_with<int, int>, "");
static_assert(concepts::equality_comparable_with<int, IntComparable>, "");
static_assert(concepts::equality_comparable_with<int &, IntComparable &>, "");

CPP_template(class T)
    (requires concepts::regular<T>)
constexpr bool is_regular(T&&)
{
    return true;
}

CPP_template(class T)
    (requires (!concepts::regular<T>))
constexpr bool is_regular(T&&)
{
    return false;
}

static_assert(is_regular(42), "");
static_assert(!is_regular(XXX{}), "");

int main()
{
    return test_result();
}
