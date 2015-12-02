#pragma once
/// \file
///
/// Test utilities for hierarchical Cartesian grids
#include <hm3/grid/hc/serialization/single_fio.hpp>
#include <hm3/grid/hc/single.hpp>
#include "../../tree/tree.hpp"

namespace hm3 {
namespace test {

/// Grid coordinates
using gc = tagged_initializer_list<class grid_coordinates_tag_, num_t>;
/// Grid node length
using cl = tagged_initializer_list<class grid_node_length_tag_, num_t>;

struct grid_node : node {
  using node::operator=;
  using node::init;

  hm3::opt::optional<std::vector<num_t>> node_coordinates{};
  hm3::opt::optional<num_t> node_length{};

  template <typename Arg, typename... Args,
            CONCEPT_REQUIRES_(!ConvertibleTo<std::decay_t<Arg>, grid_node>{})>
  grid_node(Arg&& arg, Args&&... args) {
    init(std::forward<Arg>(arg), std::forward<Args>(args)...);
  }

  template <typename Arg, typename... Args>
  void init(Arg&& arg, Args&&... args) {
    this->init(std::forward<Arg>(arg));
    this->init(std::forward<Args>(args)...);
  }

  void init(gc x) {
    node_coordinates = std::vector<num_t>(size(x));
    ranges::copy(x, begin(*node_coordinates));
  }
  void init(cl x) { node_length = *x; }
};

template <typename TreeNodes, typename GridNodes>
GridNodes extend(TreeNodes ts, GridNodes gs) {
  for (auto&& gn : gs.nodes) {
    auto idx   = *gn.idx;
    auto tn_it = find_if(ts.nodes, [&](auto&& n) { return *n.idx == idx; });
    if (tn_it != end(ts.nodes)) { gn = *tn_it; }
  }
  return gs;
}

template <typename Grid>
void test_node_coordinates(Grid const& g, grid_node const& n) {
  auto xc = g.coordinates(*n.idx);
  if (n.node_coordinates) {
    auto reference = *n.node_coordinates;
    test::check_approx_equal(xc, reference);
  }
}

template <typename Grid>
void test_node_length(Grid const& g, grid_node const& n) {
  auto l   = g.length(*n.idx);           // node lenght
  auto l_l = g.length(g.level(*n.idx));  // length at node level
  CHECK(l == l_l);                       // ^^ must match
  if (n.node_length) {
    auto reference = *n.node_length;
    CHECK(l == reference);
  }
}

template <typename Grid> void check_grid_node(Grid const& g, grid_node n) {
  test_node_coordinates(g, n);
  test_node_length(g, n);
}

template <typename Grid> void grid_consistency_checks(Grid const& g) {
  auto bounding_box = g.bounding_box();
  auto xc_0         = g.coordinates(0_n);
  auto l_0          = g.length(0_n);
  auto square_0     = geometry::square<Grid::dimension()>(xc_0, l_0);
  CHECK(bounding_box == square_0);
  consistency_checks(g);
}

template <typename Grid, typename ReferenceGrid>
void check_grid(Grid const& g, ReferenceGrid const& gref) {
  check_tree(g, gref);
  for (auto&& n : gref.nodes) { check_grid_node(g, n); }
  grid_consistency_checks(g);
  check_io(g, "uniform");
}

}  // namespace test
}  // namespace hm3
