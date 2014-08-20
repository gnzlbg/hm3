#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Program termination routines
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <hom3/core/at.hpp>
#include <hom3/core/types.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {

/// Terminates the program in case of error
///
/// \warning this function never returns
[[ noreturn, noinline, cold ]] void terminate(const String error_string,
                                              const String at) {
  std::cerr << "FATAL ERROR \"" << error_string << "\" (" << at << ")."
            << std::endl;
  exit(EXIT_FAILURE);
}

}  // namespace hom3

/// Terminates the program
#define TERMINATE(error_string) hom3::terminate(error_string, AT_)
////////////////////////////////////////////////////////////////////////////////
