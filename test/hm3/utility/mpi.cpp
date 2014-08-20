/// \file
///
/// Tests MPI functionality
#include <hm3/utility/mpi.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  MPI_Finalize();
  return test::result();
}
