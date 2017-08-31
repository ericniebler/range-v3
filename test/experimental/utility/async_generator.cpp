// Range v3 library
//
// Copyright 2017 Lewis Baker
// Copyright 2017 Eric Niebler
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Dual-licensed under the Boost Software License:
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#include <range/v3/detail/config.hpp>
#if RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
#include <range/v3/experimental/utility/async_generator.hpp>
#include <range/v3/experimental/utility/task.hpp>
#include <range/v3/experimental/utility/sync_wait.hpp>
#include <range/v3/experimental/utility/sync_wait_all.hpp>
#include "../../simple_test.hpp"
#include "../../test_utils.hpp"
#include "../../detail/single_consumer_event.hpp"

#include <range/v3/view/filter.hpp>


using ranges::experimental::sync_wait;
using ranges::experimental::task;
using ranges::experimental::async_generator;
using ranges::experimental::sync_wait_all;
using ranges::experimental::detail::single_consumer_event;

CONCEPT_ASSERT(ranges::AsyncIterator<async_generator<int>::iterator>());
CONCEPT_ASSERT(ranges::AsyncView<async_generator<int>>());

// default-constructed async_generator is an empty sequence
void default_ctor_async_generator_is_empty()
{
    sync_wait([]() -> task<>
    {
        // Iterating over default-constructed async_generator just
        // gives an empty sequence.
        async_generator<int> g;
        auto it = co_await g.begin();
        CHECK(it == g.end());
    }());
}

// async_generator doesn't start if begin() not called
void async_generator_does_not_start_if_begin_not_called()
{
    bool startedExecution = false;
    {
        auto gen = [&]() -> async_generator<int>
        {
            startedExecution = true;
            co_yield 1;
        }();
        CHECK(!startedExecution);
    }
    CHECK(!startedExecution);
}

// enumerate sequence of 1 value
void enumerate_sequence_of_one_value()
{
    sync_wait([]() -> task<>
    {
        bool startedExecution = false;
        auto makeGenerator = [&]() -> async_generator<std::uint32_t>
        {
            startedExecution = true;
            co_yield 1;
        };

        auto gen = makeGenerator();

        CHECK(!startedExecution);

        auto it = co_await gen.begin();

        CHECK(startedExecution);
        CHECK(it != gen.end());
        CHECK(*it == 1u);
        CHECK(co_await ++it == gen.end());
    }());
}

// enumerate sequence of multiple values
void enumerate_sequence_of_multiple_values()
{
    sync_wait([]() -> task<>
    {
        bool startedExecution = false;
        auto makeGenerator = [&]() -> async_generator<std::uint32_t>
        {
            startedExecution = true;
            co_yield 1;
            co_yield 2;
            co_yield 3;
        };

        auto gen = makeGenerator();

        CHECK(!startedExecution);

        auto it = co_await gen.begin();

        CHECK(startedExecution);

        CHECK(it != gen.end());
        CHECK(*it == 1u);

        CHECK(co_await ++it != gen.end());
        CHECK(*it == 2u);

        CHECK(co_await ++it != gen.end());
        CHECK(*it == 3u);

        CHECK(co_await ++it == gen.end());
    }());
}

namespace
{
    struct set_to_true_on_destruction
    {
        set_to_true_on_destruction(bool* value)
            : m_value(value)
        {}

        set_to_true_on_destruction(set_to_true_on_destruction&& other)
            : m_value(other.m_value)
        {
            other.m_value = nullptr;
        }

        ~set_to_true_on_destruction()
        {
            if (m_value != nullptr)
            {
                *m_value = true;
            }
        }

        set_to_true_on_destruction(const set_to_true_on_destruction&) = delete;
        set_to_true_on_destruction& operator=(const set_to_true_on_destruction&) = delete;

    private:
        bool* m_value;
    };
}

// destructors of values in scope are called when async_generator destructed early
void destructors_of_values_in_scope_are_called_on_early_destruction()
{
    sync_wait([]() -> task<>
    {
        bool aDestructed = false;
        bool bDestructed = false;

        auto makeGenerator = [&](set_to_true_on_destruction a) -> async_generator<std::uint32_t>
        {
            set_to_true_on_destruction b(&bDestructed);
            co_yield 1;
            co_yield 2;
        };

        {
            auto gen = makeGenerator(&aDestructed);

            CHECK(!aDestructed);
            CHECK(!bDestructed);

            auto it = co_await gen.begin();
            CHECK(!aDestructed);
            CHECK(!bDestructed);
            CHECK(*it == 1u);
        }

        CHECK(aDestructed);
        CHECK(bDestructed);
    }());
}

