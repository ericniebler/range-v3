/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#include <vector>
#include <sstream>

#include <range/v3/view/addressof.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/core.hpp>
#include <range/v3/view_facade.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

using namespace ranges;

void simple_test()
{
    std::vector<int> list = {1,2,3};

    auto out = list | view::addressof;

    check_equal(out, {&list[0], &list[1], &list[2]});
}

struct test_istream_range
    : view_facade<test_istream_range, unknown>
{
private:
    friend range_access;

    std::vector<int>* list;

    struct cursor
    {
    private:
        std::size_t i = 0;
        std::vector<int>* list = nullptr;
    public:
        cursor() = default;
        explicit cursor(std::vector<int>& list_) : list(&list_){}
        void next()
        {
            ++i;
        }
        int &read() const noexcept
        {
            return (*list)[i];
        }
        bool equal(default_sentinel) const
        {
            return i == list->size();
        }
    };

    cursor begin_cursor()
    {
        return cursor{*list};
    }
public:
    test_istream_range() = default;
    explicit test_istream_range(std::vector<int>& list_) : list(&list_){}
};

void test_input_range()
{
    // It is implementation dependent,
    // for how long returned reference remains valid.
    // It should be valid at least until next read.
    // For test purposes we use custom input range.

    std::vector<int> list{1, 2, 3};
    auto rng = test_istream_range(list);
    ::models<concepts::InputRange>(rng);

    auto out = rng | view::addressof;
    check_equal(out, {&list[0], &list[1], &list[2]});
}

void test_pipe_prvalue()
{
    auto rng = view::iota(0) | view::take_exactly(3);
    CONCEPT_ASSERT(!view::addressof_fn::Constraint<decltype(rng)>());
}

struct test_xvalue_range
    : view_facade<test_xvalue_range, unknown>
{
private:
    friend range_access;

    struct cursor
    {
    private:
        std::size_t i = 0;
        mutable std::vector<int> list = {1,2,3};
    public:
        cursor() = default;
        void next()
        {
            ++i;
        }
        int &&read() const noexcept
        {
            return std::move(list[i]);
        }
        bool equal(default_sentinel) const
        {
            return i == list.size();
        }
    };

    cursor begin_cursor()
    {
        return cursor{};
    }
};

void test_pipe_xvalue_reference()
{
    CONCEPT_ASSERT(!view::addressof_fn::Constraint<test_xvalue_range>());
}

int main()
{
    simple_test();
    test_input_range();
    test_pipe_prvalue();
    test_pipe_xvalue_reference();
    return test_result();
}
