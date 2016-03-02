#pragma once
/// \file
///
/// Normalized coordinates algorithm
#include <array>
#include <hm3/geometry/point.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_location.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/location/default.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>

namespace hm3 {
namespace tree {

struct normalized_coordinates_fn {
  /// Returns the position of the location \p loc in normalized coordinates
  ///
  /// \note normalized coordinates means normalized by the length of the root
  /// node, that is, the coordinates are in range (0., 1.).
  template <typename Loc, int Nd = Loc::dimension(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc loc) const noexcept -> geometry::point<Nd> {
    auto result = geometry::point<Nd>::constant(0.5);

    uint_t l = 0;
    for (auto&& p : loc()) {
      const auto length = node_length_at_level(l) * num_t{0.25};
      const auto&& rcp  = relative_child_position<Nd>(p);
      RANGES_FOR (auto&& d, dimensions(Nd)) { result[d] += rcp[d] * length; }
      ++l;
    }
    return result;
  }

  /// Returns the position of node \p n in normalized coordinates within the
  /// tree \p t
  ///
  /// \note normalized coordinates means normalized by the length of the root
  /// node, that is, the coordinates are in range (0., 1.).
  template <typename Tree, typename Loc = loc_t<Tree::dimension()>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& t, node_idx n, Loc l = Loc()) const noexcept {
    return (*this)(node_location(t, n, l));
  }
};

namespace {
constexpr auto&& normalized_coordinates
 = static_const<normalized_coordinates_fn>::value;
}  // namespace

}  // namespace tree
}  // namespace hm3
