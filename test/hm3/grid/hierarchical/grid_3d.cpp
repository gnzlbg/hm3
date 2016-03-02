/// \file
///
/// Hierarchical Cartesian Grid 3D tests
#include "grid.hpp"
#include "../../tree/tree_3d.hpp"

using namespace hm3;
using namespace test;

/// Explicit instantiate it
template struct hm3::grid::hc::single<3>;

struct uniform_grid {
  std::vector<grid_node> nodes{
   {idx{0}, gc{.5, .5, .5}, cl{1.0}},         //
   {idx{1}, gc{.25, .25, .25}, cl{.5}},       //
   {idx{2}, gc{.75, .25, .25}, cl{.5}},       //
   {idx{3}, gc{.25, .75, .25}, cl{.5}},       //
   {idx{4}, gc{.75, .75, .25}, cl{.5}},       //
   {idx{5}, gc{.25, .25, .75}, cl{.5}},       //
   {idx{6}, gc{.75, .25, .75}, cl{.5}},       //
   {idx{7}, gc{.25, .75, .75}, cl{.5}},       //
   {idx{8}, gc{.75, .75, .75}, cl{.5}},       //
   {idx{9}, gc{.125, .125, .125}, cl{.25}},   //
   {idx{10}, gc{.375, .125, .125}, cl{.25}},  //
   {idx{11}, gc{.125, .375, .125}, cl{.25}},  //
   {idx{12}, gc{.375, .375, .125}, cl{.25}},  //
   {idx{13}, gc{.125, .125, .375}, cl{.25}},  //
   {idx{14}, gc{.375, .125, .375}, cl{.25}},  //
   {idx{15}, gc{.125, .375, .375}, cl{.25}},  //
   {idx{16}, gc{.375, .375, .375}, cl{.25}}   //
                                              //
  };
};

auto uniform_grid_v = extend(uniform_tree{}, uniform_grid{});

int main() {
  auto bbox = geometry::unit(geometry::square<3>{});
  grid::hc::single<3> g(73, bbox);
  g.refine(0_n);
  g.refine(1_n);
  g.refine(2_n);
  g.refine(3_n);
  g.refine(4_n);
  g.refine(5_n);
  g.refine(6_n);
  g.refine(7_n);
  g.refine(8_n);

  check_grid(g, uniform_grid_v);

  return test::result();
}
