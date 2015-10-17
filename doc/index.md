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
            return yield_from(view::repeat_n(i,i));
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

Range v3 provides a utility for easily creating your own range types, called `view_facade`. The code below uses `view_facade` to create a range that traverses a null-terminated string:

~~~~~~~{.cpp}
    #include <range/v3/all.hpp>
    using namespace ranges;

    // A range that iterates over all the characters in a
    // null-terminated string.
    class c_string_range
      : public view_facade<c_string_range>
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

The `view_facade` class generates an iterator and begin/end member functions from the minimal interface provided by `c_string_range`. This is an example of a very simple range for which it is not necessary to separate the range itself from the thing that iterates the range. Future examples will show examples of more sophisticated ranges.

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

Often, a new range type is most easily expressed by adapting an existing range type. That's the case for many of the range views provided by the Range v3 library; for example, the `view::remove_if` and `view::transform` views. These are rich types with many moving parts, but thanks to a helper class called `view_adaptor`, they aren't hard to write.

Below in roughly 2 dozen lines of code is the `transform` view, which takes one range and transforms all the elements with a unary function.

~~~~~~~{.cpp}
    // A class that adapts an existing range with a function
    template<class Rng, class Fun>
    class transform_view : public view_adaptor<transform_view<Rng, Fun>, Rng>
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
          : view_adaptor_t<transform_view>{std::forward<Rng>(rng)}
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

You can then add an overload that take a Range:

~~~~~~~{.cpp}
    template<class Rng, class Comp = /*...some_default..*/,
        CONCEPT_REQUIRES_(Range<Rng>())>
    void my_algorithm(Rng && rng, Comp comp = Comp{})
    {
        return my_algorithm(ranges::begin(rng), ranges::end(rng));
    }
~~~~~~~

With the type constraints expressed with the `CONCEPTS_REQUIRES_` macro, these two overloads are guaranteed to not be ambiguous.

## Range v3 and the Future

Range v3 forms the basis for a proposal to add ranges to the standard library
([N4128](www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4128.html)), and will also be the basis for a Technical Specification on Ranges. Its design direction has already passed an initial review by the standardization committee. What that means is that you may see your compiler vendor shipping a library like Range v3 at some point in the future. That's the hope, anyway.

Enjoy!

\section range-views Range Views

--------------------------------------------
The big advantage of ranges over iterators is their *composability*. They permit a functional style of programming where data is manipulated by passing it through a series of combinators. In addition, the combinators can be *lazy*, only doing work when the answer is requested, and *purely functional*, without mutating the original data. This makes it easier to reason about your code, especially when writing concurrent programs.

Below is a list of the lazy range combinators, or *views*, that Range v3 provides, and a blurb about how each is intended to be used.

<DL>
<DT>\link ranges::v3::view::adjacent_remove_if_fn `view::adjacent_remove_if`\endlink</DT>
  <DD>For each pair of adjacent elements in a source range, evaluate the specified binary predicate. If the predicate evaluates to false, the first element of the pair is included in the result range; otherwise, it is skipped. The first element in the source range is always included. (For instance, `adjacent_remove_if` with `std::equal_to` filters out all the non-unique elements.)</DD>
<DT>\link ranges::v3::view::all_fn `view::all`\endlink</DT>
  <DD>Return a range containing all the elements in the source. Useful for converting containers to ranges.</DD>
<DT>\link ranges::v3::view::bounded_fn `view::bounded`\endlink</DT>
  <DD>Convert the source range to a *bounded* range, where the type of the `end` is the same as the `begin`. Useful for iterating over a range with C++'s range-based `for` loop.</DD>
<DT>\link ranges::v3::view::chunk_fn `view::chunk`\endlink</DT>
  <DD>Given a source range and an integer *N*, produce a range of contiguous ranges where each inner range has *N* contiguous elements. The final range may have fewer than *N* elements.</DD>
<DT>\link ranges::v3::view::concat_fn `view::concat`\endlink</DT>
  <DD>Given *N* source ranges, produce a result range that is the concatenation of all of them.</DD>
<DT>\link ranges::v3::view::const_fn `view::const_`\endlink</DT>
  <DD>Present a `const` view of a source range.</DD>
<DT>\link ranges::v3::view::counted_fn `view::counted`\endlink</DT>
  <DD>Given an iterator `it` and a count `n`, create a range that starts at `it` and includes the next `n` elements.</DD>
