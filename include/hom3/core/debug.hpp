#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Implements helpful macros for happy debugging (DBG,DBGV).
///
/// \warning This file defines the implementation. The interface
/// macros (DBG,DBGV) are defined in debug.hpp because they need
/// to be included (and redefined) at every file.
///
/// If ENABLE_DEBUG_ == 1 in a given file, then:
/// - DBG(args...) prints args values (defined in dbg.hpp)
/// - DBGV(args...) prints args names and args values (defined in dbg.hpp)
/// The following versions are independent of ENABLE_DEBUG_
/// - DBG_ON / DBG_OFF - same as DBG
/// - DBGV_ON / DBGV_OFF - same as DBGV
/// \warning All macros macros are disabled if NDEBUG is defined (but
/// they are still verified by the compiler to avoid programming errors
/// and unused variable warnings).
///
/// Note: All macros are based on the DOUT macro, which calls the
/// debug::dout function.
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <iostream>
#include <boost/preprocessor.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {

/// Debugging utilities
namespace debug {

/// If on is true, stream arguments to std::cerr, otherwise does nothing.
///
/// \todo replace std::cerr with something better
///@{
/// dout N-recursion
template <class H>
void dout(const bool on, H&& msg) {  ///< Base case (N Recursion)
  if (on) {  ///< If on is false at compile time dout is removed.
    std::cerr << msg << " ";
  }
}
/// dout (N-1)-recursion
template <class H, class T> void dout(const bool on, H&& head, T&& tail) {
  if (on) {
    dout(on, std::forward<H>(head));
    dout(on, std::forward<T>(tail));
    std::cerr << "\n";
  }
}
/// dout: 0-recursion
template <class H, class... T> void dout(const bool on, H&& head, T&&... tail) {
  if (on) {
    dout(on, std::forward<H>(head));
    dout(on, std::forward<T>(tail)...);
  }
}
///@}

////////////////////////////////////////////////////////////////////////////////
}  // namespace debug
}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
/// DOUT forwards the call to debug::dout if NDEBUG is not defined
#define DOUT(...)                                 \
  do {                                            \
    if (true) { hom3::debug::dout(__VA_ARGS__); } \
  } while (false)
#else
/// Does nothing if NDEBUG is not defined
#define DOUT(...)                                  \
  do {                                             \
    if (false) { hom3::debug::dout(__VA_ARGS__); } \
  } while (false)
#endif

#ifndef NDEBUG
/// Like DOUT if condition is true, otherwise does nothing
#define DOUT_IF(condition, ...)                          \
  do {                                                   \
    if (true) {                                          \
      if (condition) { hom3::debug::dout(__VA_ARGS__); } \
    }                                                    \
  } while (false)
#else
/// Does nothing if NDEBUG is not defined
#define DOUT_IF(condition, ...)                          \
  do {                                                   \
    if (false) {                                         \
      if (condition) { hom3::debug::dout(__VA_ARGS__); } \
    }                                                    \
  } while (false)
#endif

/// Specifies the output format for DBGV
#define DBG_FORMAT(x) "|", #x ":", (x)

/// "Closure" for Boost's PP for_each
#define MEMBER(r, data, i, elem) BOOST_PP_COMMA_IF(i) DBG_FORMAT(elem)

/// Given a list of variables enclosed in parentheses, it
/// forwards to DOUT a sequence of variable names and variable values
/// using the format specified in DBG_FORMAT
#define DOUTV(on, members) DOUT(on, BOOST_PP_SEQ_FOR_EACH_I(MEMBER, , members))

#define DOUTV_IF(condition, on, members) \
  DOUT_IF(condition, on, BOOST_PP_SEQ_FOR_EACH_I(MEMBER, , members))

/// Debug macros independent of ENABLE_DEBUG_
///@{
#define DBG_ON(...) \
  do { DOUT(1, "#", __VA_ARGS__); } while (false)
#define DBG_OFF(...) \
  do { DOUT(0, "#", __VA_ARGS__); } while (false)
#define DBG_IF_ON(pred, ...) \
  do { DOUT_IF(pred, 1, "#", __VA_ARGS__); } while (false)
#define DBG_IF_OFF(pred, ...) \
  do { DOUT_IF(pred, 0, "#", __VA_ARGS__); } while (false)
#define DBGV_ON(...) \
  do { DOUTV(1, __VA_ARGS__); } while (false)
#define DBGV_OFF(...) \
  do { DOUTV(0, __VA_ARGS__); } while (false)
#define DBGV_IF_ON(pred, ...) \
  do { DOUTV_IF(pred, 1, __VA_ARGS__); } while (false)
#define DBGV_IF_OFF(pred, ...) \
  do { DOUTV_IF(pred, 0, __VA_ARGS__); } while (false)
///@}
///@}

#ifndef NDEBUG
#define DBG_EXPR(A) A
#else
#define DBG_EXPR(A)
#endif

#ifndef NDEBUG
/// Executes in debug mode
#define DEBUG_DO(f)    \
  do {                 \
    if (true) { f(); } \
  } while (false)
#else
/// Does nothing if NDEBUG is not defined
#define DEBUG_DO(f)     \
  do {                  \
    if (false) { f(); } \
  } while (false)
#endif

////////////////////////////////////////////////////////////////////////////////
