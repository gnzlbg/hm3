#pragma once
/// \file
///
/// Utilities for always handling errors when calling MPI
#include <cstdlib>
#include <functional>
#include <hm3/config/likely.hpp>
#include <hm3/ext/mpi.hpp>
#include <hm3/utility/at.hpp>
#include <hm3/utility/mpi/config.hpp>
#include <string>
#include <tuple>

namespace hm3::mpi {

/// Handles MPI errors by printing the error code, the position of the error,
/// and terminating the program:
template <typename At>
inline bool handle_error(int error_code, At&& at,
                         bool called_by_mpi_abort = false) {
  if (HM3_LIKELY(error_code == MPI_SUCCESS)) { return false; }

  if (error_code == MPI_ERR_IN_STATUS) { return true; }

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
  return false;
}

inline void handle_status(MPI_Status const&) {
  // TODO: handle MPI_ERR_IN_STATUS
}

/// Abort MPI program
inline void abort(int error_code) {
  handle_error(MPI_Abort(MPI_COMM_WORLD, error_code), HM3_AT_, true);
};

/// Calls MPI function f
template <typename F, typename At>
bool call(F&& f, At&& at) {
  return handle_error(f(), at);
}

template <typename F, typename... Args, typename At>
constexpr void call_s(At&& at, F&& f, Args&&... args) {
  MPI_Status status;
  auto error_in_status = handle_error(
   std::apply(std::forward<F>(f),
              std::make_tuple(std::forward<Args>(args)..., &status)),
   std::forward<At>(at));
  if (error_in_status) { handle_status(status); }
}

}  // namespace hm3::mpi

#define HM3_MPI_CALL(F) ::hm3::mpi::call([&]() { return F; }, HM3_AT_)
