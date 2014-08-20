/// \file
///
/// Hierarchical Cartesian Grid 2D tests
#include "grid.hpp"
#include "../../tree/tree_2d.hpp"

using namespace hm3;
using namespace test;

/// Explicit instantiate it
template struct hm3::grid::hc::single<2>;

struct uniform_grid {
  std::vector<grid_node> nodes{
   {idx{0}, gc{.5, .5}, cl{1.0}},       //
   {idx{1}, gc{.25, .25}, cl{.5}},      //
   {idx{2}, gc{.75, .25}, cl{.5}},      //
   {idx{3}, gc{.25, .75}, cl{.5}},      //
   {idx{4}, gc{.75, .75}, cl{.5}},      //
   {idx{5}, gc{.125, .125}, cl{.25}},   //
   {idx{6}, gc{.375, .125}, cl{.25}},   //
   {idx{7}, gc{.125, .375}, cl{.25}},   //
   {idx{8}, gc{.375, .375}, cl{.25}},   //
   {idx{9}, gc{.625, .125}, cl{.25}},   //
   {idx{10}, gc{.875, .125}, cl{.25}},  //
   {idx{11}, gc{.625, .375}, cl{.25}},  //
   {idx{12}, gc{.875, .375}, cl{.25}},  //
   {idx{13}, gc{.125, .625}, cl{.25}},  //
   {idx{14}, gc{.375, .625}, cl{.25}},  //
   {idx{15}, gc{.125, .875}, cl{.25}},  //
   {idx{16}, gc{.375, .875}, cl{.25}},  //
   {idx{17}, gc{.625, .625}, cl{.25}},  //
   {idx{18}, gc{.875, .625}, cl{.25}},  //
   {idx{19}, gc{.625, .875}, cl{.25}},  //
   {idx{20}, gc{.875, .875}, cl{.25}}   //
                                        //
  };
};

auto uniform_grid_v = extend(uniform_tree{}, uniform_grid{});

int main() {
  auto bbox = geometry::square<2>::unit();
  grid::hc::single<2> g(21, bbox);
  g.refine(0_n);
  g.refine(1_n);
  g.refine(2_n);
  g.refine(3_n);
  g.refine(4_n);

  check_grid(g, uniform_grid_v);
  return test::result();
}
