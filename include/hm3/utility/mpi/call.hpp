#pragma once
/// \file
///
/// Utilities for always handling errors when calling MPI
#include <cstdlib>
#include <hm3/utility/config/at.hpp>
#include <hm3/utility/config/likely.hpp>
#include <hm3/utility/mpi/config.hpp>
#include <string>

namespace hm3 {
namespace mpi {

/// Handles MPI errors by printing the error code, the position of the error,
/// and terminating the program:
template <typename At>
inline void handle_error(int error_code, At&& at,
                         bool called_by_mpi_abort = false) {
  if (HM3_UNLIKELY(error_code != MPI_SUCCESS)) {
    char error_string[MPI_MAX_ERROR_STRING];
    int length;
    MPI_Error_string(error_code, error_string, &length);
    fprintf(stderr, "[MPI ERROR]: \n %s \n at: %s \n", error_string,
            at.function_name);  // TODO: file and line missing
    /// TODO: switch to hm3::fmt
    if (!called_by_mpi_abort) {
      handle_error(MPI_Abort(MPI_COMM_WORLD, error_code), at, true);
    } else {  // calling MPI_Abort failed, WTF ?
      fprintf(stderr, "[MPI ERROR - MPI_Abort FAILED]: \n %s \n at: %s \n",
              error_string, at.function_name);
      exit(EXIT_FAILURE);  // hope for the best
    }
  }
}

/// Abort MPI program
inline void abort(int error_code) {
  handle_error(MPI_Abort(MPI_COMM_WORLD, error_code), HM3_AT_, true);
};

/// Calls MPI function f
template <typename F, typename At> void call(F&& f, At&& at) {
  handle_error(f(), at);
}

}  // namespace mpi
}  // namespace hm3

#define HM3_MPI_CALL(F) ::hm3::mpi::call([&]() { return F; }, HM3_AT_)
