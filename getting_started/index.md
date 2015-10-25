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

- C++ standard library: only [libc++](http://libcxx.llvm.org/) tip-of-trunk is tested,
- MPI: only [mpich](http://www.mpich.org/) is tested, and
- Boost (header-only): only latest [boost](http://www.boost.org/) is tested.

For compiling the ParaView plugins the compiled sources of ParaView are needed.

## How to build

Run `./aux/configure.py -h` to get a description of the different build modes,
build options, and targets available. In a nutshell:

```bash
make tests # Builds and runs all tests.
make vis   # Builds the visualization tools.
make fmt   # Reformats all source code files.
make cov   # Builds, run tests with coverage analysis, and generates report.
make site  # Builds the website.
```

## FAQ

### Why clang?

Is the only modern compiler that works find in both on Cray and IBM
supercomputers and has OpenMP support.

### Which Boost libraries are required?

Only the following header-only libraries are used:

- Boost.Container (for vector, static vector, and flat set/maps).
- Boost.DynamicBitset
- Boost.Units

These should be fetched automatically from github in the future. 

### Why MPICH?

- All relevant proprietary MPI implementations are MPICH derivatives (Cray, IBM,
Intel, and MVAPICH, and at the end of the day, one will use one of them).

- MPICH coverage of the current MPI standard is larger, and
recent MPI standards are implemented there faster.

- MPICH has good (and bug-free) support for `MPI_THREAD_MULTIPLE`.

- MPICH has static checking of MPI data-types when used in combination with
  clang.

Note: OpenMPI is not tested but it should work just fine.
