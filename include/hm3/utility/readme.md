# HM3's utilities

## Subdirectories overview:

- `config/`: configurable `hm3`s macros: assert, line information, system
  information, compiler detection, attributes (alignment, restrict, optimization
  attribute macros), debugging macros, exception macros, abort/terminate macros
- `mpi/`: C++ wrapper over MPI
- `matrix/`: wrapper over Eigen3

## Utilities overview:

- `any`: can store objects of any type via type-erasure
- `bit`: bit manipulation algorithms
- `bounded<T, From, To>`: bounded integer type (e.g. `[0, 10)`, asserts if the
   bounds are violated).
- `compact_optional<T>`: efficient `optional` for types that have a sentinel
   value (e.g. if an `int` cannot ever be negative, `-1` can be used to
   represent emptiness).
- `fmt`: a wrapper around the `<cppformat>` library: 
  `hm3::fmt::print("hell{d} {}", 0, "world");` is way faster than `<iostream>`.
- `heap_buffer`: `view<T, Storage>` an already existing container of type
  `Storage` (typically a `vector<char>`) as a consecutive sequence of `T`s.
- `log`: a wrapper around the `<spdlog>` logging library.
- `math`: mathematical constants and functions.
- `optional<T>`: can contain an object of type `T` or be empty
- `range`: wrapper over the `range-v3` library.
- `inline_vector<T, Capacity>`: a resizable container with fixed `Capacity` that
   stores its elements within the container object itself (without memory
   allocations, so that the content can be put on the stack or in static
   storage).
- `test`: very small unit testing framework.
- `units`: wrapper over the `Boost.Units` library
- `variant<A, B, C, ...>`: wrapper over `<experimental/variant>`. A tagged union
  (sum type) that can store objects of types `A, B, C, ...` but nothing else.
   
### <a id="UTILITIES_PERFORMANCE"></a>Performance

- `cold_do(f)` executes computation `f` but preventing its inlining.
- Know your attributes: `[[attribute]]`
  - Function attributes:
    - `[[gnu::noreturn]]` function cannot return.
    - `[[gnu::noinline]]` function is not inlined.
    - `[[gnu::always_inline]]` inlines `inline` function even with
      optimizations disabled.
    - `[[gnu::flatten]]` every call _inside_ the function is inlined (not the
      function itself).
    - `[[gnu::pure]]` function has _no side-effects_ and its result depends on
      arguments _and/or_ global state.
    - `[[gnu::const]]` function has _no side-effects_ and its result depends
      _only_ on its arguments.
    - `[[gnu::no_instrument_function]]` if instrumentation is enabled, this
      function is not instrumented.
    - `[[gnu::hot]]` function is a hot spot (executed very often).
    - `[[gnu::cold]]` function is unlikely to be executed.
  - Variable/type attributes:
    - `[[aligned (alignment)]]` minimum alignment (in bytes)
  - Other:
    - `[[clang::fallthrough]]` denote intentional fall-through between switch
      labels.

- Know your builtins:
 
    ```
  - `void* __builtin_assume_alligned(const void *exp, size_t align, ...)`
    returns `exp`, and allows the compiler to assume that the returned pointer
    is at least `align` bytes aligned.
  - `void __builtin_prefetch (const void *addr, rw, locality)` where `addr` is
    the address of the memory to prefetch, `rw = {0, 1}` indicates an expected
    read (`0`) or an expected write (`1`), and `locality = {0, 1, 2, 3}`
    expresses the temporal locality of the data, where `0` means that the data
    need not be left in the cache after the access, `3` means that the data
    should be left in all levels of cache, and `1-2` are values in between.
