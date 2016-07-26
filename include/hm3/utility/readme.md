# HM3's utilities

## Subdirectories overview:

- `config/`: configurable `hm3`s macros: assert, line information, system
  information, compiler detection, attributes (alignment, restrict, optimization
  attribute macros), debugging macros, exception macros, abort/terminate macros.
- `mpi/`: C++ wrapper over MPI.
- `matrix/`: wrapper over Eigen3.

## Utilities overview:

- `any`: can store objects of any type via type-erasure.
- `bit`: bit manipulation algorithms.
- `bounded<T, From, To>`: bounded integer type (e.g. `[0, 10)`, asserts if the
   bounds are violated).
- `cold_do(f)` executes `f` but preventing it from being inlined.
- `compact_optional<T>`: efficient `optional` for types that have a sentinel
   value (e.g. if an `int` cannot ever be negative, `-1` can be used to
   represent emptiness).
   - `optional_idx<T, Tag>` makes is a wrapper around `compact_optional` for
   indices with a sentinel value.
- `fmt`: a wrapper around the `<cppformat>` library:
  `hm3::fmt::print("hell{d} {}", 0, "world");` is way faster than `<iostream>`.
- `heap_buffer`: `view<T, Storage>` an already existing container of type
  `Storage` (typically a `vector<char>`) as a consecutive sequence of `T`s.
- `log`: a wrapper around the `<spdlog>` logging library.
- `math`: mathematical constants and functions.
- `optional<T>`: can contain an object of type `T` or be empty.
- `range`: wrapper over the `range-v3` library.
- `inline_vector<T, Capacity>`: a resizable container with fixed `Capacity` that
   stores its elements within the container object itself (without memory
   allocations, so that the content can be put on the stack or in static
   storage).
- `test`: very small unit testing framework.
- `units`: wrapper over the `Boost.Units` library (compile time checking of unit
   dimensions).
- `variant<A, B, C, ...>`: wrapper over `<experimental/variant>`. A tagged union
  (sum type) that can store objects of types `A, B, C, ...` but nothing else.
