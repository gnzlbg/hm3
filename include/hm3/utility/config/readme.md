# HM3's utility config

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

### `fatal_error`: unrecoverable error

```c++
HM3_FATAL_ERROR(message);
```

Note: when `HM3_FATAL_ERROR` is called, the program will be exited without stack
unwinding, that is, destructors won't be run: file handles won't be closed,
memory won't be freed by the program (but by the operating system), buffered i/o
won't be flushed...
    
### `debug_print`:

  TODO: document how to use the OUTPUT macro
