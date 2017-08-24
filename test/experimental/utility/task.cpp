// Range v3 library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#include <range/v3/detail/config.hpp>
#if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#include <range/v3/experimental/utility/task.hpp>
#include <range/v3/experimental/utility/sync_await.hpp>
#include "../../simple_test.hpp"
#include "../../test_utils.hpp"

ranges::experimental::task<int> fun1()
{
    co_return 42;
}

ranges::experimental::task<int> fun2()
{
    int i = co_await fun1();
    co_return i + 1;
}

void test_int()
{
    auto i = fun2();
    try
    {
        int j = ranges::experimental::sync_await(std::move(i));
        CHECK(j == 43);
    }
    catch(...)
    {
        CHECK(false);
    }
}

int count = 0;
ranges::experimental::task<> fun3()
{
    ++count;
    co_return;
}
ranges::experimental::task<> fun4()
{
    ++count;
    (void)co_await fun3();
}

void test_void()
{
    auto v = fun4();
    try
    {
        ranges::experimental::sync_await(std::move(v));
        CHECK(count == 2);
    }
    catch(...)
    {
        CHECK(false);
    }
}

ranges::experimental::task<> fun5()
{
    throw 42;
}
ranges::experimental::task<> fun6()
{
    (void)co_await fun5();
}

void test_exception()
{
    auto e = fun6();
    try
    {
        ranges::experimental::sync_await(std::move(e));
        CHECK(false);
    }
    catch(int i)
    {
        CHECK(i == 42);
    }
    catch(...)
    {
        CHECK(false);
    }
}

int main()
{
    test_int();
    test_void();
    test_exception();

    return ::test_result();
}


#else // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#error here
int main()
{}
#endif
