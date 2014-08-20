////////////////////////////////////////////////////////////////////////////////
/// \file This file implements the debugging interface
/// macros (DBG,DBGV, and TRACE)
///
/// \warning This file is parsed multiple times _on pourpose_ (see debug.hpp).
/// That is, include guards are not allowed here!
///
/// The macro ENABLE_DBG_ needs to be redefined at every file.
/// All implicit debugging calls (i.e. DBG,DBGV) are enabled
/// with ENABLE_DBG_ for a given file in debug mode (i.e. if NDEBUG undefined).
////////////////////////////////////////////////////////////////////////////////

/// Prints value of arguments using debug::dout if ENABLE_DBG_ is set
/// to 1 _and_ NDEBUG is not defined.
///
/// Example:
/// \code
/// int arg1 = 0; double arg2 = 3.5; String arg3("hello");
/// DBG(arg1,arg2,arg3); // prints: "0 3.5 hello"
/// \endcode
#define DBG(...) \
  do { DOUT(ENABLE_DBG_, "#", __VA_ARGS__); } while (false)

/// Prints value of arguments using debug::dout if ENABLE_DBG_ is set
/// to 1, NDEBUG is not defined, _and_ the condition pred is true.
///
/// Example:
/// \code
/// int arg1 = 0; double arg2 = 3.5; String arg3("hello");
/// bool test = false;
/// DBG_IF(test,arg1,arg2,arg3); // prints nothing
/// test = true;
/// DBG_IF(test,arg1,arg2,arg3); // prints: "0 3.5 hello"
/// \endcode
#define DBG_IF(pred, ...) \
  do { DOUT_IF(pred, ENABLE_DBG_, "#", __VA_ARGS__); } while (false)

/// Prints the name _and_ value of the variables using debug::dout if
/// ENABLE_DBG_ is set to 1 _and_ NDEBUG is not defined
///
/// Example:
/// \code
/// int arg1 = 2; String arg2("hello");
/// DBGV((arg1),(arg2)) // prints: "arg1 : 2 | arg2 : "hello"
/// \endcode
#define DBGV(...) \
  do { DOUTV(ENABLE_DBG_, __VA_ARGS__); } while (false)

#define DBGV_IF(pred, ...) \
  do { DOUTV_IF(pred, ENABLE_DBG_, "#", __VA_ARGS__); } while (false)

/// Prints "In _function_name_" (used at function entrance)
#define TRACE_IN_()                                                     \
  do {                                                                  \
    DBG("# -------------------------------------------------------\n"); \
    DBG("# In f:", __PRETTY_FUNCTION__, "\n");                          \
  } while (false)

/// Prints "In _function_name_" and arguments (used at function
/// entrance with function arguments)
#define TRACE_IN(...)                                                   \
  do {                                                                  \
    DBG("# -------------------------------------------------------\n"); \
    DBG("# In f:", __PRETTY_FUNCTION__, "with args:");                  \
    DBGV(__VA_ARGS__);                                                  \
    DBG("", "\n");                                                      \
  } while (false)

/// Prints "Out _function_name_" (used at function exit)
#define TRACE_OUT()                                                     \
  do {                                                                  \
    DBG("# Out f:", __PRETTY_FUNCTION__);                               \
    DBG("# -------------------------------------------------------\n"); \
  } while (false)

// NOTE: Include guards are not allowed on this file!
