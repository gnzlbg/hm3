/// \file
///
/// Tests MPI functionality
#include <hm3/utility/mpi.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main(int argc, char* argv[]) {
  mpi::env env(argc, argv);
  mpi::comm comm = env.world();
  mpi::reactor reactor(comm);
  using namespace mpi::literals;

  // This test must be called with mpirun -np 2
  CHECK(comm.size() == 2);

  int value[2] = {-1, -1};

  if (rank(comm) == 0) {
    value[0] = 0;
    reactor.send(value[0], 1_r, "ZERO"_t);
    reactor.receive(value[1], 1_r, "ONE"_t);
  }
  if (rank(comm) == 1) {
    value[1] = 1;
    reactor.send(value[1], 0_r, "ONE"_t);
    reactor.receive(value[0], 0_r, "ZERO"_t);
  }

  reactor.wait_all();

  return test::result();
}
