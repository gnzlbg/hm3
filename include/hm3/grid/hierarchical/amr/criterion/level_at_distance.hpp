#pragma once
/// \file
///
/// Cells at a given distance from the boundary are refined until they are at a
/// given level
#include <hm3/grid/hierarchical/amr/action.hpp>
#include <hm3/grid/hierarchical/types.hpp>
#include <type_traits>
#include <utility>
#include <vector>

namespace hm3 {
namespace amr {
namespace criterion {

/// Enforces a target level `l_t` up to a target distance `d_t`
///
/// d_n := node distance
/// l_n := node level
///
/// if d_n < d_t and,
///   l_n < l_t => refine,
///   l_n > l_t => coarsen,
///   l_n = l_t => none,
/// otherwise => coarsen
///
struct level_till_distance {
  num_t target_distance;
  uint_t target_level;

  level_till_distance()                           = default;
  level_till_distance(level_till_distance const&) = default;
  level_till_distance(level_till_distance&&)      = default;
  level_till_distance& operator=(level_till_distance const&) = default;
  level_till_distance& operator=(level_till_distance&&) = default;

  level_till_distance(num_t d_t, uint_t l_t)
   : target_distance(d_t), target_level(l_t) {}

  template <typename Grid, typename SD, typename Node>
  action operator()(Grid&& grid, SD&& distance_fn, Node n) const {
    auto node_level    = grid.level(n);
    auto node_distance = std::abs(distance_fn(grid.coordinates(n)));

    if (node_distance < target_distance) {
      if (node_level < target_level) { return action::refine; }

      if (node_level > target_level) {
        return action::coarsen;
      }  // node_level == target_level:
      return action::none;
    }  // node_distance >= target_distance
    return action::coarsen;
  }
};

template <typename Step> struct multiple {
  using Steps = std::vector<Step>;
  Steps steps;

  multiple()                = default;
  multiple(multiple const&) = default;
  multiple(multiple&&)      = default;
  multiple& operator=(multiple const&) = default;
  multiple& operator=(multiple&&) = default;

  multiple(std::size_t n) : steps(n) {}
  multiple(Steps s) : steps{std::move(s)} {}

  multiple(std::initializer_list<Step> s) : multiple{Steps{s}} {}

  template <typename Grid, typename SD, typename Node>
  amr::action operator()(Grid&& g, SD&& d_fn, Node n) const {
    return accumulate(steps, action::coarsen, [
      g = std::forward<Grid>(g), d_fn = std::forward<SD>(d_fn), n
    ](auto&& acc, auto a) { return combine(acc, a(g, d_fn, n)); });
  }
};

struct level_till_cell_distances {
 private:
  struct step {
    uint_t no_cells;
    uint_t target_level;
  };

  multiple<level_till_distance> r_;

 public:
  template <typename Grid>
  level_till_cell_distances(Grid const& grid, std::initializer_list<step> l)
   : r_(l.size()) {
    std::vector<step> steps_{l};
    sort(steps_, std::greater<>{}, &step::target_level);
    num_t distance = 0.;
    transform(steps_, begin(r_.steps), [&](auto&& s) {
      distance += s.no_cells * grid.length(tree::level_idx{s.target_level});
      return level_till_distance{distance, s.target_level};
    });
  }

  template <typename Grid, typename SD, typename Node>
  amr::action operator()(Grid&& grid, SD&& sd, Node n) const {
    return r_(std::forward<Grid>(grid), std::forward<SD>(sd), n);
  }
};

}  // namespace criterion
}  // namespace amr
}  // namespace hm3
