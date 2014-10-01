range-v3
========

Range library for C++11/14/17. This code is the basis of a formal proposal to add range support to the C++ standard library (N4128).

**Development Status:** This code is fairly stable, well-tested, and suitable for casual use, although currently lacking documentation. No promise is made about support or long-term stability. This code *will* evolve without regard to backwards compatibility.

License:
--------

Most of the source code in this project are mine, and those are under the Boost Software License. Parts are taken from Alex Stepanov's Elements of Programming, Howard Hinnant's libc++, and from the SGI STL. Please see the attached LICENSE file and the CREDITS file for the licensing and acknolwedgements.

Supported Compilers
-------------------

The code is known to work on the following compilers:

- Clang, svn trunk
- GCC 4.9.0

To Do:
------

* Survey the use of projections. Do they only apply when evaluating predicates, or are they
  like full input transformations? (E.g, does `copy` get a projection parameter or not? Does the projection get applied by e.g. `set_difference`?)
* Longer-term goals:
  - Make `inplace_merge` work with forward iterators
  - Make the sorting algorithms work with forward iterators

Design Decisions, Guidelines, and Rationale:
--------------------------------------------

The following is true of Ranges:

- The type of the end iterator (hereafter refered to as a "sentinel") can differ from that of the begin iterator.
- When a Range is destroyed, any iterators generated from that range are invalidated.
- Ranges do not logically own their elements. They refer to elements stored elsewhere (or are generated on demand).
- Ranges are Regular types, so copies are independent. An iterator from a Range `A` that was copied from `B`
  is not invalidated when `B` is destroyed.
- If an un-cv-qualified type `T` models Range, then the type `T const` need not. This permits ranges that maintain
  mutable internal state; e.g., an `istream` range.

The following is true of algorithms:

- Algorithms are changed to reflect the fact that the sentinel may have a different type than the begin iterator.
- Algorithms, in addition to the old versions that take begin/end iterator/sentinel arguments, now have versions
  that take Iterable arguments in place of begin/end pairs.
- Algorithm versions that take Iterables are semantically identical to the identically-named versions that takes
  iterator/sentinel pairs. The two flavors have the same return types. Both evaluate eagerly.
- Algorithms that necessarily process their entire input sequence return the iterator position at the end in
  addition to whatever else they return. The purpose is to return potentially useful information that is computed
  as a side-effect of the normal execution of the algorithm; for example, the position of a C-style string's null
  terminator. Exceptions to this design guideline are made when one of the following is true:
    * The algorithm might in some cases not consume the entire input sequence. (The point of this exception is
      to avoid forcing the algorithm to compute something that is not necessary for successful completion. For
      example, `find`.)
    * When the sole purpose of the algorithm is specifically to compute a single value; hence, changing
      the return type will necessarily break code using the C++11 version. Examples include `is_sorted` and
      `accumulate`.
- "Three-legged" iterator-based algorithms now have 4 versions:
    * The old three-legged iterator version (that has traditionally merely assumed the sequence denoted by the third
      iterator is long enough),
    * A four-legged version that uses the sentinel of the second sequence as an additional termination condition,
    * A version that takes an Iterable and an Iterator (which dispatches to the three-legged iterator-based version),
      and
    * A version that takes two Iterables (which dispatches to the four-legged iterator-based version).
- Purely as an implementation consideration, the three-legged algorithms must be able to distinguish a native array
  from an iterator; e.g. when the user calls the algorithm with an array as the second input sequence (where either
  an Iterable or an iterator is allowed to appear). Naively coded, this would be ambiguous due to the decay of arrays
  to pointers. In this case, it should dispatch to the version that takes two Iterables, not the version that takes
  an Iterable and an iterator.
- Algorithms that do not mutate their input sequence must also work when `initializer_list`s are used in place of
  Iterables.
- If an algorithm returns an iterator into an Iterable argument, the Iterable must be an lvalue. This is to avoid
  returning an iterator that is immediately made invalid. Conversely, if no iterator into an Iterable argument is
  returned, then the Iterable should be taken by "universal reference".
- Algorithms that take callables should work with any kind of INVOKE-able entity (see [func.require]); for example,
  pointers to data members are valid unary INVOKE-able objects.
- Wherever appropriate, algorithms should optionally take INVOKE-able *projections* that are applied to each element
  in the input sequence(s). This, in effect, allows users to trivially transform each input sequence for the sake
  of that single algorithm invocation. The reason for projections is described in
  [Sean Parent's "C++ Seasoning" talk] [6] on slide 38.
- Algorithms that take two input sequences should (optionally) take two projections.
- For algorithms that optionally accept functions/predicates (e.g. `transform`, `sort`), projection arguments
  positionally follow functions/predicates. There are no algorithm overloads that allow the user to specify the
  projection without also specifying a predicate, even if the default would suffice. This is to reduce the
  number of overloads and also to avoid any potential for ambiguity.
