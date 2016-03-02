/// \file
///
/// Hierarchical Cartesian Grid 1D tests
#include "grid.hpp"
#include "tree/tree_1d.hpp"

using namespace hm3;
using namespace test;

/// Explicit instantiate it
template struct hm3::grid::hc::single<1>;

struct uniform_grid {
  std::vector<grid_node> nodes{
   {idx{0}, gc{0.5}, cl{1.0}},       //
   {idx{1}, gc{0.25}, cl{0.5}},      //
   {idx{2}, gc{0.75}, cl{0.5}},      //
   {idx{3}, gc{.125}, cl{0.25}},     //
   {idx{4}, gc{.375}, cl{0.25}},     //
   {idx{5}, gc{0.625}, cl{0.25}},    //
   {idx{6}, gc{0.875}, cl{0.25}},    //
   {idx{7}, gc{.0625}, cl{0.125}},   //
   {idx{8}, gc{.1875}, cl{0.125}},   //
   {idx{9}, gc{.3125}, cl{0.125}},   //
   {idx{10}, gc{.4375}, cl{0.125}},  //
   {idx{11}, gc{.5625}, cl{0.125}},  //
   {idx{12}, gc{.6875}, cl{0.125}},  //
   {idx{13}, gc{.8125}, cl{0.125}},  //
   {idx{14}, gc{.9375}, cl{0.125}}   //
                                     //
  };
};

auto uniform_grid_v = extend(uniform_tree{}, uniform_grid{});

int main() {
  auto bbox = geometry::unit(geometry::square<1>{});
  grid::hc::single<1_u> g(20, bbox);
  g.refine(0_n);
  g.refine(1_n);
  g.refine(2_n);
  g.refine(3_n);
  g.refine(4_n);
  g.refine(5_n);
  g.refine(6_n);

  check_grid(g, uniform_grid_v);
  return test::result();
}
