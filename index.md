---
pagetitle: Ranges v3
title: Ranges v3
...

Range library for C++11/14/17. This code is the basis of [a formal proposal](https://ericniebler.github.io/std/wg21/D4128.html) to add range support to the C++ standard library.

**Development Status:**

This code is fairly stable, well-tested, and suitable for casual use, although currently lacking documentation. No promise is made about support or long-term stability. This code *will* evolve without regard to backwards compatibility.

**Build status (on Travis-CI):**

[![Build Status](https://travis-ci.org/ericniebler/range-v3.svg?branch=master)](https://travis-ci.org/ericniebler/range-v3)

Installation
============

This library is header-only. You can get the source code from the [range-v3 repository](https://github.com/ericniebler/range-v3) on github. To compile with Range-v3, you can either #include the entire library:

````c++
#include <range/v3/all.hpp>
````

Or you can #include only the core, and then the individual headers you want:

````c++
#include <range/v3/core.hpp>
#include <range/v3/....
````

License:
============

Most of the source code in this project are mine, and those are under the Boost Software License. Parts are taken from Alex Stepanov's Elements of Programming, Howard Hinnant's libc++, and from the SGI STL. Please see the attached LICENSE file and the CREDITS file for the licensing and acknowledgements.

Supported Compilers
============

The code is known to work on the following compilers:

- clang 3.4.0
- GCC 4.9.0

Documentation
============

Please see the [Quick Start](/range-v3/01_quick_start.html) for some ideas of the things you can do with the Range v3 library.
