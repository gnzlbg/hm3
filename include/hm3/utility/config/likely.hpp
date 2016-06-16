#pragma once
/// \file
///
/// LIKELY/UNLIKELY Macros (hints for branch prediction)
///
/// Use like this to indicate how likely/unlikely is the result of a boolean
/// expresion to be true:
///
/// if(HM3_UNLIKELY(1 == 0)) { ... }
/// int f(bool);
/// bool b = almost_always_true();
/// int a = f(HM3_LIKELY(b));
///
/// And like this to indicate which value should the compiler expect a variable
/// to have:
///
/// switch(HM3_EXPECT(x, 5)) {
///   default: { ... }
///   case 1: { ... }
///   case 5: { ... }
///   case 3: { ... }
/// }
///
/// To disable LIKELY/UNLIKELY: define HM3_DISABLE_LIKELY_MACROS
///
/// Note: the !! in the macros is required to handle 64bit values on 32-bit
/// systems properly

// Define this to disable likely / unlikely macros
// #define HM3_DISABLE_LIKELY_MACROS

#ifndef HM3_DISABLE_LIKELY_MACROS
#define HM3_LIKELY(boolean_expr) __builtin_expect(!!(boolean_expr), 1)
#define HM3_UNLIKELY(boolean_expr) __builtin_expect(!!(boolean_expr), 0)
#define HM3_EXPECT(expr, value) __builtin_expect((expr), (value))
#else
#define HM3_LIKELY(boolean_expr) (boolean_expr)
#define HM3_UNLIKELY(boolean_expr) (boolean_expr)
#define HM3_EXPECT(expr, value) (expr)
#endif
