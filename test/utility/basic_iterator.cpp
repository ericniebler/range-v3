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

#include <cstring>
#include <tuple>
#include <range/v3/utility/basic_iterator.hpp>
#include <range/v3/utility/common_tuple.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

#include <range/v3/utility/counted_iterator.hpp>

namespace test_weak_input
{
    template<typename I>
    struct cursor
    {
        I it_;
        struct mixin : ranges::basic_mixin<cursor>
        {
            mixin() = default;
            mixin(cursor c) : ranges::basic_mixin<cursor>(c) {}
            mixin(I i) : ranges::basic_mixin<cursor>(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        cursor(cursor<J> that) : it_(std::move(that.it_)) {}

        auto get() const -> decltype(*it_) { return *it_; }
        void next() { ++it_; }
    };

    CONCEPT_ASSERT(ranges::detail::WeakInputCursor<cursor<char*>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    static_assert(
        std::is_same<
            iterator<char*>::iterator_category,
            ranges::weak_input_iterator_tag>::value,
        "");

    void test()
    {
        using namespace ranges;
        using I = iterator<char const *>;

        static const char sz[] = "hello world";
        I i{sz};
        CHECK(*i == 'h');
        ++i;
        CHECK(*i == 'e');
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
            mixin(cursor c) : ranges::basic_mixin<cursor>(c) {}
            mixin(I i) : ranges::basic_mixin<cursor>(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        cursor(cursor<J> that) : it_(std::move(that.it_)) {}

        auto get() const -> decltype(*it_) { return *it_; }
        bool equal(cursor<I> const &that) const  { return that.it_ == it_; }
        void next() { ++it_; }
        void prev() { --it_; }
        void advance(ranges::iterator_difference_t<I> n) {
            it_ += n;
        }
        ranges::iterator_difference_t<I> distance_to(cursor<I> const &that) const {
            return that.it_ - it_;
        }
    };

    CONCEPT_ASSERT(ranges::detail::RandomAccessCursor<cursor<char*>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    static_assert(
        std::is_same<
            iterator<char*>::iterator_category,
            ranges::random_access_iterator_tag>::value,
        "");

    void test()
    {
        using namespace ranges;

        iterator<char*> a(nullptr);
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

namespace test_weak_output
{
    template<typename I>
    struct cursor
    {
    private:
        friend ranges::range_access;
        I it_;
        void set(ranges::iterator_value_t<I> v) const { *it_ = v; }
        void next() { ++it_; }
    public:
        struct mixin : ranges::basic_mixin<cursor>
        {
            mixin() = default;
            mixin(cursor c) : ranges::basic_mixin<cursor>(c) {}
            mixin(I i) : ranges::basic_mixin<cursor>(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
    };

    CONCEPT_ASSERT(ranges::detail::WeakOutputCursor<cursor<char*>, char>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    CONCEPT_ASSERT(ranges::WeakOutputIterator<iterator<char*>, char>());

    void test()
    {
        char buf[10];
        iterator<char*> i(buf);
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
            mixin(cursor c) : ranges::basic_mixin<cursor>(c) {}
            mixin(I i) : ranges::basic_mixin<cursor>(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        cursor(cursor<J> that) : it_(std::move(that.it_)) {}

        using value_type = ranges::iterator_value_t<I>;
        value_type get() const { return *it_; }
        void set(value_type v) const { *it_ = v; }
        void next() { ++it_; }
        bool equal(cursor const &that) const { return it_ == that.it_; }
    };

    CONCEPT_ASSERT(ranges::detail::OutputCursor<cursor<char*>, char>());
    CONCEPT_ASSERT(ranges::detail::ForwardCursor<cursor<char*>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    CONCEPT_ASSERT(ranges::OutputIterator<iterator<char*>, char>());
    CONCEPT_ASSERT(ranges::ForwardIterator<iterator<char*>>());

    void test()
    {
        char buf[10];
        iterator<char*> i(buf);
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
        CHECK(i == iterator<char*>{buf+5});
        ++i;
        CHECK(i != iterator<char*>{buf+5});
        CHECK(i == iterator<char*>{buf+6});
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
            mixin(zip1_cursor c) : ranges::basic_mixin<zip1_cursor>(c) {}
            mixin(I i) : ranges::basic_mixin<zip1_cursor>(zip1_cursor{i}) {}
        };
        zip1_cursor() = default;
        explicit zip1_cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        zip1_cursor(zip1_cursor<J> that) : it_(std::move(that.it_)) {}

        using value_type = std::tuple<ranges::iterator_value_t<I>>;
        using reference = ranges::common_tuple<ranges::iterator_reference_t<I>>;
        using rvalue_reference = ranges::common_tuple<ranges::iterator_rvalue_reference_t<I>>;
        reference get() const { return reference{*it_}; }
        rvalue_reference move() const { return rvalue_reference{ranges::iter_move(it_)}; }
        void set(reference const &v) const { reference{*it_} = v; }
        void set(value_type&& v) const { reference{*it_} = std::move(v); }
        void next() { ++it_; }
        bool equal(zip1_cursor const &that) const { return it_ == that.it_; }
    };

    CONCEPT_ASSERT(ranges::detail::OutputCursor<zip1_cursor<MoveOnly*>, std::tuple<MoveOnly>&&>());
    CONCEPT_ASSERT(ranges::detail::ForwardCursor<zip1_cursor<MoveOnly*>>());

    template<class I>
    using iterator = ranges::basic_iterator<zip1_cursor<I>>;

    CONCEPT_ASSERT(ranges::OutputIterator<iterator<MoveOnly*>, std::tuple<MoveOnly>&&>());
    CONCEPT_ASSERT(ranges::ForwardIterator<iterator<MoveOnly*>>());

    void test()
    {
        MoveOnly buf[10] = {};
        iterator<MoveOnly*> i(buf);
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
            mixin(cursor c) : ranges::basic_mixin<cursor>(c) {}
            mixin(I i) : ranges::basic_mixin<cursor>(cursor{i}) {}
        };
        cursor() = default;
        explicit cursor(I i) : it_(i) {}
        template<class J, CONCEPT_REQUIRES_(ranges::ConvertibleTo<J, I>())>
        cursor(cursor<J> that) : it_(std::move(that.it_)) {}

        auto get() const -> decltype(*it_) { return *it_; }
        bool equal(cursor<I> const &that) const  { return that.it_ == it_; }
        void next() { ++it_; }
        ranges::iterator_difference_t<I> distance_to(cursor<I> const &that) const {
            return that.it_ - it_;
        }
    };

    CONCEPT_ASSERT(ranges::detail::SizedCursor<cursor<char*>>());
    CONCEPT_ASSERT(ranges::detail::ForwardCursor<cursor<char*>>());

    template<class I>
    using iterator = ranges::basic_iterator<cursor<I>>;

    static_assert(
        std::is_same<
            iterator<char*>::iterator_category,
            ranges::forward_iterator_tag>::value,
        "");

    void test()
    {
        using namespace ranges;

        iterator<char*> a(nullptr);
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

    return ::test_result();
}
