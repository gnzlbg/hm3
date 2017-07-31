#pragma once
/// \file
///
/// Boundary cell geometry.

#include <hm3/solver/geometry/boundary_cell/surface.hpp>
#include <hm3/solver/geometry/types.hpp>

namespace hm3::solver::geometry {

/// Boundary cell types
namespace boundary_cell {

/// Boundary cell data
struct data {
  cell_idx_t cell_idx;

  friend constexpr bool operator==(data const& a, data const& b) noexcept {
    return a.cell_idx == b.cell_idx;
  }
  friend constexpr bool operator!=(data const& a, data const& b) noexcept {
    return !(a == b);
  }
};

template <typename OS>
OS& to_ascii(OS& os, data const& d, ascii_fmt::solver_geometry) noexcept {
  ascii_fmt::to_ascii(os, d.cell_idx);
  return os;
}
// clang-format off

/// Geometry of a boundary cell
template <dim_t Ad, typename CellData>
using geometry
= std::conditional_t<Ad == 1, hg::segment<1, surface<1>, CellData>,
  std::conditional_t<Ad == 2, hg::polygon<2, surface<2>, CellData>,
  std::conditional_t<Ad == 3, hg::polyhedron<surface<3>, CellData>,
                     void>>>;
// clang-format on

/// Boundary cell
template <dim_t Ad, typename CellData = data>
struct cell : geometry<Ad, CellData> {
  using base = geometry<Ad, CellData>;
  using base::base;
  using base::operator=;

  using surface_t          = hg::associated::boundary_t<base>;
  using internal_surface_t = typename surface_t::internal_surface_t;
  using boundary_surface_t = typename surface_t::boundary_surface_t;

  template <typename Rng, typename S = range_value_t<Rng>,
            CONCEPT_REQUIRES_(Range<Rng>{} and ConvertibleTo<S, surface_t>{}
                              and not Same<uncvref_t<S>, surface_t>{})>
  cell(Rng&& rng)
   : base(rng | view::transform([](auto&& s) { return surface_t{s}; })) {}

  template <typename Rng, typename S = range_value_t<Rng>,
            CONCEPT_REQUIRES_(Range<Rng>{} and Same<uncvref_t<S>, surface_t>{})>
  cell(Rng&& rng) : base(std::forward<Rng>(rng)) {}
};

template <dim_t Ad, typename CD>
constexpr bool operator==(cell<Ad, CD> const& a, cell<Ad, CD> const& b) {
  ascii_fmt::err("cell eq\n");
  using bt = typename cell<Ad, CD>::base;
  return static_cast<bt const&>(a) == static_cast<bt const&>(b);
}

template <dim_t Ad, typename CD, typename CT = cell<Ad, CD>>
constexpr bool operator!=(cell<Ad, CD> const& a, cell<Ad, CD> const& b) {
  ascii_fmt::err("cell eq\n");
  return !(a == b);
}

template <typename OS, dim_t Ad, typename CD>
OS& to_ascii(OS& os, cell<Ad, CD> const& c,
             ascii_fmt::solver_geometry) noexcept {
  os << "{ bc | g: ";
  using gt = typename cell<Ad, CD>::base;
  ascii_fmt::to_ascii(os, static_cast<gt const&>(c), ascii_fmt::geometry{});
  os << " | d: ";
  ascii_fmt::to_ascii(os, data(c));
  os << "}";
  return os;
}

}  // namespace boundary_cell

}  // namespace hm3::solver::geometry
