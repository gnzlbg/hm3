#pragma once
/// \file
///
/// Assume macro

/// Tells the optimizer that it can assume that the condition is true
#define HM3_ASSUME(cond)                  \
  do {                                    \
    if (!(cond)) __builtin_unreachable(); \
  } while (0)
