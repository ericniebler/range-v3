User Manual       {#mainpage}
===========

\tableofcontents

\section tutorial-preface Preface

--------------------------------------------
Range library for C++11/14/17. This code is the basis of [a formal proposal](https://ericniebler.github.io/std/wg21/D4128.html) to add range support to the C++ standard library.

**Development Status:**

This code is fairly stable, well-tested, and suitable for casual use, although currently lacking documentation. No promise is made about support or long-term stability. This code *will* evolve without regard to backwards compatibility.

\subsection tutorial-installation Installation

--------------------------------------------
This library is header-only. You can get the source code from the [range-v3 repository](https://github.com/ericniebler/range-v3) on github. To compile with Range-v3, you can either `#%include` the entire library:

~~~~~~~{.cpp}
#include <range/v3/all.hpp>
~~~~~~~

Or you can `#%include` only the core, and then the individual headers you want:

~~~~~~~{.cpp}
#include <range/v3/core.hpp>
#include <range/v3/....
~~~~~~~

\subsection tutorial-license License

--------------------------------------------
Most of the source code in this project are mine, and those are under the Boost Software License. Parts are taken from Alex Stepanov's Elements of Programming, Howard Hinnant's libc++, and from the SGI STL. Please see the attached LICENSE file and the CREDITS file for the licensing and acknowledgements.

\subsection tutorial-compilers Supported Compilers

--------------------------------------------
The code is known to work on the following compilers:

- clang 3.4.0
- GCC 4.9.0

\section tutorial-quick-start Quick Start

--------------------------------------------
Range v3 is a generic library that augments the existing standard library with facilities for working with *ranges*. A range can be loosely thought of a pair of iterators, although they need not be implemented that way. Bundling begin/end iterators into a single object brings several benefits.

## Why Use Ranges?

### Convenience

It's more convenient to pass a single range object to an algorithm than separate begin/end iterators. Compare:

~~~~~~~{.cpp}
    std::vector<int> v{/*...*/};
    std::sort( v.begin(), v.end() );
~~~~~~~

with

~~~~~~~{.cpp}
    std::vector<int> v{/*...*/};
    ranges::sort( v );
~~~~~~~

Range v3 contains a full implementation of all the standard algorithms with range-based overloads for convenience.

### Composability

Having a single range object permits *pipelines* of operations. In a pipeline, a range is lazily adapted or eagerly mutated in some way, with the result immediately available for further adaptation or mutation. Lazy adaption is handled by *views*, and eager mutation is handled by *actions*.

#### Views

A view is a lightweight wrapper that presents a view of an underlying sequence of elements in some custom way without mutating or copying it. Views are cheap to create and copy, and have non-owning reference semantics. Below are some examples:

Filter a container using a predicate and transform it.

~~~~~~~{.cpp}
    std::vector<int> vi{1,2,3,4,5,6,7,8,9,10};
    using namespace ranges;
    auto rng = vi | view::remove_if([](int i){return i % 2 == 1;})
                  | view::transform([](int i){return std::to_string(i);});
    // rng == {"2","4","6","8","10"};
~~~~~~~

Generate an infinite list of integers starting at 1, square them, take the first 10, and sum them:

~~~~~~~{.cpp}
    using namespace ranges;
    int sum = accumulate(view::ints(1)
                       | view::transform([](int i){return i*i;})
                       | view::take(10), 0);
~~~~~~~

Generate a sequence on the fly with a range comprehension and initialize a vector with it:

~~~~~~~{.cpp}
    using namespace ranges;
    std::vector<int> vi =
        view::for_each(view::ints(1,10), [](int i){
            return yield_from(view::repeat(i,i));
        });
    // vi == {1,2,2,3,3,3,4,4,4,4,5,5,5,5,5,...}
~~~~~~~

#### Actions

When you want to mutate a container in-place, or forward it through a chain of mutating operations, you can use actions. The following examples should make it clear.

Read data into a vector, sort it, and make it unique.

~~~~~~~{.cpp}
    extern std::vector<int> read_data();
    using namespace ranges;
    std::vector<int> vi = read_data() | action::sort | action::unique;
~~~~~~~

Do the same to a `vector` that already contains some data:

~~~~~~~{.cpp}
    vi = std::move(vi) | action::sort | action::unique;
~~~~~~~

Mutate the container in-place:

~~~~~~~{.cpp}
    vi |= action::sort | action::unique;
~~~~~~~

Same as above, but with function-call syntax instead of pipe syntax:

~~~~~~~{.cpp}
    action::unique(action::sort(vi));
~~~~~~~

## Create Custom Ranges

Range v3 provides a utility for easily creating your own range types, called `range_facade`. The code below uses `range_facade` to create a range that traverses a null-terminated string:

~~~~~~~{.cpp}
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
~~~~~~~

The `range_facade` class generates an iterator and begin/end member functions from the minimal interface provided by `c_string_range`. This is an example of a very simple range for which it is not necessary to separate the range itself from the thing that iterates the range. Future examples will show examples of more sophisticated ranges.

With `c_string_range`, you can now use algorithms to operate on null-terminated strings, as below:

~~~~~~~{.cpp}
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
~~~~~~~

## Adapting Ranges

Often, a new range type is most easily expressed by adapting an existing range type. That's the case for many of the range views provided by the Range v3 library; for example, the `view::remove_if` and `view::transform` views. These are rich types with many moving parts, but thanks to a helper class called `range_adaptor`, they aren't hard to write.

Below in roughly 2 dozen lines of code is the `transform` view, which takes one range and transforms all the elements with a unary function.

~~~~~~~{.cpp}
    // A class that adapts an existing range with a function
    template<class Rng, class Fun>
    class transform_view : public range_adaptor<transform_view<Rng, Fun>, Rng>
    {
        friend range_access;
        semiregular_t<Fun> fun_; // Make Fun model SemiRegular if it doesn't
        class adaptor : public adaptor_base
        {
            semiregular_t<Fun> fun_;
        public:
            adaptor() = default;
            adaptor(semiregular_t<Fun> const &fun) : fun_(fun) {}
            // Here is where we apply Fun to the elements:
            auto current(range_iterator_t<Rng> it) const -> decltype(fun_(*it))
            {
                return fun_(*it);
            }
        };
        adaptor begin_adaptor() const { return {fun_}; }
        adaptor end_adaptor() const { return {fun_}; }
    public:
        transform_view() = default;
        transform_view(Rng && rng, Fun fun)
          : range_adaptor_t<transform_view>{std::forward<Rng>(rng)}
          , fun_(std::move(fun))
        {}
    };

    template<class Rng, class Fun>
    transform_view<Rng, Fun> transform(Rng && rng, Fun fun)
    {
        return {std::forward<Rng>(rng), std::move(fun)};
    }
~~~~~~~

Range transformation is achieved by defining a nested `adaptor` class that handles the transformation, and then defining `begin_adaptor` and `end_adaptor` members that return adaptors for the begin iterator and the end sentinel, respectively. The `adaptor` class has a `current` member that performs the transformation. It is passed an iterator to the current element. Other members are available for customization: `equal`, `next`, `prev`, `advance`, and `distance_to`; but the transform adaptor accepts the defaults defined in `adaptor_base`.

With `transform_view`, we can print out the first 20 squares:

~~~~~~~{.cpp}
    int main()
    {
        auto squares = ::transform(view::ints(1), [](int i){return i*i;});
        for(int i : squares | view::take(20))
            std::cout << i << ' ';
        std::cout << '\n';
        // prints 1 4 9 16 25 36 49 64 81 100 121 144 169 196 225 256 289 324 361 400
    }
~~~~~~~

The `transform_view` defined above is an InputRange when its wrapping an InputRange, a ForwardRange when its wrapping a ForwardRange, etc. That happens because of smart defaults defined in the `adaptor_base` class. That frees you from having to deal with a host of niggly detail when implementing iterators.

*(Note: the above `transform_view` always stores a copy of the function in the sentinel. That is only necessary if the underlying range's sentinel type models BidirectionalIterator. That's a finer point that you shouldn't worry about right now.)*

## Constrain Functions with Concepts

The Range v3 library makes heavy use of concepts to constrain functions, control overloading, and check type constraints at compile-time. It achieves this with the help of a Concepts Lite emulation layer that works on any standard-conforming C++11 compiler. The library provides many useful concepts, both for the core language and for iterators and ranges. You can use the concepts framework to constrain your own code.

For instance, if you would like to write a function that takes an iterator/sentinel pair, you can write it like this:

~~~~~~~{.cpp}
    template<class Iter, class Sent, class Comp = /*...some_default..*/,
        CONCEPT_REQUIRES_(IteratorRange<Iter, Sent>())>
    void my_algorithm(Iter first, Sent last, Comp comp = Comp{})
    {
        // ...
    }
~~~~~~~

You can then add an overload that take an Iterable:

~~~~~~~{.cpp}
    template<class Rng, class Comp = /*...some_default..*/,
        CONCEPT_REQUIRES_(Iterable<Rng>())>
    void my_algorithm(Rng && rng, Comp comp = Comp{})
    {
        return my_algorithm(ranges::begin(rng), ranges::end(rng));
    }
~~~~~~~

With the type constraits expressed with the `CONCEPTS_REQUIRES_` macro, these two overloads are guaranteed to not be ambiguous.

## Range v3 and the Future

Range v3 forms the basis for a proposal to add ranges to the standard library
([N4128](www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4128.html)), and will also be the basis for a Technical Specification on Ranges. Its design direction has already passed an initial review by the standardization committee. What that means is that you may see your compiler vendor shipping a library like Range v3 at some point in the future. That's the hope, anyway.

Enjoy!
