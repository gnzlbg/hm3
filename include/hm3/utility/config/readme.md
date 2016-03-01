# HM3's utility config

Most of the utilities here can be split into:

- Debugging.
- Error handling.
- Platform detection/support (compiler, os, ...).
- Optimization/analysis hints.

## Debugging

### `assert`:

[Assertions](http://blog.regehr.org/archives/1091:

> An assertion is a Boolean expression at a specific point in a program (that
  may or may not be evaluated) which will be true unless there is a bug in the
  program.

You can use it to check pre-conditions, post-conditions, and invariants:

```c++
HM3_ASSERT(condition, message); // perform checks in debug mode
HM3_ASSERT([&]() -> bool { /* executes in debug mode */ }(), message);
```

**Note:** assertions are used in release mode to hint the compiler that some
invariants are true.

### `debug_print`:

TODO: document how to use the OUTPUT macro

### `at`:

### `debug_trap`:

## Error Handling

### `fatal_error`: unrecoverable error

```c++
HM3_FATAL_ERROR(message);
```

Note: when `HM3_FATAL_ERROR` is called, the program will be exited without stack
unwinding, that is, destructors won't be run: file handles won't be closed,
memory won't be freed by the program (but by the operating system), buffered i/o
won't be flushed...

### `exception`

## Optimization/Analysis hints

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

### `restrict`:

### `aligned`:

**TODO**: `__builtin_prefetch`

## Platform detection/support

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
