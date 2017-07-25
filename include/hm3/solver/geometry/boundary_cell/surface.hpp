#pragma once
/// \file
///
/// Boundary cell surface.
#include <hm3/solver/geometry/surface/boundary.hpp>
#include <hm3/solver/geometry/surface/internal.hpp>
#include <hm3/solver/geometry/surface/types.hpp>
#include <hm3/solver/geometry/types.hpp>
#include <hm3/solver/types.hpp>

namespace hm3::solver::geometry {

namespace boundary_cell {

/// Surface of a boundary cell
template <dim_t Ad>
struct surface {
  using self               = surface<Ad>;
  using surface_geometry   = cell_surface_type<Ad>;
  using internal_surface_t = internal_surface<Ad>;
  using boundary_surface_t = boundary_surface<Ad>;

  using data_t = variant<surface_geometry,    //
                         internal_surface_t,  //
                         boundary_surface_t   //
                         >;

  using geometry_type = hg::associated::t_<internal_surface_t>;

  static_assert(
   Same<geometry_type,  //
        hg::associated::t_<boundary_surface_t>>{},
   "the internal and boundary surfaces must have the same geometry type");

  // clang-format off

  using edge_value_type = meta::if_c<Ad == 2, self,                        //
                          meta::if_c<Ad == 3, hg::segment<Ad>,
                                     void>>;
  // clang-format on

  data_t value;

  /// Type of the boundary cell surface
  surface_t type() const noexcept {
    switch (value.index()) {
      case 0: {
        return surface_t::geometry;
      }
      case 1: {
        return surface_t::internal;
      }
      case 2: {
        return surface_t::boundary;
      }
      default: { HM3_UNREACHABLE(); }
    }
  }

  cell_idx_t cell_idx() const noexcept {
    HM3_ASSERT(type() == surface_t::internal, "");
    auto* ptr = get_if<internal_surface_t>(&value);
    HM3_ASSERT(ptr != nullptr, "");
    return (*ptr).internal_cell_idx;
  }

  boundary_idx_t boundary_idx() const noexcept {
    HM3_ASSERT(type() == surface_t::boundary, "");
    auto* ptr = get_if<boundary_surface_t>(&value);
    HM3_ASSERT(ptr != nullptr, "");
    return (*ptr).id;
  }

  surface()               = default;
  surface(surface const&) = default;
  surface(surface&&)      = default;
  surface& operator=(surface const&) = default;
  surface& operator=(surface&&) = default;
  surface(internal_surface_t i) : value{std::move(i)} {}
  surface(boundary_surface_t i) : value{std::move(i)} {}
  surface(surface_geometry i) : value{std::move(i)} {}

  constexpr bool operator==(self const& other) const noexcept {
    return value == other.value;
  }
  constexpr bool operator!=(self const& other) const noexcept {
    return value != other.value;
  }

  CONCEPT_REQUIRES(Ad == 1)
  constexpr bool operator<(self const& other) const noexcept {
    return value < other.value;
  }
  CONCEPT_REQUIRES(Ad == 1)
  constexpr bool operator<=(self const& other) const noexcept {
    return value <= other.value;
  }
  CONCEPT_REQUIRES(Ad == 1)
  constexpr bool operator>(self const& other) const noexcept {
    return value > other.value;
  }
  CONCEPT_REQUIRES(Ad == 1)
  constexpr bool operator>=(self const& other) const noexcept {
    return value >= other.value;
  }

  /// \name Vertex
  ///@{

  CONCEPT_REQUIRES(Ad == 1) constexpr auto operator()(dim_t i) noexcept {
    return visit([i](auto&& v) { return v(i); }, value);
  }

  CONCEPT_REQUIRES(Ad == 1)
  constexpr auto operator()(dim_t i) const noexcept {
    return visit([i](auto&& v) { return v(i); }, value);
  }

  CONCEPT_REQUIRES(Ad == 1)
  constexpr auto operator[](dim_t i) noexcept { return (*this)(i); }

  CONCEPT_REQUIRES(Ad == 1)
  constexpr auto operator[](dim_t i) const noexcept { return (*this)(i); }

