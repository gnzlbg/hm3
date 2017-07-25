#pragma once
/// \file
///
/// Boundary surface
#include <hm3/solver/geometry/surface/types.hpp>

namespace hm3::solver::geometry {

/// Boundary Surface.
///
/// Its geometry is a point in 1D, segment in 2D, and triangle in 3D.
/// It also stores a boundary index as associated data.
template <dim_t Ad>
struct boundary_surface : hg::simplex<Ad> {
  using self      = boundary_surface<Ad>;
  using surface_t = hg::simplex<Ad>;
  using surface_t::surface_t;

  boundary_surface()                        = default;
  boundary_surface(boundary_surface const&) = default;
  boundary_surface(boundary_surface&&)      = default;
  boundary_surface& operator=(boundary_surface const&) = default;
  boundary_surface& operator=(boundary_surface&&) = default;

  boundary_surface(surface_t s, boundary_idx_t v = boundary_idx_t{})
   : surface_t(std::move(s)), id(v) {}

  boundary_idx_t id{};

  friend bool operator==(self const& a, self const& b) {
    return a.id == b.id
           and static_cast<surface_t const&>(a)
                == static_cast<surface_t const&>(b);
  }
  friend bool operator!=(self const& a, self const& b) { return !(a == b); }

  friend bool operator<(self const& a, self const& b) {
    return static_cast<surface_t const&>(a) < static_cast<surface_t const&>(b);
  }

  friend bool operator<=(self const& a, self const& b) {
    return a == b or a < b;
  }
  friend bool operator>(self const& a, self const& b) { return !(a <= b); }
  friend bool operator>=(self const& a, self const& b) { return !(a < b); }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace hm3::solver::geometry
