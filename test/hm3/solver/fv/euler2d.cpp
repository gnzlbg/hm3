#include <hm3/geometry/sd.hpp>
#include <hm3/grid/generation/uniform.hpp>
#include <hm3/solver/fv/euler.hpp>
#include <hm3/solver/fv/euler/initial_condition/shock_tube.hpp>
#include <hm3/solver/fv/euler/vtk.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/vtk.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/utility/test.hpp>

template <typename T> struct dump;

using namespace hm3;
using namespace solver;

template <typename F> struct BoundaryConditions {
  F f;
  BoundaryConditions(F f_) : f(f_) {}
  template <typename Physics> constexpr void apply(fv::state<Physics>& s) {
    for (auto&& b : s.blocks()) {
      b.for_each_halo([&](auto&& c) { b.variables(c) = f(b, c); });
    }
  }
};

template <typename F> auto make_bcs(F&& f) { return BoundaryConditions<F>(f); }

struct NeumannBoundaryConditions {
  template <typename Physics> constexpr void apply(fv::state<Physics>& s) {
    for (auto&& b : s.blocks()) {
      b.for_each_halo([&](auto&& h_c) {
        auto i_c = b.closest_internal_cell(h_c);
        b.variables(h_c) = b.variables(i_c);
      });
    }
  }
};

template <uint_t Nd> struct test_state {
  using var_v = num_a<Nd + 2>;
  var_v cv;
  var_v pv;
  std::array<var_v, Nd> f;
  num_t a;
  num_t s_max;
};

template <uint_t Nd>
void check_variables(fv::euler::physics<Nd> p, test_state<Nd> a) {
  auto cv = p.cv();
  auto pv = p.pv();

  using var_v = typename test_state<Nd>::var_v;

  {  // check cv
    CHECK(cv.rho(a.cv) == a.cv(p.rho()));
    CHECK(cv.rho(a.cv) == a.pv(p.rho()));
    num_t umag2 = 0.;

    for (auto d : p.dimensions()) {
      CHECK(cv.rho_u(a.cv, d) == a.cv(p.rho_u(d)));
      CHECK(cv.rho_u(a.cv)(d) == a.cv(p.rho_u(d)));
      CHECK(cv.rho_u(a.cv, d) == a.pv(p.rho()) * a.pv(p.u(d)));
      CHECK(cv.rho_u(a.cv)(d) == a.pv(p.rho()) * a.pv(p.u(d)));
      CHECK(cv.u(a.cv, d) == a.cv(p.rho_u(d)) / a.cv(p.rho()));
      CHECK(cv.u(a.cv)(d) == a.cv(p.rho_u(d)) / a.cv(p.rho()));
      CHECK(cv.u(a.cv, d) == a.pv(p.u(d)));
      CHECK(cv.u(a.cv)(d) == a.pv(p.u(d)));
      umag2 += std::pow(cv.u(a.cv)(d), 2);
    }
    num_t umag = std::sqrt(umag2);
    CHECK(cv.u_mag2(a.cv) == umag2);
    CHECK(cv.u_mag(a.cv) == umag);

    CHECK(cv.rho_E(a.cv) == a.cv(p.rho_E()));
    CHECK(cv.p(a.cv) == a.pv(p.p()));

    CHECK(cv.speed_of_sound(a.cv) == a.a);

    var_v cv_c = a.cv;
    cv.to_pv_ip(cv_c);

    RANGES_FOR (auto&& v, p.variables()) {
      CHECK(cv_c(v) == a.pv(v));
      CHECK(cv.to_pv(a.cv)(v) == a.pv(v));
    }

    for (auto d : p.dimensions()) {
      var_v cv_f;
      cv.flux_ip(a.cv, cv_f, d);
      RANGES_FOR (auto&& v, p.variables()) {
        CHECK(cv_f(v) == a.f[d](v));
        CHECK(cv.flux(a.cv, d)(v) == a.f[d](v));
      }
    }
  }  // cv

  CHECK(cv.speed_of_sound(a.cv) == pv.speed_of_sound(a.pv));

  {  // check pv
    CHECK(pv.rho(a.pv) == a.pv(p.rho()));
    CHECK(pv.rho(a.pv) == a.pv(p.rho()));
    num_t umag2 = 0.;

    for (auto d : p.dimensions()) {
      CHECK(pv.rho_u(a.pv, d) == a.cv(p.rho_u(d)));
      CHECK(pv.rho_u(a.pv)(d) == a.cv(p.rho_u(d)));
      CHECK(pv.rho_u(a.pv, d) == a.pv(p.rho()) * a.pv(p.u(d)));
      CHECK(pv.rho_u(a.pv)(d) == a.pv(p.rho()) * a.pv(p.u(d)));
      CHECK(pv.u(a.pv, d) == a.cv(p.rho_u(d)) / a.cv(p.rho()));
      CHECK(pv.u(a.pv)(d) == a.cv(p.rho_u(d)) / a.cv(p.rho()));
      CHECK(pv.u(a.pv, d) == a.pv(p.u(d)));
      CHECK(pv.u(a.pv)(d) == a.pv(p.u(d)));
      umag2 += std::pow(pv.u(a.pv)(d), 2);
    }
    num_t umag = std::sqrt(umag2);
    CHECK(pv.u_mag2(a.pv) == umag2);
    CHECK(pv.u_mag(a.pv) == umag);

    CHECK(pv.rho_E(a.pv) == a.cv(p.rho_E()));
    CHECK(pv.p(a.pv) == a.pv(p.p()));

    CHECK(pv.speed_of_sound(a.pv) == a.a);

    var_v pv_c = a.pv;
    pv.to_cv_ip(pv_c);

    RANGES_FOR (auto&& v, p.variables()) {
      CHECK(pv_c(v) == a.cv(v));
      CHECK(pv.to_cv(a.pv)(v) == a.cv(v));
    }

    for (auto d : p.dimensions()) {
      var_v pv_f;
      pv.flux_ip(a.pv, pv_f, d);
      RANGES_FOR (auto&& v, p.variables()) {
        CHECK(pv_f(v) == a.f[d](v));
        CHECK(pv.flux(a.pv, d)(v) == a.f[d](v));
      }
    }
  }  // pv
}

