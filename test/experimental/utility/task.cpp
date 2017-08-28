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
#include <range/v3/experimental/utility/sync_wait.hpp>
#include "../../simple_test.hpp"
#include "../../test_utils.hpp"

using namespace std::experimental;
using ranges::experimental::task;
using ranges::experimental::sync_wait;

// Concept checks:
CONCEPT_ASSERT(!ranges::AsyncView<task<>>());
CONCEPT_ASSERT(ranges::AsyncView<task<int>>());
CONCEPT_ASSERT(ranges::AsyncView<task<int &>>());

CONCEPT_ASSERT(
    ranges::Same<
        ranges::value_type_t<ranges::co_await_resume_t<ranges::iterator_t<task<int &>>>>,
        int>());

task<int> fun1()
{
    co_return 42;
}

task<int> fun2()
{
    int i = co_await fun1();
    co_return i + 1;
}

task<> test_task_as_range()
{
    auto e = fun2();
    auto e2 = std::move(e);
    e = std::move(e2);
    int count = 0;
    for co_await (int i : e)
    {
        ++count;
        CHECK(i == 43);
    }
    CHECK(count == 1);
}

void test_int()
{
    auto i = fun2();
    try
    {
        int j = sync_wait(std::move(i));
        CHECK(j == 43);
    }
    catch(...)
    {
        CHECK(false);
    }
}

int count = 0;
task<> fun3()
{
    ++count;
    co_return;
}
task<> fun4()
{
    ++count;
    (void)co_await fun3();
}

void test_void()
{
    auto v = fun4();
    try
    {
        sync_wait(std::move(v));
        CHECK(count == 2);
    }
    catch(...)
    {
        CHECK(false);
    }
}

task<> fun5()
{
    throw 42;
}
task<> fun6()
{
    (void)co_await fun5();
}

void test_exception()
{
    auto e = fun6();
    try
    {
        sync_wait(std::move(e));
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

namespace T
{
    struct S
    {
        struct promise_type
        {
            suspend_never initial_suspend() { return {}; }
            suspend_never final_suspend() { return {}; }
            void unhandled_exception() {}
        };
    };

    suspend_never operator co_await(S &) { return {}; }
}

struct minig
{
    struct promise_type
    {
        int val;
        minig get_return_object() { return { *this }; }
        suspend_always initial_suspend() noexcept { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        void return_void() noexcept {}
        void unhandled_exception() noexcept {}
        suspend_always await_transform(int v) noexcept {
            val = v;
            return {};
        }
    };
    using HDL = coroutine_handle<promise_type>;
    HDL coro;
    minig(promise_type &p) : coro(HDL::from_promise(p)) {}
    minig(minig &&that) : coro(ranges::exchange(that.coro, nullptr)) {}
    ~minig() { coro.destroy(); }
    bool move_next()
    {
        coro.resume();
        return !coro.done();
    }
    int current_value() { return coro.promise().val; }
};

minig f(int n)
{
    for (int i = 0; i < n; ++i)
        co_await i; // Yes, really!
}

CONCEPT_ASSERT(!ranges::CoAwaitable<int>());
CONCEPT_ASSERT(ranges::CoAwaitable<int, minig>());

int main()
{
    test_int();
    test_void();
    test_exception();

    sync_wait(test_task_as_range());

    // This exercises the CoAwaitable concept and sync_wait:
    T::S s;
    sync_wait(s);

    return ::test_result();
}

#else // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
int main()
{}
#endif
