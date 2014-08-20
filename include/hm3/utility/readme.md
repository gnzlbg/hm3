###<a id="UTILITIES_DEBUGGING"></a>Debugging

```c++
DBG("this is printed if ENABLE_DBG_ == 1 for the current file");
DBG_ON("this is always printed");
DBGV_ON((var)(f())(3)); // prints: "var : value | function() : value | 3 : 3"
```

**Deprecated**: For "long" functions (i.e. not one liners), we also have a trace
back system that is used with the `TRACE_IN`/`TRACE_OUT` macros:

```c++
int fun(int a) {
TRACE_IN((a)); // pass the parameters to trace in
/*
do stuff
int b = ...;
*/
TRACE_OUT((b)); // you have to call a TRACE_OUT; before every return statement.
return b;
}
```

By enabling debug for a module with `ENABLE_DBG_ == 1` you'll get the entry and
exit points of every function in that module as well as the input parameters and
return values of every function call.

**Note:** the above functionality is deprecated but hasn't been completely
removed yet. To generate traces the preferred method is to use the
instrumentation utilities with `-finstrument-functions` (GCC only, clang has a
bug here).

### <a id="UTILITIES_DEFENSIVE"></a>Defensive programming

[Assertions](http://blog.regehr.org/archives/1091:

> An assertion is a Boolean expression at a specific point in a program (that
  may or may not be evaluated) which will be true unless there is a bug in the
  program.

You can use it to check pre-conditions, post-conditions, and invariants:

```c++
HM3_ASSERT(condition, message); // perform checks in debug mode
HM3_ASSERT([&]() -> bool { /* executes in debug mode */ }(), message);
```

### <a id="UTILITIES_ERROR"></a>Error handling

```c++
HM3_FATAL_ERROR(message); // exits the program
```

There is also basic support for exceptions:

```c++
error::exception(message); // throws run-time error
```

For the moment there just hasn't been any exceptional behavior to handle. Most
of the time if an error occurs there is absolutely nothing that we can do about
it so hard errors (using `HM3_FATAL_ERROR`) have been the preferred way of dealing
with these situations.

### <a id="UTILITIES_MEMORY"></a>Memory

- **Heap allocation**: `std::make_unique`/`std::make_shared`.
- **Stack allocation**:
  - `std::array/std::tuple` are your friends,
  - stack allocator for containers:

    ```c++
    memory::stack::arena<double,10> stackMemory; // memory pool on the stack
    auto myVector = memory::stack::make<std::vector>(stackMemory); // stack allocated std::vector<double>
    auto myList = memory::stack::make<std::list>(stackMemory); // std::list<double> shares memory pool with vector
    ```
  - **Note**: `HM3_FATAL_ERROR` is called if you run out of memory.
  - **TODO**: document `memory::stack::vector` which is a drop in replacement
    for `std::vector` and contains its own arena.
- Heap vs stack?
  - read and understand [everything you need to know about
    memory](http://www.akkadia.org/drepper/cpumemory.pdf), and [copying and
    moving in
  c++](http://thbecker.net/articles/rvalue_references/section_01.html),
  - in particular: understand memory allocation, cache hierarchies, spatial
    and temporal locality, and memory access costs.

### <a id="UTILITIES_MATH"></a>Math
- compile-time math functions are located in `math::ct`,
- run-time math functions are located in `math::rt`,
- `math::absdiff` takes the difference of two unsigned integers,
- `math::approx(T a, T b)` computes a == b using [Google Test
  FloatingPoint](http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison/3423299#3423299),
-  understand [everything you need to know about floating point
   arithmetic](http://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html).

### <a id="UTILITIES_IO"></a>Input/Output
- located in `io` namespace,
- **Properties** (simulation, grid generation, ...):

  ```c++
  io::Properties ps; // creates property container
  io::insert_property<Num>(ps, "dx", 3.0); // inserts property of type Num and name "dx"
  auto dx = io::read<Num>ps,"dx"); // reads property of type Num and name "dx"
  io::read(ps, "dx", dx); // reads property "dx" into dx
  ```

### <a id="UTILITIES_RANGES"></a>Ranges
- for an overview see
  [Boost.Range](http://www.boost.org/doc/libs/release/libs/range/),
- range algorithms are located in the `algorithm` namespace,
- there are helpers to make:
  - `Range<T>(.,.)` of integers or iterators,
  - `RangeFilter<T>`s and filtered ranges `FRange<T>`,
  - `RangeTransformer<T>`s ,
  - type-erased ranges `AnyRange<T>`.

### <a id="UTILITIES_DISTRIBUTED"></a>Distributed computing
- See [Boost.MPI](http://www.boost.org/doc/libs/release/libs/mpi/).
- `boost::mpi::root_do(comm, F&& f)` executes computation `f` at the root of
  the communicator `comm`.

### <a id="UTILITIES_INTERFACES"></a>Interfaces
- prefer [concept-based static
  polymorphism](http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
  to run-time polymorphism.
- prefer [concept-based run-time
  polymorphism](http://www.youtube.com/watch?v=_BpMYeUFXv8) to
  inheritance-based polymorphism (flexible, cleaner,
  [faster](http://probablydance.com/2013/01/13/a-faster-implementation-of-stdfunction/)):

  ```c++
  /// Models the _CONCEPT_NAME_ concept:
  /// - fun1: Num -> bool
  /// - fun2: NumA<3> -> Num
  struct Interface {
   /// Adapts T to model the _CONCEPT_NAME_ concept:
    /// - can be specialized and overloaded for different T's
    /// - a shared_ptr with type-erasure can be used to provide reference semantics
    /// - an unique_ptr with type-erasure can be used to provide value semantics
    template<typename T> explicit Interface(const T& t)
      : fun1([&](Num x){ return t.ts_fun1(x); })  // ts_fun1 -> fun1
      , fun2([&}(NumA<3> x){ return t.ts_fun2(x); })  // ts_fun2 -> fun2
      {}
    /// The easiest way to provide reference semantics is to use generic functors
    /// and capture by reference:
    std::function<bool(Num)> fun1;
    std::function<Num(NumA<3>)> fun2;
    /// The easiest way to provide value semantics is to use a model sub-struct
    /// with type-erasure.
    };
  ```

### <a id="UTILITIES_TRAITS"></a>Traits
- useful traits are located in `traits` namespace,
- simple `EnableIf` / `DisableIf` implementation (see [Remastered
  enable_if](http://flamingdangerzone.com/cxx11/2012/06/01/almost-static-if.html)
  and [Beating overload resolution into
  submission](http://flamingdangerzone.com/cxx11/2013/03/11/overload-ranking.html)),

  ```c++
  template<SInd nd, EnableIf<traits::equal<SInd,nd,2>> = traits::dummy>
  void do_something(const NumA<Nd> x) { /* exists only for 2D input arrays */ }
  ```
  - note `EnableIf<>...` doesn't work due to [this bug in clang](http://llvm.org/bugs/show_bug.cgi?id=11723).
  - prefer tag dispatching to SFINAE.

### <a id="UTILITIES_RETURN"></a>Return type deduction
- for complicated return types one has to write something like:
  - in c++11: `noexcept(expr) -> decltype(expr) { return expr; }`,
    - _note:_ `expr` can't contain a lambda!
  - in c++1y: `noexcept(expr) -> { return expr; }`.
- The `HM3_RETURNS` macro does _the right thing_:

  ```c++
  inline auto ghost_cells() const HM3_RETURNS(cell_ids() | ghosts);
  ```

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
  - likely (`long __builtin_expect(long exp, long c)`, it is expected that
    `exp == c`):

    ```c++
    if (likely (expr)) { /*...*/ }
    if (unlikely(expr)) { /*...*/ }
    // #define likely(x)    __builtin_expect (!!(x), 1)
    // #define unlikely(x)  __builtin_expect (!!(x), 0)
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
