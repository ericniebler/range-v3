Quick Start
===========

Range v3 is a generic library that augments the existing standard library with facilities for working with *ranges*. A range can be loosely thought of a pair of iterators, although they need not be implemented that way. Bundling begin/end iterators into a single object brings several benefits.

## Why Use Ranges?

### Convenience

It's more convenient to pass a single range object to an algorithm than separate begin/end iterators. Compare:

````c++
std::vector<int> v{/*...*/};
std::sort( v.begin(), v.end() );
````

with

````c++
std::vector<int> v{/*...*/};
ranges::sort( v );
````

Range v3 contains a full implementation of all the standard algorithms with range-based overloads for convenience.

### Composability

Having a single range object permits *pipelines* of operations. In a pipeline, a range is lazily adapted or eagerly mutated in some way, with the result immediately available for further adaptation or mutation. Lazy adaption is handled by *range views*, and eager mutation is handled by *container algorithms*.

#### Range views

A range view is a lightweight wrapper that presents a view of an underlying sequence of elements in some custom way without mutating or copying it. Views are cheap to create and copy, and have non-owning reference semantics. Below are some examples:

1. Filter a container using a predicate and transform it.

    ````c++
    std::vector<int> vi{1,2,3,4,5,6,7,8,9,10};
    using namespace ranges;
    auto rng = vi | view::filter([](int i){return i % 2 == 0;})
                  | view::transform([](int i){return std::to_string(i);});
    // rng == {"2","4","6","8","10"};
    ````

2. Generate an infinite list of integers starting at 1, square them, take the first 10, and sum them:

    ````c++
    using namespace ranges;
    int sum = accumulate(view::ints(1)
                       | view::transform([](int i){return i*i;})
                       | view::take(10), 0);
    ````

3. Generate a sequence on the fly with a range comprehension and initialize a vector with it:

    ````c++
    using namespace ranges;
    std::vector<int> vi =
        view::for_each(view::ints(1,10), [](int i){
            return yield_from(view::repeat(i,i));
        });
    // vi == {1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,...}
    ````

#### Container Algorithms

When you want to mutate a container in-place, or forward it through a chain of mutating operations, you can use container algorithms. The following examples should make it clear.

1. Read data into a vector, sort it, and make it unique.

    ````c++
    extern std::vector<int> read_data();
    using namespace ranges;
    std::vector<int> vi = read_data() | cont::sort | cont::unique;
    ````

2. Do the same to a `vector` that already contains some data:

    ````c++
    vi = std::move(vi) | cont::sort | cont::unique;
    ````

3. Mutate the container in-place:

    ````c++
    vi |= cont::sort | cont::unique;
    ````

4. Same as above, but with function-call syntax instead of pipe syntax:

    ````c++
    cont::unique(cont::sort(vi));
    ````

## Create Custom Ranges

Range v3 provides a utility for easily creating your own range types, called `range_facade`. The Code below uses `range_facade` to create a range that traverses a null-terminated string:

````c++
#include <range/v3/all.hpp>
using namespace ranges;

// A range that iterates over all the characters in a
// null-terminated string.
class c_string_range
  : public range_facade<c_string_range>
{
    friend range_access;
    char const * sz_;
    char const & current() const { return *sz_; }
    bool done() const { return *sz_ == '\0'; }
    void next() { ++sz_; }
public:
    c_string_range() = default;
    explicit c_string_range(char const *sz) : sz_(sz)
    {
        assert(sz != nullptr);
    }
};
````

The `range_facade` class generates an iterator and begin/end member functions from the minimal interface provided by `c_string_range`. This is an example of a very simple range for which it is not necessary to separate the range itself from the thing that iterates the range. Future examples will show examples of more sophisticated ranges.

With `c_string_range`, you can now use algorithms to operate on null-terminated strings, as below:

````c++
#include <iostream>

int main()
{
    c_string_range r("hello world");
    // Iterate over all the characters and print them out
    ranges::for_each(r, [](char ch){
        std::cout << ch << ' ';
    });
    // prints: h e l l o   w o r l d
}
````

## Create Custom Range Views

TODO

## Constrain Functions with Range and Iterator Concepts

TODO

## Range v3 and the Future

Range v3 forms the basis for a proposal to add ranges to the standard library
([N4128](www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4128.html)), and will also be the basis for a Technical Specification on Ranges. Its design direction has already passed an initial review by the standardization committee. What that means is that you may see your compiler vendor shipping a library like Range v3 at some point in the future. That's the hope, anyway.

Enjoy!
