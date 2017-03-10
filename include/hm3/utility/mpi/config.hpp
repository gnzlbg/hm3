#pragma once
/// \file
///
///
#include <hm3/ext/mpi.hpp>

namespace hm3::mpi {

constexpr const char* stringify_thread_support(int thread_support) {
  switch (thread_support) {
    case MPI_THREAD_MULTIPLE: {
      return "MPI_THREAD_MULTIPLE";
    }
    case MPI_THREAD_FUNNELED: {
      return "MPI_THREAD_MULTIPLE";
    }
    case MPI_THREAD_SERIALIZED: {
      return "MPI_THREAD_SERIALIZED";
    }
    case MPI_THREAD_SINGLE: {
      return "MPI_THREAD_SINGLE";
    }
    default: { return "MPI_THREAD_UNKNOWN"; }
  }
}

}  // namespace hm3::mpi
