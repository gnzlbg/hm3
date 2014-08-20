#pragma once
/// \file normalized_coordinates.hpp
#include <array>
#include <hm3/tree/algorithm/node_location.hpp>
#include <hm3/tree/concepts.hpp>
#include <hm3/tree/types.hpp>
#include <hm3/tree/relations/tree.hpp>
#include <hm3/tree/location/default.hpp>
#include <hm3/utility/static_const.hpp>
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace tree {

struct normalized_coordinates_fn {
  /// Returns the position of the location \p loc in normalized coordinates
  ///
  /// \note normalized coordinates means normalized by the length of the root
  /// node, that is, the coordinates are in range (0., 1.).
  template <typename Loc, int nd = Loc::dimension(),
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc loc) const noexcept -> geometry::point<nd> {
    auto result = geometry::point<nd>::constant(0.5);

    uint_t l = 0;
    for (auto&& p : loc()) {
      const auto length = node_length_at_level(l) * num_t{0.25};
      const auto&& rcp  = relative_child_position<nd>(p);
      for (auto&& d : dimensions(nd)) { result[d] += rcp[d] * length; }
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
