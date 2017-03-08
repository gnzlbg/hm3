#pragma once
/// \file
///
/// MPI Environment
#include <hm3/utility/assert.hpp>
#include <hm3/utility/mpi/call.hpp>
#include <hm3/utility/mpi/comm.hpp>
#include <hm3/utility/mpi/config.hpp>
#include <hm3/utility/mpi/functions.hpp>

namespace hm3::mpi {

/// MPI Environment
struct env {
  // non-copyable
  env(env const&) = delete;
  env& operator=(env const&) = delete;

  // moveable
  env(env&&)   = default;
  env& operator=(env&&) = default;

  env(int argc, char* argv[]) {
    HM3_ASSERT(!initialized(), "re-initializing mpi multiple times!");
    initialize_mpi(&argc, &argv);
    HM3_ASSERT(initialized(), "MPI initialization failed");
  }

  env() {
    HM3_ASSERT(!initialized(), "re-initializing mpi multiple times!");
    initialize_mpi(nullptr, nullptr);
    HM3_ASSERT(initialized(), "MPI initialization failed");
  }

  ~env() {
    HM3_ASSERT(initialized(), "MPI should have been initialized!");
    HM3_MPI_CALL(MPI_Finalize());
    HM3_ASSERT(finalized(), "MPI should have been finalized!");
  }

  static comm world() noexcept { return comm{MPI_COMM_WORLD}; }

 private:
  void initialize_mpi(int* argc, char*** argv) {
    if (initialized()) { return; }
    HM3_ASSERT(!initialized(), "MPI has already been initialized!");

    int thread_support;
    HM3_MPI_CALL(
     MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &thread_support));

    if (thread_support != MPI_THREAD_MULTIPLE) {
      ascii_fmt::err("[MPI-ERROR] Thread support is: {}\n",
                     stringify_thread_support(thread_support));
    }
  }
};

}  // namespace hm3::mpi
