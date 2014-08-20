#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file LIKELY/UNLIKELY Macros
///
/// Use like this:
///
/// if(HOM3_UNLIKELY(1 == 0)) { ... }
/// int f(bool);
/// bool b = true;
/// int a = f(HOM3_LIKELY(b));
///
/// To disable LIKELY/UNLIKELY: define HOM3_DISABLE_LIKELY_MACROS
///
////////////////////////////////////////////////////////////////////////////////

/// Define this macro to disable LIKELY/UNLIKELY
/// #define HOM3_DISABLE_LIKELY_MACROS

#ifndef HOM3_DISABLE_LIKELY_MACROS
#define HOM3_LIKELY(boolean_expr) __builtin_expect((boolean_expr), 1)
#define HOM3_UNLIKELY(boolean_expr) __builtin_expect((boolean_expr), 0)
#else
#define HOM3_LIKELY(boolean_expr) (boolean_expr)
#define HOM3_UNLIKELY(boolean_expr) (boolean_expr)
#endif

////////////////////////////////////////////////////////////////////////////////
