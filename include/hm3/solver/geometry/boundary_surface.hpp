#pragma once
/// \file
///
/// Boundary surface
#include <hm3/solver/geometry/types.hpp>

namespace hm3::solver::geometry {

/// Boundary Surface.
///
/// Its geometry is a point in 1D, segment in 2D, and triangle in 3D.
/// It also stores a boundary index as associated data.
template <dim_t Ad>
struct boundary_surface : hg::simplex<Ad> {
  using simplex_t = hg::simplex<Ad>;
  using self      = boundary_surface<Ad>;
  using simplex_t::simplex_t;

  boundary_surface()                        = default;
  boundary_surface(boundary_surface const&) = default;
  boundary_surface(boundary_surface&&)      = default;
  boundary_surface& operator=(boundary_surface const&) = default;
  boundary_surface& operator=(boundary_surface&&) = default;

  boundary_surface(simplex_t s, bidx_t v = bidx_t{})
   : simplex_t(std::move(s)), id(v) {}

  bidx_t id{};

  friend bool operator==(self const& a, self const& b) {
    return a.id == b.id
           and static_cast<simplex_t const&>(a)
                == static_cast<simplex_t const&>(b);
  }
  friend bool operator!=(self const& a, self const& b) { return !(a == b); }

  friend bool operator<(self const& a, self const& b) {
    return static_cast<simplex_t const&>(a) < static_cast<simplex_t const&>(b);
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
