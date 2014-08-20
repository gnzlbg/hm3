////////////////////////////////////////////////////////////////////////////////
/// \file \brief Tests MPI functionality
#include <hom3/core/mpi.hpp>
#include <hom3/core/test.hpp>
////////////////////////////////////////////////////////////////////////////////
using namespace hom3;

void initialization() {
  using namespace mpi;
  communicator world;

  if (is_root(world)) {
    String msg, out_msg = "Hello";
    requests<2> reqs = {{world.isend(1, 0, out_msg), world.irecv(1, 1, msg)}};
    wait_all(reqs);
    // std::cout << msg << "!" << std::endl;
  } else {
    String msg, out_msg = "world";
    requests<2> reqs = {{world.isend(0, 1, out_msg), world.irecv(0, 0, msg)}};
    wait_all(reqs);
    // std::cout << msg << ", ";
  }
}

int main(int argc, char* argv[]) {
  mpi::environment env(argc, argv);
  initialization();
  return test::result();
}