<DT>\link ranges::v3::view::delimit_fn `view::delimit`\endlink</DT>
  <DD>Given a source range and a value, return a new range that ends either at the end of the source or at the first occurrence of the value, whichever comes first. Alternatively, `view::delimit` can be called with an iterator and a value, in which case it returns a range that starts at the specified position and ends at the first occurrence of the value.</DD>
<DT>\link ranges::v3::view::drop_fn `view::drop`\endlink</DT>
  <DD>Remove the first *N* elements from the front of a source range.</DD>
<DT>\link ranges::v3::view::drop_while_fn `view::drop_while`\endlink</DT>
  <DD>Remove elements from the front of a range that satisfy a unary predicate.</DD>
<DT>\link ranges::v3::view::empty() `view::empty`\endlink</DT>
  <DD>Create an empty range with a given value type.</DD>
<DT>\link ranges::v3::view::generate_fn `view::generate`\endlink</DT>
  <DD>Given a nullary function, return an infinite range whose elements are generated with the function.</DD>
<DT>\link ranges::v3::view::generate_n_fn `view::generate_n`\endlink</DT>
  <DD>Given a nullary function and a count, return a range that generates the requested number of elements by calling the function.</DD>
<DT>\link ranges::v3::view::group_by_fn `view::group_by`\endlink</DT>
  <DD>Given a source range and a binary predicate, return a range of ranges where each range contains contiguous elements from the source range such that the following condition holds: for each element in the range apart from the first, when that element and the first element are passed to the binary predicate, the result is true. In essence, `view::group_by` *groups* contiguous elements together with a binary predicate.</DD>
<DT>\link ranges::v3::view::indirect_fn `view::indirect`\endlink</DT>
  <DD>Given a source range of readable values (e.g. pointers or iterators), return a new view that is the result of dereferencing each.</DD>
<DT>\link ranges::v3::view::intersperse_fn `view::intersperse`\endlink</DT>
  <DD>Given a source range and a value, return a new range where the value is inserted between contiguous elements from the source.</DD>
<DT>\link ranges::v3::view::ints_fn `view::ints`\endlink</DT>
  <DD>Generate a range of monotonically increasing `int`s. When used without arguments, it generates the quasi-infinite range [0,1,2,3...]. It can also be called with a lower bound, or with a lower and upper bound (exclusive).</DD>
<DT>\link ranges::v3::view::iota_fn `view::iota`\endlink</DT>
  <DD>A generalization of `view::ints` that generates a sequence of monotonically increasing values of any incrementable type. When specified with a single argument, the result is an infinite range beginning at the specified value. With two arguments, the values are assumed to denote a half-open range.</DD>
<DT>\link ranges::v3::view::join_fn `view::join`\endlink</DT>
  <DD>Given a range of ranges, join them into a flattened sequence of elements. Optionally, you can specify a value or a range to be inserted between each source range.</DD>
<DT>\link ranges::v3::view::keys_fn `view::keys`\endlink</DT>
  <DD>Given a range of `pair`s (like a `std::map`), return a new range consisting of just the first element of the `pair`.</DD>
<DT>\link ranges::v3::view::move_fn `view::move`\endlink</DT>
  <DD>Given a source range, return a new range where each element has been has been cast to an rvalue reference.</DD>
<DT>\link ranges::v3::view::partial_sum_fn `view::partial_sum`\endlink</DT>
  <DD>Given a range and a binary function, return a new range where the *N*<SUP>th</SUP> element is the result of applying the function to the *N*<SUP>th</SUP> element from the source range and the (N-1)th element from the result range.</DD>
<DT>\link ranges::v3::view::remove_if_fn `view::remove_if`\endlink</DT>
  <DD>Given a source range and a unary predicate, filter out those elements that do not satisfy the predicate. (For users of Boost.Range, this is like the `filter` adaptor with the predicate negated.)</DD>
<DT>\link ranges::v3::view::repeat_fn `view::repeat`\endlink</DT>
  <DD>Given a value, create a range that is that value repeated infinitely.</DD>
<DT>\link ranges::v3::view::repeat_n_fn `view::repeat_n`\endlink</DT>
  <DD>Given a value and a count, create a range that is that value repeated *count* number of times.</DD>
<DT>\link ranges::v3::view::replace_fn `view::replace`\endlink</DT>
  <DD>Given a source range, a source value and a target value, create a new range where all elements equal to the source value are replaced with the target value.</DD>
