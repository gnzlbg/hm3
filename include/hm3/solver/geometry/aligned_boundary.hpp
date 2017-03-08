#pragma once
/// \file
///
/// Grid-aligned boundary
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/algorithm/intersection/box_plane.hpp>
#include <hm3/geometry/algorithm/relative_position.hpp>
#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitive/plane.hpp>

namespace hm3::solver {

/// Boundary aligned with a Cartesian direction of the grid
template <dim_t Ad>
struct aligned_boundary {
  using plane_t = geometry::plane<Ad>;
  plane_t boundary_;
  boundary_idx idx_;

  /// Boundary index of the aligned boundary.
  boundary_idx const& idx() const noexcept { return idx_; }
  /// Geometry of the aligned boundary.
  plane_t const& geometry() const noexcept { return boundary_; }

  aligned_boundary(plane_t boundary__, boundary_idx idx__)
   : boundary_(boundary__), idx_(idx__) {}

  /// Does the geometry \p g intersect the aligned boundary?
  template <typename Geometry>
  bool intersection_test(Geometry&& g) const noexcept {
    return hm3::geometry::intersection.test(g, geometry());
  }

  /// Reshapes a cell (in place) to conform to the boundary.
  template <typename Cell>
  void reshape_cell(Cell& c) const noexcept {
    using namespace hm3::geometry;
    HM3_ASSERT(intersection_test(c.geometry()),
               "error the cell {} doesn't intersect the boundary {}", c,
               boundary_);

#ifndef HM3_DISABLE_ASSERTIONS
    // if assertions are enabled, make sure that exactly only one boundary
    // surface is found
    //
    // if assertions are disabled, we are done when the first surface is found
    suint_t no_boundary_surfaces = 0;
#endif  // HM3_DISABLE_ASSERTIONS
    for (auto& s : c.surfaces()) {
      auto rp = relative_position(s.geometry(), geometry());

      // cell surface completely inside? => not a boundary
      if (rp == relative_position_t::inside) { continue; }

      // cell surface intersected but not parallel to the aligned boundary?
      // => not a boundary
      if (rp == relative_position_t::intersected
          and not parallel(s.geometry(), geometry())) {
        continue;
      }

      s.set_boundary_index(idx());

#ifdef HM3_DISABLE_ASSERTIONS
      return;  // first boundary surface found => done (assertions disabled)
#else          // not HM3_DISABLE_ASSERTIONS
      HM3_ASSERT(no_boundary_surfaces < 1, "too many boundary surfaces");
      ++no_boundary_surfaces;
#endif         // HM3_DISABLE_ASSERTIONS
    }
#ifdef HM3_DISABLE_ASSERTIONS
    HM3_FATAL_ERROR("no boundary surface found");
#else
    HM3_ASSERT(no_boundary_surfaces == 1, "{} boundary surfaces found",
               no_boundary_surfaces);
#endif
  }

  /// Reshape boundary cells \p cells with the boundary.
  template <typename Cells>
  void reshape_bcells(Cells& cells) const noexcept {
    for (auto&& c : cells) { reshape_cell(c); }
  }
};

}  // namespace hm3::solver
