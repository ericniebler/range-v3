range-v3
========

Range library for C++11/14/17. This code is the basis of [a formal proposal](https://ericniebler.github.io/std/wg21/D4128.html) to add range support to the C++ standard library.

About:
------

Why does C++ need another range library? Simply put, the existing solutions haven't kept up with the rapid evolution of C++. Range v3 is a library for the future C++. Not only does it work well with today's C++ -- move semantics, lambdas, automatically deduced types and all -- it also anticipates tomorrow's C++ with Concepts.

Range v3 forms the basis of a proposal to add range support to the standard library ([N4128: Ranges for the Standard Library](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4128.html)). It also will be the reference implementation for an upcoming Technical Specification. These are the first steps toward turning ranges into an international standard.

Documentation:
--------------

Check out the (woefully incomplete) documentation [here](https://ericniebler.github.io/range-v3/).

Other resources (mind the dates, the library probably has changed since then):

- Usage:
  - Talk: [CppCon 2015: Eric Niebler "Ranges for the Standard Library"](https://www.youtube.com/watch?v=mFUXNMfaciE), 2015.
  - [A slice of Python in C++](http://ericniebler.com/2014/12/07/a-slice-of-python-in-c/), 07.12.2014.
  - Actions (back then called [Container Algorithms](http://ericniebler.com/2014/11/23/container-algorithms/)), 23.11.2014.
  - [Range comprehensions](http://ericniebler.com/2014/04/27/range-comprehensions/), 27.04.2014.
  - [Input iterators vs input ranges](http://ericniebler.com/2013/11/07/input-iterators-vs-input-ranges/), 07.11.2013.

- Design / Implementation:
  - Rationale behind range-v3: [N4128: Ranges for the standard library Revision 1](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4128.html), 2014.
  - Ranges TS: [N4560: C++ Extensions for Ranges](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4560.pdf), 2015.
  - Implementation of customization points in range-v3:
    - [N4381: Suggested Design for Customization Points](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html), 2015.
    - [P0386: Inline variables](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0386r0.pdf), 2016.
    - [Customization Point Design in C++11 and Beyond](http://ericniebler.com/2014/10/21/customization-point-design-in-c11-and-beyond/), 2014.
  - Proxy iterators in range-v3:
    - [D0022: Proxy Iterators for the Ranges Extensions](https://ericniebler.github.io/std/wg21/D0022.html).
    - [To Be or Not to Be (an Iterator)](http://ericniebler.com/2015/01/28/to-be-or-not-to-be-an-iterator/), 2015.
    - [Iterators++: Part1](http://ericniebler.com/2015/02/03/iterators-plus-plus-part-1/), 2015.
    - [Iterators++: Part2](http://ericniebler.com/2015/02/13/iterators-plus-plus-part-2/), 2015.
    - [Iterators++: Part3](http://ericniebler.com/2015/03/03/iterators-plus-plus-part-3/), 2015.
  - Metaprogramming utilities:
    - See the [meta documentation](https://ericniebler.github.io/meta/index.html), the library has changed significantly since the [2014 blog post](http://ericniebler.com/2014/11/13/tiny-metaprogramming-library/).
  - Concept emulation layer: [Concept checking in C++11](http://ericniebler.com/2013/11/23/concept-checking-in-c11/), 2013.
  - [C++Now 2014: Eric Niebler "C++11 Library Design"](https://www.youtube.com/watch?v=zgOF4NrQllo), 2014.

License:
--------

Most of the source code in this project are mine, and those are under the Boost Software License. Parts are taken from Alex Stepanov's Elements of Programming, Howard Hinnant's libc++, and from the SGI STL. Please see the attached LICENSE file and the CREDITS file for the licensing and acknowledgments.

Supported Compilers
-------------------

The code is known to work on the following compilers:

- clang 3.6.2 (or later)
- GCC 4.9.1 (or later) (C++14 support requires GCC 5.2; C++14 "extended constexpr" support is poor before 6.1.)
- Clang/LLVM 6 (or later) on Windows (older versions may work - we haven't tested.)

[ Note: We've "retired" support for Clang/C2 with the VS2015 toolset (i.e., the `v140_clang_c2` toolset) which Microsoft no longer supports for C++ use. We no longer have CI runs, but haven't gone out of our way to break anything, so it will likely continue to work. ]

**Development Status:** This code is fairly stable, well-tested, and suitable for casual use, although currently lacking documentation. No promise is made about support or long-term stability. This code *will* evolve without regard to backwards compatibility.

**Build status**
- on Travis-CI: [![Travis Build Status](https://travis-ci.org/ericniebler/range-v3.svg?branch=master)](https://travis-ci.org/ericniebler/range-v3)
- on AppVeyor: [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/fwl9ymc2t6ukn9qj/branch/master?svg=true)](https://ci.appveyor.com/project/ericniebler/range-v3)

Release Notes:
--------------
* **0.5.0** Apr 30, 2019
  * **NEW:** MSVC support, from @CaseyCarter :tada: (See the docs for the list of supported compilers.)
  * **NEW:** `view::enumerate`, from @MikeGitb
  * **NEW:** `view::addressof`, from @tower120
  * **NEW:** `unstable_remove_if` algorithm and action, from @tower120
  * **NEW:** `adjacent_remove_if` algorithm and action, from @cjdb
  * **NEW:** `ostream_joiner`, from @sv1990
  * `view::drop_while` and `view::take_while` get projection support, from @mrpi
  * `view::filter` and `view::remove_if` get projection support, from @mrpi
  * `view::unique` accepts optional comparison operator, from @tete17
  * `action::slice` supports sliding from the end, from @tete17
  * Support coroutines on MSVC, from @CaseyCarter
  * Faster `view::generate_n`, from GitHub user @tower120
  * Improved aligned new detection for libc++ on iOS, from @mtak-
  * Various CMake improvements, from @johelegp
  * `view_adaptor` supports `basic_iterator`-style mixins, from @tower120
  * Fix `ranges::advance` for random-access iterators for `n==0`, from @tower120
  * Bugs fixed: [#755](https://github.com/ericniebler/range-v3/issues/755), [#759](https://github.com/ericniebler/range-v3/issues/759), [#942](https://github.com/ericniebler/range-v3/issues/942), [#946](https://github.com/ericniebler/range-v3/issues/946), [#952](https://github.com/ericniebler/range-v3/issues/952), [#975](https://github.com/ericniebler/range-v3/issues/975), [#978](https://github.com/ericniebler/range-v3/issues/978), [#986](https://github.com/ericniebler/range-v3/issues/986), [#996](https://github.com/ericniebler/range-v3/issues/996), [#1041](https://github.com/ericniebler/range-v3/issues/1041), [#1047](https://github.com/ericniebler/range-v3/issues/1047), [#1088](https://github.com/ericniebler/range-v3/issues/1088), [#1094](https://github.com/ericniebler/range-v3/issues/1094), [#1107](https://github.com/ericniebler/range-v3/issues/1107), [#1129](https://github.com/ericniebler/range-v3/issues/1129)

* **0.4.0** Oct 18, 2018
  - Minor interface-breaking changes:
    * `single_view` returns by `const &` (see [#817](https://github.com/ericniebler/range-v3/issues/817)).
    * `reverse_view` of a non-Sized, non-Bounded RandomAccess range (eg., a null-terminated string) no longer satisfies SizedRange.
    * The `generate` and `generate_n` views now return the generated values by xvalue reference (`T &&`) to the value cached within the view (see [#905](https://github.com/ericniebler/range-v3/issues/905)).
    * Views no longer prefer returning constant iterators when they can; some views have different constant and mutable iterators.
  - Enhancements:
    * Views can successfully adapt other views that have different constant and mutable iterators.
    * The `single` and `empty` views are much closer to the versions as specified in [P0896](http://wg21.link/P0896).
  - Bug fixes:
    * "single_view should not copy the value" [#817](https://github.com/ericniebler/range-v3/issues/817).
    * "Calling back() on strided range does not return the correct last value in range" [#901](https://github.com/ericniebler/range-v3/issues/901).
    * "generate(foo) | take(n) calls foo n+1 times" [#819](https://github.com/ericniebler/range-v3/issues/819).
    * "generate seems broken with move-only return types" [#905](https://github.com/ericniebler/range-v3/issues/905).
    * "Unexpected behavior in generate with return by reference" [#807](https://github.com/ericniebler/range-v3/issues/807).
    * "Inconsistent behaviour of ranges::distance with ranges::view::zip using infinite views." [#783](https://github.com/ericniebler/range-v3/issues/783).
    * "Infinite loop when using ranges::view::cycle with an infinite range" [#780](https://github.com/ericniebler/range-v3/issues/780).
    * "Composing ranges::view::cycle with ranges::view::slice" [#778](https://github.com/ericniebler/range-v3/issues/778).
    * "cartesian_product view, now with moar bugs." [#919](https://github.com/ericniebler/range-v3/issues/919).
* **0.3.7** Sept 19, 2018
  - Improved support for clang-cl (thanks to @CaseyCarter).
  - Fix for `any_view<T, category::sized | category::input>` (see #869).
  - Fix `iter_move` of a `ranges::reverse_iterator` (see #888).
  - Fix `move_sentinel` comparisons (see #889).
  - Avoid ambiguity created by `boost::advance` and `std::advance` (see #893).
* **0.3.6** May 15, 2018
  - NEW: `view::exclusive_scan` (thanks to GitHub user @mitsutaka-takeda).
  - All views get non-`const` overloads of `.empty()` and `.size()` (see [ericniebler/stl2\#793](https://github.com/ericniebler/stl2/issues/793)).
  - Upgrade Conan support for conan 1.0.
  - `subspan` interface tweaks.
  - Fix bug in `view::split` (see [this stackoverflow question](https://stackoverflow.com/questions/49015671)).
  - Fix bug in `view::stride` (see [ericniebler/stl2\#805](https://github.com/ericniebler/stl2/issues/805)).
  - Fix `const`-correctness problem in `view::chunk` (see [this stackoverflow question](https://stackoverflow.com/questions/49210190)).
  - Replace uses of `ranges::result_of` with `ranges::invoke_result`.
  - Fix potential buffer overrun of `view::drop` over RandomAccessRanges.
  - Lots of `view::cartesian_product` fixes (see [ericniebler/stl2\#820](https://github.com/ericniebler/stl2/issues/820), [ericniebler/stl2\#823](https://github.com/ericniebler/stl2/issues/823)).
  - Work around gcc-8 regression regarding `volatile` `std::initializer_list`s (see [ericniebler/stl2\#826](https://github.com/ericniebler/stl2/issues/826)).
  - Fix `const`-correctness problem of `view::take`.
* **0.3.5** February 17, 2018
  - Rvalues may satisfy `Writable` (see [ericniebler/stl2\#387](https://github.com/ericniebler/stl2/issues/387)).
  - `view_interface` gets a bounds-checking `at` method.
  - `chunk_view` works on Input ranges.
  - Fix bug in `group_by_view`.
  - Improved concept checks for `partial_sum` numeric algorithm.
  - Define `ContiguousIterator` concept and `contiguous_iterator_tag` iterator
    category tag.
  - Sundry `span` fixes.
  - `action::insert` avoids interfering with `vector`'s exponentional growth
    strategy.
  - Add an experimental `shared` view for views that need container-like scratch
    space to do their work.
  - Faster, simpler `reverse_view`.
  - Rework `ranges::reference_wrapper` to avoid [LWG\#2993](https://wg21.link/lwg2993).
  - Reworked `any_view`, the type-erased view wrapper.
  - `equal` algorithm is `constexpr` in C++14.
  - `stride_view` no longer needs an `atomic` data member.
  - `const`-correct `drop_view`.
  - `adjacent_filter_view` supports bidirectional iteration.
  - Massive `view_adaptor` cleanup to remove the need for a `mutable` data
    member holding the adapted view.
  - Fix `counting_iterator` post-increment bug.
  - `tail_view` of an empty range is an empty range, not undefined behavior.
  - Various portability fixes for gcc and clang trunk.
* **0.3.0** June 30, 2017
  - Input views may now be move-only (from @CaseyCarter)
  - Input `any_view`s are now *much* more efficient (from @CaseyCarter)
  - Better support for systems lacking a working `<thread>` header (from @CaseyCarter)
* **0.2.6** June 21, 2017
  - Experimental coroutines with `ranges::experimental::generator` (from @CaseyCarter)
  - `ranges::optional` now behaves like `std::optional` (from @CaseyCarter)
  - Extensive bug fixes with Input ranges (from @CaseyCarter)
* **0.2.5** May 16, 2017
  - `view::chunk` works on Input ranges (from @CaseyCarter)
  - `for_each_n` algorithm (from @khlebnikov)
  - Portability fixes for MinGW, clang-3.6 and -3.7, and gcc-7; and cmake 3.0
* **0.2.4** April 12, 2017
  Fix the following bug:
  - `action::stable_sort` of `vector` broken on Clang 3.8.1 since ~last Xmas (ericniebler/range-v3#632).
* **0.2.3** April 4, 2017
  Fix the following bug:
  - iterators that return move-only types by value do not satisfy Readable (ericniebler/stl2#399).
* **0.2.2** March 30, 2017
  New in this release:
  - `view::linear_distribute(from,to,n)` - A view of `n` elements between `from` and `to`, distributed evenly.
  - `view::indices(n)` - A view of the indices `[0,1,2...n-1]`.
  - `view::closed_indices(n)` - A view of the indices `[0,1,2...n]`.

  This release deprecates `view::ints(n)` as confusing to new users.
* **0.2.1** March 22, 2017
  New in this release:
  - `view::cartesian_product`
  - `action::reverse`
* **0.2.0** March 13, 2017
  Bring many interfaces into sync with the Ranges TS.
  - Many interfaces are simply renamed. The following table shows the old names
    and the new. (All names are in the `ranges::v3` namespace.)

    | Old Name                      | New Name                  |
    |-------------------------------|---------------------------|
    | `indirect_swap`               | `iter_swap`               |
    | `indirect_move`               | `iter_move`               |
    | `iterator_value_t`            | `value_type_t`            |
    | `iterator_reference_t`        | `reference_t`             |
    | `iterator_difference_t`       | `difference_type_t`       |
    | `iterator_size_t`             | `size_type_t`             |
    | `iterator_rvalue_reference_t` | `rvalue_reference_t`      |
    | `iterator_common_reference_t` | `iter_common_reference_t` |
    | `range_value_t`               | `range_value_type_t`      |
    | `range_difference_t`          | `range_difference_type_t` |
    | `range_size_t`                | `range_size_type_t`       |
    | `range_iterator_t`            | `iterator_t`              |
    | `range_sentinel_t`            | `sentinel_t`              |
  - `common_iterator` now requires that its two types (`Iterator` and `Sentinel`)
    are different. Use `common_iterator_t<I, S>` to get the old behavior (i.e., if the two types are the same, it is an alias for `I`; otherwise, it is
    `common_iterator<I, S>`).
  - The following iterator adaptors now work with iterators that return proxies
    from their postfix increment operator (i.e., `operator++(int)`):
    * `common_iterator`
    * `counted_iterator`
  - The following customization points are now implemented per the Ranges TS
    spec and will no longer find the associated unconstrained overload in
    namespace `std::`:
    * `ranges::begin`
    * `ranges::end`
    * `ranges::size`
    * `ranges::swap`
    * `ranges::iter_swap`

    (In practice, this has very little effect but it may effect overloading in
    rare situations.)
  - `ranges::is_swappable` now only takes one template parameter. The new
    `ranges::is_swappable_with<T, U>` tests whether `T` and `U` are swappable.
    `ranges::is_swappable<T>` is equivalent to `ranges::is_swappable_with<T &, T &>`.
  - The following object concepts have changed to conform with the Ranges TS
    specification, and approved changes (see [P0547](http://wg21.link/p0547)):
    * `Destructible`
    * `Constructible`
    * `DefaultConstructible`
    * `MoveConstructible`
    * `MoveConstructible`
    * `Movable`
    * `Assignable`
  - The `View` concept is no longer satisfied by reference types.
  - The syntax for defining a concept has changed slightly. See [utility/iterator_concepts.hpp](https://github.com/ericniebler/range-v3/blob/master/include/range/v3/utility/iterator_concepts.hpp) for examples.
* **0.1.1**
  Small tweak to `Writable` concept to fix #537.
* **0.1.0**
  March 8, 2017, Begin semantic versioning

Say Thanks!
-----------

I do this work because I love it and because I love C++ and want it to be as excellent as I know it can be. If you like my work and are looking for a way to say thank you, you can leave a supportive comment on [my blog](http://ericniebler.com). Or you could leave me some kudos on my Open Hub range-v3 contribution page. Just click the **Give Kudos** button [here](https://www.openhub.net/p/range-v3/contributors/3053743222308608).