template <uint_t Nd>
void check_time_step(fv::euler::physics<Nd> p, test_state<Nd> a) {
  auto cv = p.cv();
  auto pv = p.pv();

  {  // cv:
    auto one_over_s_max = fv::euler::time_step(a.cv, cv, 1.0, 1.0);
    auto s_max          = 1. / one_over_s_max;
    CHECK(a.s_max == s_max);
  }
  {  // pv:
    auto one_over_s_max = fv::euler::time_step(a.pv, pv, 1.0, 1.0);
    auto s_max          = 1. / one_over_s_max;
    CHECK(a.s_max == s_max);
  }
}

template <uint_t Nd, typename var_v = typename test_state<Nd>::var_v>
void check_lax_friedrichs_flux(fv::euler::physics<Nd> p, test_state<Nd> l,
                               test_state<Nd> r
                               //, std::array<var_v, Nd> result
                               ) {
  auto cv = p.cv();
  auto pv = p.pv();

  struct {
    num_t dx   = 1.0;
    num_t dt   = 1.0;
    num_t area = 1.0;
  } s;

  RANGES_FOR (auto&& d, p.dimensions()) {
    auto f_cv1 = fv::euler::flux::local_lax_friedrichs(cv, l.cv, r.cv, d, s);
    auto f_cv2 = fv::euler::flux::local_lax_friedrichs(cv, r.cv, l.cv, d, s);

    RANGES_FOR (auto&& v, p.variables()) { CHECK(f_cv1(v) == -f_cv2(v)); }
  }
}

