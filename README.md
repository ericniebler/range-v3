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

- clang 3.5.2
- GCC 4.9.4 (C++14 support requires GCC 5.2; C++14 "extended constexpr" support is poor before 6.1.)
- VS2015 Update 3 "Clang with Microsoft CodeGen" (Clang/C2)

**Development Status:** This code is fairly stable, well-tested, and suitable for casual use, although currently lacking documentation. No promise is made about support or long-term stability. This code *will* evolve without regard to backwards compatibility.

**Build status**
- on Travis-CI: [![Travis Build Status](https://travis-ci.org/ericniebler/range-v3.svg?branch=master)](https://travis-ci.org/ericniebler/range-v3)
- on AppVeyor: [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/9mekkhw1wm22rum9/branch/master?svg=true)](https://ci.appveyor.com/project/CaseyCarter/range-v3)

Say Thanks!
-----------

I do this work because I love it and because I love C++ and want it to be as excellent as I know it can be. If you like my work and are looking for a way to say thank you, you can leave a supportive comment on [my blog](http://ericniebler.com). Or you could leave me some kudos on my Open Hub range-v3 contribution page. Just click the **Give Kudos** button [here](https://www.openhub.net/p/range-v3/contributors/3053743222308608).
