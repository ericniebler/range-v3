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

#include <cstring>
#include <tuple>
#include <range/v3/utility/basic_iterator.hpp>
#include <range/v3/utility/common_tuple.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace test_weak_input
{
    template<typename I>
    struct cursor
    {
        I it_;
        struct mixin : ranges::basic_mixin<cursor>
        {
            mixin() = default;
            using ranges::basic_mixin<cursor>::basic_mixin;
            mixin(I i) : mixin(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        cursor(cursor<J> that) : it_(std::move(that.it_)) {}

        auto read() const -> decltype(*it_) { return *it_; }
        void next() { ++it_; }
    };

    CONCEPT_ASSERT(ranges::detail::InputCursor<cursor<char *>>());
    CONCEPT_ASSERT(!ranges::detail::CursorSentinel<cursor<char *>, cursor<char *>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    static_assert(
        std::is_same<
            iterator<char *>::iterator_category,
            ranges::input_iterator_tag>::value,
        "");
    static_assert(
        !ranges::EqualityComparable<iterator<char *>>(),
        "");

    void test()
    {
        using namespace ranges;
        using I = iterator<char const *>;
        CONCEPT_ASSERT(std::is_same<std::iterator_traits<I>::pointer, char const *>());

        static char const sz[] = "hello world";
        I i{sz};
        CHECK(*i == 'h');
        CHECK(&*i == i.operator->());
        ++i;
        CHECK(*i == 'e');
        CHECK(&*i == i.operator->());
    }
}

namespace test_random_access
{
    template<typename I>
    struct cursor
    {
        I it_;
        struct mixin : ranges::basic_mixin<cursor>
        {
            mixin() = default;
            using ranges::basic_mixin<cursor>::basic_mixin;
            mixin(I i) : mixin(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        cursor(cursor<J> that) : it_(std::move(that.it_)) {}

        auto read() const -> decltype(*it_) { return *it_; }
        bool equal(cursor<I> const &that) const { return that.it_ == it_; }
        void next() { ++it_; }
        void prev() { --it_; }
        void advance(ranges::difference_type_t<I> n) {
            it_ += n;
        }
        ranges::difference_type_t<I> distance_to(cursor<I> const &that) const {
            return that.it_ - it_;
        }
    };

    CONCEPT_ASSERT(ranges::detail::RandomAccessCursor<cursor<char *>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    static_assert(
        std::is_same<
            iterator<char *>::iterator_category,
            ranges::random_access_iterator_tag>::value,
        "");

    void test()
    {
        using namespace ranges;

        iterator<char *> a(nullptr);
        iterator<char const *> b(nullptr);
        iterator<char const *> c(a);

        CONCEPT_ASSERT(std::is_same<std::iterator_traits<iterator<char *>>::pointer, char *>());

        b = a;
        bool d = a == b;
        d = (a != b);

        detail::ignore_unused(
            d,
            a < b,
            a <= b,
            a > b,
            a >= b,
            (a-b),
            (b-a),
            (a-a),
            (b-b));
    }
}

namespace test_weak_output
{
    template<typename I>
    struct cursor
    {
        struct mixin : ranges::basic_mixin<cursor>
        {
            mixin() = default;
            using ranges::basic_mixin<cursor>::basic_mixin;
            explicit mixin(I i) : mixin(cursor{i}) {}
        };

        cursor() = default;
        explicit cursor(I i) : it_(i) {}

        void write(ranges::value_type_t<I> v) const { *it_ = v; }
        void next() { ++it_; }
    private:
        I it_;
    };

    CONCEPT_ASSERT(ranges::detail::OutputCursor<cursor<char *>, char>());
    CONCEPT_ASSERT(!ranges::detail::CursorSentinel<cursor<char *>, cursor<char *>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    CONCEPT_ASSERT(ranges::OutputIterator<iterator<char *>, char>());
    CONCEPT_ASSERT(!ranges::EqualityComparable<iterator<char *>>());

    void test()
    {
        char buf[10];
        iterator<char *> i(buf);
        *i = 'h';
        ++i;
        *i = 'e';
        ++i;
        *i = 'l';
        ++i;
        *i = 'l';
        ++i;
        *i = 'o';
        ++i;
        *i = '\0';
        CHECK(0 == std::strcmp(buf, "hello"));
    }
}

namespace test_output
{
    template<typename I>
    struct cursor
    {
        I it_;
        struct mixin : ranges::basic_mixin<cursor>
        {
            mixin() = default;
            using ranges::basic_mixin<cursor>::basic_mixin;
            mixin(I i) : mixin(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        cursor(cursor<J> that) : it_(std::move(that.it_)) {}

        using value_type = ranges::value_type_t<I>;
        value_type read() const { return *it_; }
        void write(value_type v) const { *it_ = v; }
        I arrow() const { return it_; }
        void next() { ++it_; }
        bool equal(cursor const &that) const { return it_ == that.it_; }
    };

    CONCEPT_ASSERT(ranges::detail::OutputCursor<cursor<char *>, char>());
    CONCEPT_ASSERT(ranges::detail::ForwardCursor<cursor<char *>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    CONCEPT_ASSERT(ranges::OutputIterator<iterator<char *>, char>());
    CONCEPT_ASSERT(ranges::ForwardIterator<iterator<char *>>());

    CONCEPT_ASSERT(std::is_same<std::iterator_traits<iterator<char *>>::pointer, char *>());

    void test()
    {
        char buf[10];
        iterator<char *> i(buf);
        *i = 'h';
        CHECK(*i == 'h');
        CHECK(*i == *i);
        ++i;
        *i = 'e';
        CHECK('e' == *i);
        ++i;
        *i = 'l';
        ++i;
        *i = 'l';
        ++i;
        *i = 'o';
        ++i;
        *i = '\0';
        CHECK(0 == std::strcmp(buf, "hello"));
        CHECK(i == iterator<char *>{buf+5});
        ++i;
        CHECK(i != iterator<char *>{buf+5});
        CHECK(i == iterator<char *>{buf+6});
    }
}

namespace test_move_only
{
    struct MoveOnly
    {
        MoveOnly() = default;
        MoveOnly(MoveOnly &&) = default;
        MoveOnly(MoveOnly const &) = delete;
        MoveOnly &operator=(MoveOnly &&) = default;
        MoveOnly &operator=(MoveOnly const &) = delete;
    };

    template<typename I>
    struct zip1_cursor
    {
        I it_;
        struct mixin : ranges::basic_mixin<zip1_cursor>
        {
            mixin() = default;
            using ranges::basic_mixin<zip1_cursor>::basic_mixin;
            mixin(I i) : mixin(zip1_cursor{i}) {}
        };
        zip1_cursor() = default;
        explicit zip1_cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        zip1_cursor(zip1_cursor<J> that) : it_(std::move(that.it_)) {}

        using value_type = std::tuple<ranges::value_type_t<I>>;
        using reference = ranges::common_tuple<ranges::reference_t<I>>;
        using rvalue_reference = ranges::common_tuple<ranges::rvalue_reference_t<I>>;
        reference read() const { return reference{*it_}; }
        rvalue_reference move() const { return rvalue_reference{ranges::iter_move(it_)}; }
        void write(reference const &v) const { reference{*it_} = v; }
        void write(value_type && v) const { reference{*it_} = std::move(v); }
        void next() { ++it_; }
        bool equal(zip1_cursor const &that) const { return it_ == that.it_; }
    };

    CONCEPT_ASSERT(ranges::detail::OutputCursor<zip1_cursor<MoveOnly *>, std::tuple<MoveOnly> &&>());
    CONCEPT_ASSERT(ranges::detail::ForwardCursor<zip1_cursor<MoveOnly *>>());

    template<class I>
    using iterator = ranges::basic_iterator<zip1_cursor<I>>;

    CONCEPT_ASSERT(ranges::OutputIterator<iterator<MoveOnly *>, std::tuple<MoveOnly> &&>());
    CONCEPT_ASSERT(ranges::ForwardIterator<iterator<MoveOnly *>>());

    void test()
    {
        MoveOnly buf[10] = {};
        iterator<MoveOnly *> i(buf);
        *i = std::tuple<MoveOnly>{};
        ranges::common_tuple<MoveOnly&> x = *i; (void)x;
        std::tuple<MoveOnly> v = ranges::iter_move(i);
        *i = std::move(v);
    }
}

namespace test_forward_sized
{
    template<typename I>
    struct cursor
    {
        I it_;
        struct mixin : ranges::basic_mixin<cursor>
        {
            mixin() = default;
            using ranges::basic_mixin<cursor>::basic_mixin;
            mixin(I i) : mixin(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        cursor(cursor<J> that) : it_(std::move(that.it_)) {}

        auto read() const -> decltype(*it_) { return *it_; }
        bool equal(cursor<I> const &that) const { return that.it_ == it_; }
        void next() { ++it_; }
        ranges::difference_type_t<I> distance_to(cursor<I> const &that) const {
            return that.it_ - it_;
        }
    };

    CONCEPT_ASSERT(ranges::detail::SizedCursorSentinel<cursor<char *>, cursor<char *>>());
    CONCEPT_ASSERT(ranges::detail::ForwardCursor<cursor<char *>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    static_assert(
        std::is_same<
            iterator<char *>::iterator_category,
            ranges::forward_iterator_tag>::value,
        "");

    void test()
    {
        using namespace ranges;

        iterator<char *> a(nullptr);
        iterator<char const *> b(nullptr);
        iterator<char const *> c(a);

        b = a;
        bool d = a == b;
        d = (a != b);

        detail::ignore_unused(
            d,
            a < b,
            a <= b,
            a > b,
            a >= b,
            (a-b),
            (b-a),
            (a-a),
            (b-b));
    }
}

RANGES_DIAGNOSTIC_IGNORE_UNNEEDED_MEMBER

void test_box()
{
    struct A : ranges::box<int> {};
    CHECK(sizeof(A) == sizeof(int));
    struct empty {};
    struct B : ranges::box<empty> { int i; };
    CHECK(sizeof(B) == sizeof(int));
    B b1, b2;
    if (ranges::detail::box_compression<empty>() == ranges::detail::box_compress::coalesce)
        CHECK((&b1.get() == &b2.get()));
    struct nontrivial { nontrivial() {} };
    struct C : ranges::box<nontrivial> { int i; };
    CHECK(sizeof(C) == sizeof(int));
    C c1, c2;
    CHECK((&c1.get() != &c2.get()));

    {
        // empty but not trivial cursor that defines value_type:
        struct cursor {
            using value_type = int;
            cursor() {}
            int read() const { return 42; }
            void next() {}
        };
        CONCEPT_ASSERT(ranges::detail::box_compression<cursor>() ==
            ranges::detail::box_compress::ebo);
        CONCEPT_ASSERT(ranges::Same<int, ranges::basic_iterator<cursor>::value_type>());
    }
}

int main()
{
    using namespace ranges;
    std::cout << "\nTesting basic_iterator\n";

    ::test_weak_input::test();
    ::test_random_access::test();
    ::test_weak_output::test();
    ::test_output::test();
    ::test_move_only::test();
    ::test_forward_sized::test();
    ::test_box();

    return ::test_result();
}
