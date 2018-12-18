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

static_assert(concepts::Same<int, int>, "");
static_assert(concepts::Same<void, void>, "");
static_assert(concepts::Same<void const, void const>, "");
static_assert(!concepts::Same<int&, int>, "");
static_assert(!concepts::Same<void, void const>, "");
static_assert(!concepts::Same<void(), void(*)()>, "");

static_assert(concepts::ConvertibleTo<int, int>, "");
static_assert(concepts::ConvertibleTo<short&, short const&>, "");
static_assert(concepts::ConvertibleTo<int, short>, "");
static_assert(!concepts::ConvertibleTo<int&, short&>, "");
static_assert(!concepts::ConvertibleTo<int, void>, "");
static_assert(!concepts::ConvertibleTo<int, int&>, "");

static_assert(concepts::UnsignedIntegral<unsigned>, "");
static_assert(!concepts::UnsignedIntegral<int>, "");

static_assert(concepts::Assignable<int&, int>, "");
static_assert(!concepts::Assignable<int const&, int>, "");
static_assert(!concepts::Assignable<int, int>, "");

static_assert(concepts::Destructible<int>, "");
static_assert(concepts::Destructible<const int>, "");
static_assert(!concepts::Destructible<void>, "");
static_assert(concepts::Destructible<int&>, "");
static_assert(!concepts::Destructible<void()>, "");
static_assert(concepts::Destructible<void(*)()>, "");
static_assert(concepts::Destructible<void(&)()>, "");
static_assert(!concepts::Destructible<int[]>, "");
static_assert(concepts::Destructible<int[2]>, "");
static_assert(concepts::Destructible<int(*)[2]>, "");
static_assert(concepts::Destructible<int(&)[2]>, "");
static_assert(concepts::Destructible<moveonly>, "");
static_assert(concepts::Destructible<nonmovable>, "");
static_assert(!concepts::Destructible<NotDestructible>, "");

static_assert(concepts::Constructible<int>, "");
static_assert(concepts::Constructible<int const>, "");
static_assert(!concepts::Constructible<void>, "");
static_assert(!concepts::Constructible<int const &>, "");
static_assert(!concepts::Constructible<int ()>, "");
static_assert(!concepts::Constructible<int(&)()>, "");
static_assert(!concepts::Constructible<int[]>, "");
static_assert(concepts::Constructible<int[5]>, "");
static_assert(!concepts::Constructible<nondefaultconstructible>, "");
static_assert(concepts::Constructible<int const(&)[5], int(&)[5]>, "");
static_assert(!concepts::Constructible<int, int(&)[3]>, "");

static_assert(concepts::Constructible<int, int>, "");
static_assert(concepts::Constructible<int, int&>, "");
static_assert(concepts::Constructible<int, int&&>, "");
static_assert(concepts::Constructible<int, const int>, "");
static_assert(concepts::Constructible<int, const int&>, "");
static_assert(concepts::Constructible<int, const int&&>, "");

static_assert(!concepts::Constructible<int&, int>, "");
static_assert(concepts::Constructible<int&, int&>, "");
static_assert(!concepts::Constructible<int&, int&&>, "");
static_assert(!concepts::Constructible<int&, const int>, "");
static_assert(!concepts::Constructible<int&, const int&>, "");
static_assert(!concepts::Constructible<int&, const int&&>, "");

static_assert(concepts::Constructible<const int&, int>, "");
static_assert(concepts::Constructible<const int&, int&>, "");
static_assert(concepts::Constructible<const int&, int&&>, "");
static_assert(concepts::Constructible<const int&, const int>, "");
static_assert(concepts::Constructible<const int&, const int&>, "");
static_assert(concepts::Constructible<const int&, const int&&>, "");

static_assert(concepts::Constructible<int&&, int>, "");
static_assert(!concepts::Constructible<int&&, int&>, "");
static_assert(concepts::Constructible<int&&, int&&>, "");
static_assert(!concepts::Constructible<int&&, const int>, "");
static_assert(!concepts::Constructible<int&&, const int&>, "");
static_assert(!concepts::Constructible<int&&, const int&&>, "");

static_assert(concepts::Constructible<const int&&, int>, "");
static_assert(!concepts::Constructible<const int&&, int&>, "");
static_assert(concepts::Constructible<const int&&, int&&>, "");
static_assert(concepts::Constructible<const int&&, const int>, "");
static_assert(!concepts::Constructible<const int&&, const int&>, "");
static_assert(concepts::Constructible<const int&&, const int&&>, "");

