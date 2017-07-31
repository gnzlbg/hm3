#pragma once
/// \file
///
/// Simple internal cell surface
#include <hm3/solver/geometry/surface/types.hpp>
#include <hm3/solver/types.hpp>

namespace hm3::solver::geometry {

/// Internal Surface.
///
/// Its geometry is a point in 1D, segment in 2D, and a polygon in 3D.
/// It also stores the index of the adjacent internal cell as associated data.
template <dim_t Ad>
struct internal_surface : cell_surface_type<Ad> {
  using surface_t = cell_surface_type<Ad>;
  using self      = internal_surface<Ad>;
  using surface_t::surface_t;
  // clang-format off
  using concatenated_value_type
  = meta::if_c<Ad == 1 or Ad == 2,
               hg::polyline<Ad, self, void>,
               // TODO: meta::if_c<Ad == 3,
               //       hg::polysurface<Ad, surface<Ad>, void>,
               void>;
  // clang-format on

  internal_surface()                        = default;
  internal_surface(internal_surface const&) = default;
  internal_surface(internal_surface&&)      = default;
  internal_surface& operator=(internal_surface const&) = default;
  internal_surface& operator=(internal_surface&&) = default;

  internal_surface(surface_t s, cell_idx_t v = cell_idx_t{})
   : surface_t(std::move(s)), internal_cell_idx(v) {}

  cell_idx_t internal_cell_idx{};

  friend bool operator==(self const& a, self const& b) {
    return a.internal_cell_idx == b.internal_cell_idx
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

template <typename OS, dim_t Ad>
OS& to_ascii(OS& os, internal_surface<Ad> const& s,
             ascii_fmt::solver_geometry) {
  os << "{ is | g: ";
  using s_t = typename internal_surface<Ad>::surface_t;
  ascii_fmt::to_ascii(os, static_cast<s_t const&>(s));
  os << " | d: ";
  ascii_fmt::to_ascii(os, s.internal_cell_idx);
  os << "}";
  return os;
}

}  // namespace hm3::solver::geometry
