---
---

# <a id="STARTED"></a> Getting started

## Dependencies

HM3 build system requires [cmake >= 3.0](http://www.cmake.org/) and
[python >= 2.7](https//www.python.org). To generate the documentation
[doxygen](http://www.stack.nl/~dimitri/doxygen/) is required.

For compiling HM3, a standard compliant C++14 compiler with support for a few
language extensions (OpenMP, pragma once, zero-variadic-macro-arguments) is
required. Only [clang](http://clang.llvm.org/) tip-of-trunk is currently tested.
HM3 depends on the following external libraries:

- C++ standard library,
- MPI, and
- Boost (header-only installation).

For compiling the ParaView plugins an installation of ParaView from source is
required.

## Build options

The build options available are specified in
['cmake/hm3_options.cmake'](https://github.com/gnzlbg/hm3/blob/master/cmake/hm3_options.cmake).

## Available targets

Binary targets:

- `make check`: Builds and runs all tests.
- `make vis`: Builds the ParaView plugins.

Style targets:

- `make format`: Reformats all source code files (checks the formatting).
- `make check-format`: Checks the formatting of all source code files (returns a diff).
- `make check-tidy`: Checks the styleguide (returns a diff)

Documentation and website target:

- `make doc`: Builds the Doxygen documentation.
- `make site`: Builds the website.
- `make gh-pages.push`: Builds the website and uploads it to the `gh-pages`
  branch.