struct XXX
{
    XXX() = default;
    XXX(XXX&&) = delete;
    explicit XXX(int) {}
};

static_assert(concepts::Constructible<XXX, int>, "");
static_assert(!concepts::MoveConstructible<XXX>, "");
static_assert(!concepts::Movable<XXX>, "");
static_assert(!concepts::Semiregular<XXX>, "");
static_assert(!concepts::Regular<XXX>, "");

static_assert(concepts::DefaultConstructible<int>, "");
static_assert(concepts::DefaultConstructible<int const>, "");
static_assert(!concepts::DefaultConstructible<int const &>, "");
static_assert(!concepts::DefaultConstructible<int ()>, "");
static_assert(!concepts::DefaultConstructible<int(&)()>, "");
static_assert(!concepts::DefaultConstructible<int[]>, "");
static_assert(concepts::DefaultConstructible<int[5]>, "");
static_assert(!concepts::DefaultConstructible<nondefaultconstructible>, "");

static_assert(concepts::MoveConstructible<int>, "");
static_assert(concepts::MoveConstructible<const int>, "");
static_assert(concepts::MoveConstructible<int &>, "");
static_assert(concepts::MoveConstructible<int &&>, "");
static_assert(concepts::MoveConstructible<const int &>, "");
static_assert(concepts::MoveConstructible<const int &&>, "");
static_assert(concepts::Destructible<moveonly>, "");
static_assert(concepts::Constructible<moveonly, moveonly>, "");
static_assert(concepts::MoveConstructible<moveonly>, "");
static_assert(!concepts::MoveConstructible<nonmovable>, "");
static_assert(concepts::MoveConstructible<nonmovable &>, "");
static_assert(concepts::MoveConstructible<nonmovable &&>, "");
static_assert(concepts::MoveConstructible<const nonmovable &>, "");
static_assert(concepts::MoveConstructible<const nonmovable &&>, "");

static_assert(concepts::CopyConstructible<int>, "");
static_assert(concepts::CopyConstructible<const int>, "");
static_assert(concepts::CopyConstructible<int &>, "");
static_assert(!concepts::CopyConstructible<int &&>, "");
static_assert(concepts::CopyConstructible<const int &>, "");
static_assert(!concepts::CopyConstructible<const int &&>, "");
static_assert(!concepts::CopyConstructible<moveonly>, "");
static_assert(!concepts::CopyConstructible<nonmovable>, "");
static_assert(concepts::CopyConstructible<nonmovable &>, "");
static_assert(!concepts::CopyConstructible<nonmovable &&>, "");
static_assert(concepts::CopyConstructible<const nonmovable &>, "");
static_assert(!concepts::CopyConstructible<const nonmovable &&>, "");

static_assert(concepts::Movable<int>, "");
static_assert(!concepts::Movable<int const>, "");
static_assert(concepts::Movable<moveonly>, "");
static_assert(!concepts::Movable<nonmovable>, "");

static_assert(concepts::Copyable<int>, "");
static_assert(!concepts::Copyable<int const>, "");
static_assert(!concepts::Copyable<moveonly>, "");
static_assert(!concepts::Copyable<nonmovable>, "");

// static_assert(concepts::Predicate<std::less<int>, int, int>, "");
// static_assert(!concepts::Predicate<std::less<int>, char*, int>, "");

static_assert(concepts::Swappable<int &>, "");
static_assert(concepts::Swappable<int>, "");
static_assert(!concepts::Swappable<int const &>, "");
static_assert(concepts::Swappable<IntSwappable>, "");
static_assert(concepts::SwappableWith<IntSwappable, int &>, "");
static_assert(!concepts::SwappableWith<IntSwappable, int const &>, "");

static_assert(concepts::StrictTotallyOrdered<int>, "");
static_assert(concepts::Common<int, IntComparable>, "");
static_assert(concepts::CommonReference<int &, IntComparable &>, "");
static_assert(concepts::StrictTotallyOrderedWith<int, IntComparable>, "");
static_assert(concepts::StrictTotallyOrderedWith<IntComparable, int>, "");
static_assert(concepts::WeaklyEqualityComparableWith<int, int>, "");
static_assert(concepts::EqualityComparable<int>, "");
static_assert(concepts::EqualityComparableWith<int, int>, "");
static_assert(concepts::EqualityComparableWith<int, IntComparable>, "");
static_assert(concepts::EqualityComparableWith<int &, IntComparable &>, "");

CPP_template(class T)
    (requires concepts::Regular<T>)
constexpr bool is_regular(T&&)
{
    return true;
}

CPP_template(class T)
    (requires not concepts::Regular<T>)
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
