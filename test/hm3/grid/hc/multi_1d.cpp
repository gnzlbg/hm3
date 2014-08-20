/// \file multi_1d.cpp
#include <hm3/grid/hc/multi.hpp>
#include <hm3/utility/test.hpp>

/// Explicit instantiate it
template struct hm3::grid::hc::multi<1>;

using namespace hm3;
using namespace grid;

int main(int argc, char* argv[]) {
  mpi::env env(argc, argv);
  auto comm = env.world();
  io::session::remove("hc_multi_1d", comm);
  io::session s(io::create, "hc_multi_1d", comm);

  auto bbox = geometry::square<1>::unit();
  hc::multi<1> g(s, 20, 2, bbox);

  g.refine(0_n);
  g.refine(1_n);
  g.refine(2_n);

  g.node(3_n, 0_g) = 0_gn;
  g.node(4_n, 0_g) = 1_gn;

  g.node(4_n, 1_g) = 0_gn;
  g.node(5_n, 1_g) = 1_gn;
  g.node(6_n, 1_g) = 2_gn;

  CHECK(g.is_leaf(4_n));
  g.refine(4_n);
  g.node(4_n, 1_g) = grid_node_idx{};
  g.node(7_n, 1_g) = 3_gn;
  g.node(8_n, 1_g) = 4_gn;
  CHECK(!g.is_leaf(4_n));

  g.node(4_n, 1_g) = 0_gn;
  g.remove(7_n, 1_g);
  g.remove(8_n, 1_g);
  CHECK(g.is_leaf(4_n));

  g.refine(4_n);
  CHECK(!g.is_leaf(4_n));

  g.node(7_n, 1_g) = 3_gn;
  g.node(8_n, 1_g) = 4_gn;
  CHECK(!g.is_leaf(4_n));
  g.node(8_n, 0_g) = 2_gn;
  CHECK(!g.is_leaf(4_n));

  g.remove(7_n, 1_g);
  g.remove(8_n, 1_g);
  CHECK(!g.is_leaf(4_n));
  CHECK(g.in_grid(8_n, 0_g));
  CHECK(!g.in_grid(7_n, 1_g));
  CHECK(!g.in_grid(8_n, 1_g));

  return test::result();
}
