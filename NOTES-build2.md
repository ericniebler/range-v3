Notes for `build2` users
========================

The following examples are for `build2` beginners who just want to work with this project using `build2`, and for maintainers who want to maintain the `build2` packages of this project.

For more info, read [`build2`'s documentation](https://build2.org/doc.xhtml), in particular the [build-system's manual](https://build2.org/build2/doc/build2-build-system-manual.xhtml).

# Add `range-v3` as a dependency to your `build2` project

0. If you don't already have a `build2` project, [use `bdep new` to immediately create a new "hello world" project](https://build2.org/bdep/doc/bdep-new.xhtml).
1. Add this line in your project's `./manifest`:
    ```
    depends: range-v3 ~0.11.0
    ```
    (Of course you can use another version if it's available, see [`build2`'s documentation for the versions constraints](https://build2.org/bpkg/doc/build2-package-manager-manual.xhtml#package-version-constraint))

2. Then optionally specify how to find the package by adding in `./repositories.manifest` either:
    - to get it from `https://cppget.org` (assuming a published package exist in the `stable` repo):
        ```
        :
        role: prerequisite
        location: https://pkg.cppget.org/1/stable
        ```
    - to get it from this git repository (here we'll us the `master` branch as an example):
        ```
        :
        role: prerequisite
        location: https://github.com/ericniebler/range-v3.git#master
        ```
    Specifying the repository in `repositories.manifest` will make all developers who want to work on your project to automatically fetch that package the specified repository without other operations, so it's a good default.

    - Another option (more for advanced `build2` users) is to add one of the repositories to your configuration manually using [`bpkg add [...]`](https://build2.org/bpkg/doc/bpkg-rep-add.xhtml).

3. To make one of your target depends on the library, in the `buildfile` defining that target:
    ```
    import rangev3 = range-v3%lib{range-v3}

    lib{mylibrary} : $rangev3
    ```
    The first line `import`s the library target `lib{range-v3}` from the package `range-v3`.
    The second line specify that `lib{range-v3}` is a prerequisite of `lib{mylibrary}`.

At this point, the C++ files defined as prerequisite of `lib{mylibrary}` can use:

``` c++
#include <range/v3/all.hpp> // For example, get everything.
```

You can now try to build your project (using `b` or  `bdep update`), which will automatically first invoke `bdep sync -f` to update your dependencies as specified.

(There are variations to these instructions. See [`build2`'s build-system manual for details](https://build2.org/build2/doc/build2-build-system-manual.xhtml).)


# Start development on `range-v3` locally with `build2`:

1. Clone this repository (at a branch providing `build2` files), for example:
    ```
    git clone https://github.com/ericniebler/range-v3.git#master range-v3
    cd range-v3
    ```
2. Initialize the project in a configuration:

    This means create or reuse a configuration directory and add the project in it to be configured. As a shortcut, it can be done in one command: [`bdep init ...`](https://build2.org/bdep/doc/bdep-init.xhtml).

    As an example, let's initialize the project to build with clang in C++17:
    ```
    bdep init -C ../build-myconfig cc config.cxx=clang++ config.c=clang config.cxx.std=17
    ```
    Note that you can initialize one project in more than one configurations.

3. You can now start to work on the project:
     - `b` or `b update` to build, `b clean` to clean the generated files;
     - `b test` to build the test and run them all;
     - `b install config.install.root=../path/to/install/dir` to install the built project somewhere;
     - `b clean update test install config.install.root=../path/to/install/dir` to do these operations sequentially;

    See [`build2`'s documentation for more](https://build2.org/doc.xhtml).

4. You can initialize the project in different configurations (clang in debug, msvc in release, etc.), then use `bdep update -a`, `bdep test -a`, etc. to build and test on all these configurations. See [`bdep` documentation](https://build2.org/bdep/doc/bdep.xhtml) or [the `build2` toolchain introduction](https://build2.org/build2-toolchain/doc/build2-toolchain-intro.xhtml).


# Just build and install `range-v3` (no development) using `build2`:

You can just build, test, install the library without setting up a project using build2 (which is useful for implementing CI, for example, or when you just wanting to install a specific version in the system).

0. Create (or reuse) a `build2` configuration that will be used to build, test and install the package.
    As an example, let's make a configuration building with clang in C++17:
    ```
    mkdir build-clang
    cd build-clang
    bpkg create cc config.cxx=clang++ config.c=clang config.cxx.std=17 [...]
    ```

1. Then depending on how you prefer to acquire the package, either:
    - Fetch and build range-v3, for example the version currently in branch `master` :
      ```
      bpkg build https://github.com/ericniebler/range-v3.git#master
      ```
    - Or add `https://cppget.org` repository, fetch and build the package - for example if the package is available in the `stable` repo:
      ```
      bpkg add bpkg add https://pkg.cppget.org/1/stable
      bpkg fetch
      bpkg build range-v3
      ```

2. At this point, you might want to check that the tests run, at least for that configuration:
    ```
    bpkg test range-v3
    ```

3. Install that version of range-v3 in `../path/to/install/dir`
    ```
    bpkg install range-v3 config.install.root=../path/to/install/dir
    ```

# Send a (modified) version of `range-v3` to `build2`'s community CI

`build2` provide a [CI server](https://ci.cppget.org/) open for submissions from any open-source project.

When working with a unpublished version of this `range-v3` repository, you can send this specific revision for CI by:

0. Make the current revision accessible publicly so that the server can retrieve it. For example, push that specific commit somewhere in a github.com repository which is publicly accessible.

1. In the directory of the project:
    ```
    bdep ci
    ```
    And just follow the instructions.

See [`bdep ci`'s manual](https://build2.org/bdep/doc/bdep-ci.xhtml) for more details.

# Publish a new version of the `range-v3` package for `build2` on `cppget.org`

This is for the owners/maintainers of this project.

Note: you must have write-acccess to the repository to be allowed to complete the publication submission.

0. Before submitting the package, make sure the information of the package are up to date:
    - Update the `version` field in `./manifest` (and commitit);
    - Make sure you are at the right git revision and make sure it's available publicly online.
    - Make sure there is a version tag pointing at the commit to publish.

    Note: these could be done automatically using [`bdep release`](https://build2.org/bdep/doc/bdep-release.xhtml) but this project is not maintained using `build2` so it's probably better to update the manifest manually.

1. Submit the package for publication (to https://cppget.org by default):
    ```
    bdep publish
    ```
    And follow the instructions.

See [`bdep publish`'s manual](https://build2.org/bdep/doc/bdep-publish.xhtml) for details.
