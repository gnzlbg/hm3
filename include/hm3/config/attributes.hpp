#pragma once
/// \file
///
/// Macros for non-standard attributes.
///
/// Note: standard attributes should be used as is.
#include <hm3/config/compiler.hpp>

/// \name Disable Optimization Attributes
///
/// Disables all optimization attributes
///@{

#ifdef HM3_DISABLE_OPTIMIZATION_ATTRIBUTES
#define HM3_DISABLE_FLATTEN
#define HM3_DISABLE_ALWAYS_INLINE
#define HM3_DISABLE_HOT
#define HM3_DISABLE_COLD
#define HM3_DISABLE_NO_INLINE
// #define HM3_DISABLE_NO_RETURN // not exclusively an optimization attribute
#define HM3_DISABLE_ALMOST_PURE
#define HM3_DISABLE_PURE
#define HM3_DISABLE_KERNEL
#endif  // HM3_DISABLE_OPTIMIZATION_ATTRIBUTES

///@}  // Disable Optimization Attributes

/// \name Flatten attribute
///
/// The flatten function attribute inlines all the functions within the current
/// function.
///@{

#ifdef HM3_DISABLE_FLATTEN
#define HM3_FLATTEN
#else

#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_FLATTEN gnu::flatten
#else
#error HM3_FLATTEN is not implemented for this compiler.
#endif

#endif  // HM3_DISABLE_FLATTEN

///@}  // Flatten attribute

/// \name Always inline attribute
///
/// The always inline function attribute inlines this function at every call
/// site.
///@{

#ifdef HM3_DISABLE_ALWAYS_INLINE
#define HM3_ALWAYS_INLINE
#else

#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_ALWAYS_INLINE gnu::always_inline
#else
#error HM3_ALWAYS_INLINE is not implemented for this compiler.
#endif

#endif  // HM3_DISABLE_ALWAYS_INLINE

///@}  // Inline attribute

/// \name Hot attribute
///
/// The hot function attribute marks this function as a hot spot (i.e.
/// a function that gets called often).
///@{

#ifdef HM3_DISABLE_HOT
#define HM3_HOT
#else

#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_HOT gnu::hot
#else
#error HM3_HOT is not implemented for this compiler.
#endif

#endif  // HM3_DISABLE_HOT

///@}  // Hot attribute

/// \name Cold attribute
///
/// The cold function attribute marks this function as a cold spot (i.e. a
/// a function that is almost never called).
///
/// Useful to prevent inlining.
///
///@{

#ifdef HM3_DISABLE_COLD
#define HM3_COLD
#else

#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_COLD gnu::cold
#else
#error HM3_COLD is not implemented for this compiler.
#endif

#endif  // HM3_DISABLE_COLD

///@}  // Cold attribute

/// \name No inline attribute
///
/// The noinline function attribute prevents a function from being inlined by
/// the compiler.
///@{

#ifdef HM3_DISABLE_NO_INLINE
#define HM3_NO_INLINE
#else

#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_NO_INLINE gnu::noinline
#else
#error HM3_NO_INLINE is not implemented for this compiler.
#endif

#endif  // HM3_DISABLE_NO_INLINE

///@}  // No inline attribute

/// \name No return attribute
///
/// The no return function attribute indicates that a function does not return.
///@{

#ifdef HM3_DISABLE_NO_RETURN
#define HM3_NO_RETURN
#else

#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_NO_RETURN noreturn
#else
#error HM3_NO_RETURN is not implemented for this compiler.
#endif

#endif  // HM3_DISABLE_NO_RETURN

///@}  // No return attribute

/// \name Almost Pure attribute
///
/// The almost pure function attribute can be applied to functions that:
/// - have no side-effects (no I/O, no modification of global state, ...)
/// - whose return value only depends on:
///   - its arguments, and/or,
///   - immutable global state.
///
/// @{

#ifdef HM3_DISABLE_ALMOST_PURE
#define HM3_ALMOST_PURE
#else

#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_ALMOST_PURE gnu::pure
#else
#error HM3_ALMOST_PURE is not implemented for this compiler.
#endif

#endif  // HM3_DISABLE_ALMOST_PURE

///@}  // Almost pure attribute

/// \name Pure attribute
///
/// The pure function attribute can be applied to functions that:
/// - do not examine any value except their arguments,
/// - have no effects except the return value.
///
/// That is: no side-effects (no I/O, no modification of global state, ...), no
/// static variables, do not observer global state in any way (if the arguments
/// are pointers, it can only observe the pointer itself, not what the pointer
/// points to...), not even the `this` pointer (i.e. no non-static member
/// functions)...
///
/// \warning This attribute is really strict in what a function can/cannot do.
/// Use it wisely, and when in doubt, leave it out.
///
/// @{

#ifdef HM3_DISABLE_PURE
#define HM3_PURE
#else

#if defined(HM3_COMPILER_GCC) || defined(HM3_COMPILER_CLANG)
#define HM3_PURE gnu::const
#else
#error HM3_PURE is not implemented for this compiler.
#endif

#endif  // HM3_DISABLE_PURE

///@}  // Pure attribute

#ifdef HM3_DISABLE_KERNEL
#define HM3_KERNEL
#else
#define HM3_KERNEL [[ HM3_ALWAYS_INLINE, HM3_FLATTEN, HM3_HOT ]]
#endif
