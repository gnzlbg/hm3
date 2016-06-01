#include "slab_convergence.hpp"

int main(int argc, char* argv[]) {
  using namespace hm3;
  /// Initialize MPI
  mpi::env env(argc, argv);

  {  // fully structured
    auto tint     = solver::fv::euler_forward{};
    auto num_flux = solver::fv::heat::flux::three_point;
    num_t order   = 1.9;
    convergence_slab_analytical<2>(env, tint, num_flux, order, false);
  }
  /*
  {  // slightly unstructured
    auto tint     = solver::fv::runge_kutta_5{};
    auto num_flux = solver::fv::heat::flux::three_point;
    num_t order   = 1.9;
    convergence_slab_analytical<2>(env, tint, num_flux, order, true);
  }

  {  // fully structured
    auto tint     = solver::fv::euler_forward{};
    auto num_flux = solver::fv::heat::flux::five_point;
    num_t order   = 1.9;
    convergence_slab_analytical<2>(env, tint, num_flux, order, false);
  }
  */
  return test::result();
}
