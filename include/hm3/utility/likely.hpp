#pragma once
/// \file
///
/// LIKELY/UNLIKELY Macros (hints for branch prediction)
///
/// Use like this:
///
/// if(HM3_UNLIKELY(1 == 0)) { ... }
/// int f(bool);
/// bool b = true;
/// int a = f(HM3_LIKELY(b));
///
/// To disable LIKELY/UNLIKELY: define HM3_DISABLE_LIKELY_MACROS

#ifndef HM3_DISABLE_LIKELY_MACROS
#define HM3_LIKELY(boolean_expr) __builtin_expect((boolean_expr), 1)
#define HM3_UNLIKELY(boolean_expr) __builtin_expect((boolean_expr), 0)
#else
#define HM3_LIKELY(boolean_expr) (boolean_expr)
#define HM3_UNLIKELY(boolean_expr) (boolean_expr)
#endif
