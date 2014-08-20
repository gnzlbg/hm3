#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file AT macro
////////////////////////////////////////////////////////////////////////////////

/// Returns a string containing the position where it is defined. That
/// is, " at function _function_name_ in file _file_name_ at line _line_number_
#define AT_                                                               \
  (String(" at function ") + __PRETTY_FUNCTION__ + " in file " + __FILE__ \
   + " at line " + std::to_string(__LINE__))

////////////////////////////////////////////////////////////////////////////////