<DT>\link ranges::v3::view::replace_if_fn `view::replace_if`\endlink</DT>
  <DD>Given a source range, a unary predicate and a target value, create a new range where all elements that satisfy the predicate are replaced with the target value.</DD>
<DT>\link ranges::v3::view::reverse_fn `view::reverse`\endlink</DT>
  <DD>Create a new range that traverses the source range in reverse order.</DD>
<DT>\link ranges::v3::view::single_fn `view::single`\endlink</DT>
  <DD>Given a value, create a range with exactly one element.</DD>
<DT>\link ranges::v3::view::slice_fn `view::slice`\endlink</DT>
  <DD>Give a source range a lower bound (inclusive) and an upper bound (exclusive), create a new range that begins and ends at the specified offsets. Both the begin and the end can be integers relative to the front, or relative to the end with "`end-2`" syntax.</DD>
<DT>\link ranges::v3::view::split_fn `view::split`\endlink</DT>
  <DD>Given a source range and a delimiter specifier, split the source range into a range of ranges using the delimiter specifier to find the boundaries. The delimiter specifier can be a value, a subrange, or a function. The function should accept current/end iterators into the source range and return `make_pair(true, length)` if the current position is a boundary; otherwise, `make_pair(false, 0)`. The delimiter character(s) are excluded from the resulting range of ranges.</DD>
<DT>\link ranges::v3::view::stride_fn `view::stride`\endlink</DT>
  <DD>Given a source range and an integral stride value, return a range consisting of every *N*<SUP>th</SUP> element, starting with the first.</DD>
<DT>\link ranges::v3::view::tail_fn `view::tail`\endlink</DT>
  <DD>Given a source range, return a new range without the first element. The range must have at least one element.</DD>
<DT>\link ranges::v3::view::take_fn `view::take`\endlink</DT>
  <DD>Given a source range and an integral count, return a range consisting of the first *count* elements from the source range, or the complete range if it has fewer elements. (The result of `view::take` is not a `SizedRange`.)</DD>
<DT>\link ranges::v3::view::take_exactly_fn `view::take_exactly`\endlink</DT>
  <DD>Given a source range and an integral count, return a range consisting of the first *count* elements from the source range. The source range must have at least that many elements. (The result of `view::take_exactly` is a `SizedRange`.)</DD>
<DT>\link ranges::v3::view::take_while_fn `view::take_while`\endlink</DT>
  <DD>Given a source range and a unary predicate, return a new range consisting of the  elements from the front that satisfy the predicate.</DD>
<DT>\link ranges::v3::view::tokenize_fn `view::tokenize`\endlink</DT>
  <DD>Given a source range and optionally a submatch specifier and a `std::regex_constants::match_flag_type`, return a `std::regex_token_iterator` to step through the regex submatches of the source range. The submatch specifier may be either a plain `int`, a `std::vector<int>`, or a `std::initializer_list<int>`.</DD>
<DT>\link ranges::v3::view::transform_fn `view::transform`\endlink</DT>
  <DD>Given a source range and a unary function, return a new range where each result element is the result of applying the unary function to a source element.</DD>
<DT>\link ranges::v3::view::unbounded_fn `view::unbounded`\endlink</DT>
  <DD>Given an iterator, return an infinite range that begins at that position.</DD>
<DT>\link ranges::v3::view::unique_fn `view::unique`\endlink</DT>
  <DD>Given a range, return a new range where all consecutive elements that compare equal save the first have been filtered out.</DD>
<DT>\link ranges::v3::view::values_fn `view::values`\endlink</DT>
  <DD>Given a range of `pair`s (like a `std::map`), return a new range consisting of just the second element of the `pair`.</DD>
<DT>\link ranges::v3::view::zip_fn `view::zip`\endlink</DT>
  <DD>Given *N* ranges, return a new range where *M*<SUP>th</SUP> element is the result of calling `make_tuple` on the *M*<SUP>th</SUP> elements of all *N* ranges.</DD>
<DT>\link ranges::v3::view::zip_with_fn `view::zip_with`\endlink</DT>
  <DD>Given *N* ranges and a *N*-ary function, return a new range where *M*<SUP>th</SUP> element is the result of calling the function on the *M*<SUP>th</SUP> elements of all *N* ranges.</DD>
</DL>