  CONCEPT_REQUIRES(Ad == 1)
  static constexpr self unit(dim_t d) noexcept {
    return surface_geometry{surface_geometry::unit(d)};
  }

  CONCEPT_REQUIRES(Ad == 1)
  static constexpr self ones() noexcept {
    return surface_geometry{surface_geometry::ones()};
  }

  CONCEPT_REQUIRES(Ad == 1)
  static constexpr self zero() noexcept {
    return surface_geometry{surface_geometry::zero()};
  }

  CONCEPT_REQUIRES(Ad == 1)
  static constexpr self constant(num_t v) noexcept {
    return surface_geometry{surface_geometry::constant(v)};
  }

  CONCEPT_REQUIRES(Ad == 1)
  auto begin() noexcept {
    return visit([](auto&& v) { return ranges::begin(v); }, value);
  }

  CONCEPT_REQUIRES(Ad == 1)
  auto end() noexcept {
    return visit([](auto&& v) { return ranges::end(v); }, value);
  }

  CONCEPT_REQUIRES(Ad == 1)
  auto size() noexcept {
    return visit([](auto&& v) { return ranges::size(v); }, value);
  }

  CONCEPT_REQUIRES(Ad == 1)
  auto begin() const noexcept {
    return visit([](auto&& v) { return ranges::begin(v); }, value);
  }

  CONCEPT_REQUIRES(Ad == 1)
  auto end() const noexcept {
    return visit([](auto&& v) { return ranges::end(v); }, value);
  }

  CONCEPT_REQUIRES(Ad == 1)
  auto size() const noexcept {
    return visit([](auto&& v) { return ranges::size(v); }, value);
  }

  ///@} // Vertex

  /// \name Edge
  ///@{

  CONCEPT_REQUIRES(Ad == 2)
  auto x(dim_t i) const noexcept {
    return visit([i](auto&& v) { return v.x(i); }, value);
  }

  CONCEPT_REQUIRES(Ad == 2)
  auto line() const noexcept {
    return visit([](auto&& v) { return v.line(); }, value);
  }
  ///@} // Edge

  /// \name Polygon
  ///@{

  CONCEPT_REQUIRES(Ad == 3)
  auto polyline() const noexcept {
    return visit(
     [](auto&& v) {
       using polyline_t = hg::polyline<3>;
       return polyline_t(hg::edges(v.polyline()));
     },
     value);
  }

  // CONCEPT_REQUIRES(Ad == 3)
  // auto reserve(suint_t n) const noexcept {
  //   return visit([n](auto&& v) { return v.reserve(n); }, value);
  // }

  // CONCEPT_REQUIRES(Ad == 3)
  // auto push_back_edge(edge_value_type e) const noexcept {
  //   return visit([&](auto&& v) { return v.push_back_edge(e); }, value);
  // }

  ///@}
};  // namespace hm3::solver::geometry

/// \name VertexAccess
///@{

/// Number of vertices of the surface \p p.
template <dim_t Ad>
constexpr auto vertex_size(surface<Ad> const& p) noexcept {
  return visit([](auto&& v) { return vertex_size(v); }, p.value);
}

/// Vertex \p i of the surface \p p.
template <dim_t Ad>
constexpr auto vertex(surface<Ad> const& p, dim_t i) noexcept {
  return visit([i](auto&& v) { return vertex(v, i); }, p.value);
}

///@} // VertexAccess

/// \name EdgeAccess
///@{

/// Number of edges in the surface \p p.
template <dim_t Ad, CONCEPT_REQUIRES_(Ad > 1)>
constexpr auto edge_size(surface<Ad> const& p) noexcept {
  return visit([](auto&& v) { return edge_size(v); }, p.value);
}

/// Edge \p e of the surface \p p.
template <dim_t Ad, CONCEPT_REQUIRES_(Ad > 1)>
constexpr auto edge(surface<Ad> const& p, dim_t e) noexcept {
  return visit([e](auto&& v) { return edge(v, e); }, p.value);
}

///@}  // EdgeAccess

}  // namespace boundary_cell
}  // namespace hm3::solver::geometry
