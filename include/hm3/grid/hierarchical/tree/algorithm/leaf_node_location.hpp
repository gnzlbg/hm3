#pragma once
/// \file
///
/// Compute leaf node location algorithm
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/algorithm/intersection/box_point.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_or_parent_at.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/shift_location.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/location/default.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/utility/config/assert.hpp>

namespace hm3 {
namespace tree {
//

struct leaf_node_location_fn {
  /// Location code of the leaf node at spatial position \p x within the tree
  template <typename Tree, dim_t Nd, typename Loc = loc_t<Nd>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& tree, geometry::point<Nd> x,
                  Loc loc = Loc::min()) const noexcept {
    HM3_ASSERT(loc == Loc::min(), "!");
    for (dim_t d = 0; d < Nd; ++d) {
      HM3_ASSERT((x(d) > 0. or math::approx(x(d), 0.))
                  and (x(d) < 1.0 or math::approx(x(d), 1.0)),
                 "x({}) = {} is not in normalized coordinates [0, 1]", d, x(d));

      // TODO: figure out why this is needed: probably because the coordinates
      // must be in range [0, 1), but maybe that is fixable ?
      if (math::approx(x(d), 1.0)) { x(d) -= math::eps; }
    }

    auto leaf_node_loc = shift_location(loc, x);
    return node_or_parent_at(tree, leaf_node_loc);
  }
  /// Location code of the leaf node at spatial position \p x within the tree
  /// with bounding box \p bbox
  ///
  /// Normalizes the coordinate \p x to the tree's [0, 1] frame and then shifts
  /// the first coordinate by it.
  template <typename Tree, dim_t Nd, typename Loc = loc_t<Nd>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& tree, geometry::box<Nd> bbox,
                  geometry::point<Nd> x, Loc loc = Loc::min()) const noexcept {
    HM3_ASSERT(geometry::intersection.test(bbox, x),
               "point {} is not inside bounding box {}", x, bbox);

    // Normalize x
    auto x_min = geometry::x_min(bbox);
    x() -= x_min();
    x() /= geometry::bounding_length(bbox);

    return (*this)(tree, x, loc);
  }
};

namespace {
constexpr auto&& leaf_node_location
 = static_const<leaf_node_location_fn>::value;
}  // namespace

}  // namespace tree
}  // namespace hm3
