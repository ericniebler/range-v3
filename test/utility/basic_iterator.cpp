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

#include <list>
#include <range/v3/core.hpp>
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

        auto current() const -> decltype(*it_) { return *it_; }
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

        auto current() const -> decltype(*it_) { return *it_; }
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

int main()
{
    using namespace ranges;
    std::cout << "\nTesting basic_iterator\n";

    ::test_weak_input::test();
    ::test_random_access::test();

    return ::test_result();
}