void check_euler() {
  num_t gamma = 1.4;
  {  // 1D
    static constexpr uint_t nd = 1;

    using var_v = num_a<3>;
    auto p      = fv::euler::physics<nd>{gamma};
    // p = (gamma - 1) * (rho_e - rho_u^2 / (2 rho)) =
    // a = sqrt(gamma * p / rho) = sqrt(1.4 * 0.4 / 1.0)

    test_state<nd> s1;
    s1.cv    = var_v{0.0, 1.0, 1.0};
    s1.pv    = var_v{0.0, 1.0, 0.4};
    s1.f[0]  = var_v{0.4, 0.0, 0.0};
    s1.a     = std::sqrt(1.4 * 0.4 / 1.0);
    s1.s_max = s1.a;
    check_variables(p, s1);
    check_time_step(p, s1);

    test_state<nd> s2;
    s2.cv    = var_v{6.0, 3.0, 8.0};
    s2.pv    = var_v{2.0, 3.0, 0.8};
    s2.f[0]  = var_v{12.8, 6.0, 17.6};
    s2.a     = std::sqrt(1.4 * 0.8 / 3.0);
    s2.s_max = s2.a + s2.pv(0);
    check_variables(p, s2);
    check_time_step(p, s2);
  }  // 1D

  {  // 2D
    static constexpr uint_t nd = 2;

    using var_v = num_a<4>;
    auto p      = fv::euler::physics<nd>{gamma};
    // p = (gamma - 1) * (rho_e - rho_u^2 / (2 rho)) =
    // a = sqrt(gamma * p / rho) = sqrt(1.4 * 0.4 / 1.0)

    {
      test_state<nd> s1;
      s1.cv    = var_v{0.0, 0.0, 1.0, 1.0};
      s1.pv    = var_v{0.0, 0.0, 1.0, 0.4};
      s1.f[0]  = var_v{0.4, 0.0, 0.0, 0.0};
      s1.f[1]  = var_v{0.0, 0.4, 0.0, 0.0};
      s1.a     = std::sqrt(1.4 * 0.4 / 1.0);
      s1.s_max = s1.a;
      check_variables(p, s1);
      check_time_step(p, s1);
    }

    {
      test_state<nd> s1;
      s1.cv    = var_v{6.0, -3.0, 3.0, 8.0};
      s1.pv    = var_v{2.0, -1.0, 3.0, 0.2};
      s1.f[0]  = var_v{12.2, -6.0, 6.0, 16.4};
      s1.f[1]  = var_v{-6.0, 3.2, -3.0, -8.2};
      s1.a     = std::sqrt(1.4 * 0.2 / 3.0);
      s1.s_max = s1.a + s1.pv(0);
      check_variables(p, s1);
      check_time_step(p, s1);
    }
  }  // 2D

  {  /// TODO: 3D
  }
}
/*
void explosion_test(mpi::env& env) {
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("fv_euler_2d", comm);
  io::session s(io::create, "fv_euler_2d", comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 1;
  auto min_grid_level       = 0_u;
  auto max_grid_level       = min_grid_level + 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::square<nd>::unit();

  // Create the grid
  grid::mhc<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  grid::generation::uniform(g, min_grid_level);

  auto solver_block_capacity
   = grid::grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};

  using p        = fv::euler::physics<nd>;
  using solver_t = fv::state<p>;

  auto&& num_flux = [](auto&&... vars) {
    return fv::euler::flux::local_lax_friedrichs(
     std::forward<decltype(vars)>(vars)...);
  };
  auto compute_time_step = [](auto&&... vars) {
    return fv::euler::time_step(std::forward<decltype(vars)>(vars)...);
  };

  p physics{1.4};

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};
  // g.refine(tree::node_idx{3});

  /// Initial advection solver grid:
  solver::initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  //   auto ic = [&](auto&& x, auto&& vars) {
  //     geometry::point<nd> xc = geometry::point<nd>::constant(0.5);
  //     auto r                 = 0.2;
  //     auto pv                = physics.pv();
  //     if (geometry::sd::sphere(x, xc, r) < 0) {
  //       // inside:
  //       pv.rho(vars) = 1.0;
  //       pv.p(vars) = 1.0;
  //     } else {
  //       // outside:
  //       pv.rho(vars) = 0.125;
  //       pv.p(vars) = 0.1;
  //     }
  //     pv.u(vars).fill(0.0);
  //     pv.to_cv_ip(vars);
  //   };

  //   // Initial condition by cellfor (auto& b : as0.blocks()) {
  //   b.for_each_internal(
  //    [&](auto&& b, auto&& c) { ic(b.center(c), b.variables(c.idx)); });
  // }

  /// Boundary conditions
  auto bcs = make_bcs([&](auto&&, auto&&) {
    num_a<p::nvars()> vars;
    vars(p::rho()) = 0.125;
    vars(p::p()) = 0.1;
    physics.pv().u(vars).fill(0.0);
    physics.pv().to_cv_ip(vars);
    return vars;
  });

  num_t cfl = 1.0;
  bcs.apply(as0);
  solver::fv::vtk::serialize(as0, "result", 0, physics.cv(), 0_gn);
  for (auto timestep : view::iota(1, 1000)) {
    //   // for (auto rkstep : fv::runge_kutta_steacdps()) {
    {  // can happen in parallel
      fv::clear_rhs(as0);
      fv::clear_halos(as0);  // in debug mode
    }
    {
      //  fv::cv_to_pv(as0);
      // fv::exchange_halos(as0);
    }
    bcs.apply(as0);
    num_t dt = fv::compute_time_step(as0, cfl, compute_time_step, physics.cv());
    std::cout << "dt: " << dt << std::endl;
    // fv::compute_gradients(as0);
    //   // fv::correct_boundary_gradients(as0, bcs);
    //   // fv::limit_gradients(as0, limiter_t);
    // fv::compute_internal_fluxes(as0, num_flux, dt, physics.cv());
    // fv::compute_boundary_fluxes(as0, bcs);
    // fv::compute_source_terms(as0);
    // fv::pv_to_cv(as0);
    // fv::update_rhs(as0);
    fv::old_advance(as0, num_flux, dt, physics.cv());
    // fv::advance_variables(as0, dt);
    // if (timestep % 10 == 0) {
    bcs.apply(as0);
    solver::fv::vtk::serialize(as0, "result", timestep, physics.cv(), 0_gn);
    //}
  }
}
*/
void sod_test(mpi::env& env) {
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("fv_euler_sod_2d", comm);
  io::session s(io::create, "fv_euler_sod_2d", comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 1;
  auto min_grid_level       = 0_u;
  auto max_grid_level       = min_grid_level + 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::square<nd>::unit();

  // Create the grid
  grid::mhc<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  grid::generation::uniform(g, min_grid_level);

  auto solver_block_capacity
   = grid::grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};

  using p        = fv::euler::physics<nd>;
  using solver_t = fv::state<p>;

  p physics{1.4};

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};
  // g.refine(tree::node_idx{3});

  /// Initial advection solver grid:
  solver::initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  auto ic = fv::euler::ic::sod_shock_tube<nd>();

  // Initial condition by cell
  for (auto& b : as0.blocks()) {
    b.for_each_internal([&](auto&& c) {
      auto pvs = ic(b.center(c));
      b.variables(c.idx) = physics.pv().to_cv(pvs);
    });
  }

  /// Boundary conditions
  auto bcs = NeumannBoundaryConditions{};

  num_t cfl = 0.4;
  bcs.apply(as0);
  solver::fv::vtk::serialize(as0, "result", 0, physics.cv(), 0_gn);
  num_t time      = 0;
  num_t time_end  = 0.2;
  num_t time_step = 0;

  const auto cv = physics.cv();
  // const auto pv = physics.pv();
  auto nf       = fv::euler::flux::ausm;
  auto ts       = fv::euler::time_step;
  auto li       = fv::limiter::van_albada;
  auto time_int = fv::runge_kutta;

  while (!math::approx(time, time_end)) {
    bcs.apply(as0);
    fv::initialize_time_integration(as0);
    while (!as0.time_integration.done()) {
      //   // for (auto rkstep : fv::runge_kutta_steacdps()) {
      {  // can happen in parallel
        fv::clear_rhs(as0);
        // fv::clear_halos(as0);  // in debug mode
      }
      // fv::exchange_halos(as0);
      // bcs.apply(as0);
      num_t dt = fv::compute_time_step(as0, cfl, ts, cv, time, time_end);
      std::cout << "step: " << time_step << " | time: " << time
                << " | dt: " << dt << std::endl;
      fv::compute_structured_gradients(as0, li);
      // fv::correct_boundary_gradients(as0, bcs);
      fv::compute_structured_rhs(as0, nf, dt, cv);
      // fv::compute_boundary_rhs(as0, nf, dt, cv);
      // fv::compute_source_term_rhs(as0);
      as0.time_integration.advance_rhs(as0, dt);
      time += dt;
      time_step++;

      bcs.apply(as0);
    }
  }
  solver::fv::vtk::serialize(as0, "result", time_step, physics.cv(), 0_gn);
}

int main(int argc, char* argv[]) {
  /// Initialize MPI
  mpi::env env(argc, argv);

  check_euler();
  sod_test(env);

  // explosion_test(env);

  return test::result();
}
