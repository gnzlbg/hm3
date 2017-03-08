# HM3's config

HM3's config headers abstract over architecture/compiler dependent code:

### `compiler`:

Defines a particular macro if a particular compiler is detected:

- clang: `HM3_COMPILER_CLANG`.
- gcc: `HM3_COMPILER_GCC`.

### `system`:

Defines a particular macro and a struct of a particular type (useful for tag
dispatching) if a particular system is detected:

- Posix (Unix/Linux/...): `HM3_SYS_POSIX` / `hm3::system::posix`.
- MacOS: `HM3_SYS_APPLE` (and `HM3_SYS_POSIX`) / `hm3::system::apple`.

Note: the tags form a hierarchy which allows dispatching to the most specific
function for a given platform. For example, Apple (MacOS) is a POSIX system. If
a function is only implemented for POSIX systems using the `posix` tag, it will
be called in Apple systems as well. However, if a new overload of that function
is added using the `apple` tag, this overload will now be called in Apple
systems while the `posix` overload will be called in non-Apple posix systems.


### `debug_trap`: in source break points

`HM3_DEBUG_TRAP();` traps (stops like a break point) the program at a particular execution point. The trap
can then, e.g., be catched in a debugger where the user can decide to inspect a
back trace or continue execution.

### `attributes`: function attributes

For a comprehensive list of the attributes implemented see the file. Some of
them are:

- `[[HM3_NO_RETURN]]`: function cannot return.
- `[[HM3_NO_INLINE]]`: function is never inlined.
- `[[HM3_ALWAYS_INLINE]]`: function is always inlined.
- `[[HM3_FLATTEN]]`: everything within the function is inlined (but not the
   function itself).
- `[[HM3_ALMOST_PURE]]`: function has _no side-effects_ and its result depends on
   arguments _and/or_ global state.
- `[[HM3_PURE]]` function has _no side-effects_ and its result depends
   _only_ on its arguments and not on global state (if the arguments are
   pointers the function shall not dereference them).
- `[[HM3_HOT]]` function is executed very often.
- `[[HM3_COLD]]` function is unlikely to be executed.

### `likely`: expectation of boolean expressions

`HM3_LIKELY(bool-expr)`/`HM3_UNLIKELY(bool-expr)` indicate if a boolean
expression is likely/unlikely to be true.

### `assume`:

`HM3_ASSUME(bool-expr)` tells the compiler that it can assume  the result of the
boolean expression to be true.

### `restrict`: pointer aliasing

`HM3_RESTRICT ptr/reference` indicates that the memory a given pointer/reference
point to can only be accesses through this particular pointer/reference until
the end of its lifetime.

### `aligned`: pointer alignment

`HM3_ALIGNED_PTR(ptr)`/`HM3_ALIGNED_PTR_TO(ptr, alignment)` indicate the
alignment of a given pointer. This information is required for enabling
auto-vectorization of a piece of code in most compilers.

### `prefetch`

**TODO**: implement/document `__builtin_prefetch`.