// This test tries to cover the different state-transition code-paths
// - consumer resuming producer and producer completing asynchronously
// - producer resuming consumer and consumer requesting next value synchronously
// - producer resuming consumer and consumer requesting next value asynchronously
void async_producer_with_async_consumer()
{
    single_consumer_event p1;
    single_consumer_event p2;
    single_consumer_event p3;
    single_consumer_event c1;

    auto produce = [&]() -> async_generator<std::uint32_t>
    {
        co_await p1;
        co_yield 1;
        co_await p2;
        co_yield 2;
        co_await p3;
    };

    bool consumerFinished = false;

    auto consume = [&]() -> task<>
    {
        auto generator = produce();
        auto it = co_await generator.begin();
        CHECK(*it == 1u);
        (void)co_await ++it;
        CHECK(*it == 2u);
        co_await c1;
        (void)co_await ++it;
        CHECK(it == generator.end());
        consumerFinished = true;
    };

    auto unblock = [&]() -> task<>
    {
        p1.set();
        p2.set();
        c1.set();
        CHECK(!consumerFinished);
        p3.set();
        CHECK(consumerFinished);
        co_return;
    };

    sync_wait_all(consume(), unblock());
}

// exception thrown before first yield is rethrown from begin operation
void exception_before_yield_is_rethrow()
{
    class TestException {};
    auto gen = [](bool shouldThrow) -> async_generator<std::uint32_t>
    {
        if (shouldThrow)
        {
            throw TestException();
        }
        co_yield 1;
    }(true);

    auto beginAwaitable = gen.begin();
    CHECK(beginAwaitable.await_ready());
    CHECK_THROW(beginAwaitable.await_resume(), const TestException&);
}

// exception thrown after first yield is rethrown from increment operator
void exception_after_yield_rethrown_by_increment()
{
    class TestException {};
    auto gen = [](bool shouldThrow) -> async_generator<std::uint32_t>
    {
        co_yield 1;
        if (shouldThrow)
        {
            throw TestException();
        }
    }(true);

    auto beginAwaitable = gen.begin();
    CHECK(beginAwaitable.await_ready());
    auto it = beginAwaitable.await_resume();
    CHECK(*it == 1u);
    auto incrementAwaitable = ++it;
    CHECK(incrementAwaitable.await_ready());
    CHECK_THROW(incrementAwaitable.await_resume(), const TestException&);
    CHECK(it == gen.end());
}

// large number of synchronous completions doesn't result in stack-overflow
void no_stack_overflow_for_many_sync_completions()
{
    auto makeSequence = [](single_consumer_event& event) -> async_generator<std::uint32_t>
    {
        for (std::uint32_t i = 0; i < 1000000u; ++i)
        {
            if (i == 500000u) co_await event;
            co_yield i;
        }
    };

    auto consumer = [](async_generator<std::uint32_t> sequence) -> task<>
    {
        std::uint32_t expected = 0;
        for co_await(std::uint32_t i : sequence)
        {
            CHECK(i == expected++);
        }
        CHECK(expected == 1000000u);
    };

    auto unblocker = [](single_consumer_event& event) -> task<>
    {
        // Should have processed the first 500'000 elements synchronously with consumer driving
        // iteraction before producer suspends and thus consumer suspends.
        // Then we resume producer in call to set() below and it continues processing remaining
        // 500'000 elements, this time with producer driving the interaction.
        event.set();
        co_return;
    };

    single_consumer_event event;

    sync_wait_all(
        consumer(makeSequence(event)),
        unblocker(event));
}

// test an async_generator piped to view::filter
void test_async_filter_view()
{
    sync_wait([]() -> task<>
    {
        bool startedExecution = false;
        auto makeGenerator = [&]() -> async_generator<std::uint32_t>
        {
            startedExecution = true;
            co_yield 1;
            co_yield 2;
            co_yield 3;
            co_yield 4;
        };

        auto gen = makeGenerator()
                 | ranges::view::filter([](std::uint32_t i){return i % 2 == 0;});

        CHECK(!startedExecution);

        auto it = co_await gen.begin();
        CHECK(startedExecution);
        CHECK(it != gen.end());
        CHECK(*it == 2u);
        CHECK(co_await ++it != gen.end());
        CHECK(*it == 4u);
        CHECK(co_await ++it == gen.end());
    }());
}

int main()
{
    ::default_ctor_async_generator_is_empty();
    ::async_generator_does_not_start_if_begin_not_called();
    ::enumerate_sequence_of_one_value();
    ::enumerate_sequence_of_multiple_values();
    ::destructors_of_values_in_scope_are_called_on_early_destruction();
    ::async_producer_with_async_consumer();
    ::exception_before_yield_is_rethrow();
    ::exception_after_yield_rethrown_by_increment();
    ::no_stack_overflow_for_many_sync_completions();
    ::no_stack_overflow_for_many_sync_completions();
    ::test_async_filter_view();

    return ::test_result();
}
#else // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
int main() {}
#endif // RANGES_CXX_COROUTINES >= RANGES_CXX_COROUTINES_TS1
