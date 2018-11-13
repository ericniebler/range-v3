#include <experimental/coroutine>

struct present
{
    struct promise_type
    {
        int result;

        present get_return_object() { return {*this}; }
        std::experimental::suspend_never initial_suspend() { return {}; }
        std::experimental::suspend_never final_suspend() { return {}; }
        void return_value(int i) { result = i; }
        void unhandled_exception() {}
    };

    promise_type& promise;

    bool await_ready() const { return true; }
    void await_suspend(std::experimental::coroutine_handle<>) const {}
    int await_resume() const { return promise.result; }
};

present f(int n)
{
    if (n < 2)
        co_return 1;
    else
        co_return n * co_await f(n - 1);
}

int main()
{
    return f(5).promise.result != 120;
}
