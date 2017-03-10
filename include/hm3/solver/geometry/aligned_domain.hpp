#ifdef FIXED
#pragma once
/// \file
///
/// Solver domain
#include <hm3/solver/utility/aligned_boundary.hpp>
#include <hm3/utility/optional.hpp>
//#include <hm3/solver/utility/simplex_boundary.hpp>
//#include <hm3/solver/utility/sd_boundary.hpp>

namespace hm3::solver {

/// Geometry of the computational domain.
template <dim_t Ad>
struct domain {
  using bbox_t = geometry::aabb<Ad>;
  vector<aligned_boundary<Ad>> aligned_boundaries;
  // vector<simplex_boundary<Ad>> simplex_boundaries;
  // vector<sd_boundary<Ad>> sd_boundaries;

  /// Relative position of a geometry \p g with respect to the computational
  /// domain.
  template <typename Geometry>
  geometry::relative_position_t relative_position(Geometry&& g) const noexcept {
    using rp_t = geometry::relative_position_t;
    small_vector<rp_t, 8> rps;

    for (auto&& ab : aligned_boundaries) {
      rps.push_back(geometry::relative_position(g, ab.geometry()));
      if (geometry::is_outside(rps.back())) { return rp_t::outside; }
    }

    if (ranges::any_of(rps, geometry::is_intersected)) {
      return rp_t::intersected;
    }

    return rp_t::inside;
  }

  /// Bounding box of the domain (if any)
  optional<bbox_t> bounding_box() const noexcept { return {}; }

  /// Does the geometry \p g intersect the bbox of any of the boundaries?
  template <typename Geometry>
  bool intersects_boundary(Geometry&& g) const noexcept {
    for (auto&& ab : aligned_boundaries) {
      if (ab.intersection_test(g)) { return true; }
    }
    return false;
  }

  template <typename Cells>
  void reshape_cells(Cells& cs) const noexcept {
    for (auto&& ab : aligned_boundaries) { ab.reshape_cells(cs); }
  }
};

}  // namespace hm3::solver
#endif
