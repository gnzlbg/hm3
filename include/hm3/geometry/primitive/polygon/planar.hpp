#pragma once
/// \file
///
/// Utilities for working in the polygon's plane (in 3D).
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/polygon/colinear.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

namespace planar_detail {

struct planar_fn {
  /// Is a polygon planar?
  template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
            CONCEPT_REQUIRES_(Polygon<UP, 2>{})>
  static constexpr bool is(P&&) noexcept {
    return true;  // a 2D polygon is always planar
  }

  /// Is a polygon planar?
  template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  static constexpr bool is(P&& p) noexcept {
    auto no_vertices = vertex_size(p);
    HM3_ASSERT(no_vertices > 2, "");

    // Get 3 (hopefully non-colinear) polygon vertices:
    auto v0 = vertex(p, 0);
    auto v1 = vertex(p, 1);
    auto v2 = vertex(p, no_vertices - 1);

    // If this assertion triggers the solution is to find a way to obtain 3
    // non-colinear vertices of a polygon:
    HM3_ASSERT(!colinear(p), "the vertices {}, {}, and {}, are collinear!", v0,
               v1, v2);
    // Build a plane
    auto plane = Eigen::Hyperplane<num_t, 3>::Through(v0(), v1(), v2());

    // point lies in plane if the distance to the plane is smaller than some
    // tolerance:
    for (dim_t v = 2; v < no_vertices - 1; ++v) {
      if (plane.absDistance(vertex(p, v)()) > math::eps) { return false; }
    }
    return true;
  }

  struct basis_t {
    point<3> o;
    point<3> e[2];
  };

  /// Returns a planar basis for the polygon \p p, that is, two orthogonal
  /// vectors in the polygon's plane.
  template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  static constexpr auto basis(P&& p) noexcept -> basis_t {
    HM3_ASSERT(is(p), "polygon {} is not planar", p);

    auto no_vertices = vertex_size(p);

    // Get 3 (hopefully non-colinear) vertices:
    auto v0 = vertex(p, 0);
    auto vx = vertex(p, 1);
    auto vy = vertex(p, no_vertices - 1);

    // Define the x-axis
    auto x_axis = (vx() - v0()).normalized().eval();
    // The y-axis is orthogonal to the x-axis and the normal:
    auto y_axis = x_axis
                   .cross(  // normal vector:
                    x_axis.cross(vy() - v0()))
                   .normalized()
                   .eval();

    return basis_t{v0, {point<3>(x_axis), point<3>(y_axis)}};
  }

  /// Planar coordinates of the point \p p.
  static auto to(point<3> const& p, basis_t const& b) noexcept -> point<3> {
    auto lp  = point<3>::constant(0.);
    auto tmp = (p() - b.o()).eval();
    lp(0)    = (tmp).dot(b.e[0]());
    lp(1)    = (tmp).dot(b.e[1]());
    return lp;
  }

  /// Planar coordinates of the polygon \p p.
  template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  static constexpr auto to(P&& p, basis_t const& b) noexcept -> UP {
    HM3_ASSERT(is(p), "polygon {} is not planar", p);
    UP lp(p);
    for (auto&& v : vertices(lp)) { v = to(v, b); }
    return lp;
  }

  /// Planar coordinates of the polygon \p p.
  template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  static constexpr auto to(P&& p) noexcept -> UP {
    auto e = basis(p);
    return to(std::forward<P>(p), e);
  }

  /// From point in non-planar coordinates of the polygon \p p and base \p b
  /// back to 3D space.
  template <dim_t Nd>
  static constexpr auto from(point<Nd> p, basis_t const& b) noexcept
   -> point<3> {
    return point<3>(b.o() + b.e[0]() * p(0) + b.e[1]() * p(1));
  }
};

}  // namespace planar_detail

namespace {
constexpr auto const& planar = static_const<planar_detail::planar_fn>::value;
}  // namespace

}  // namespace polygon_primitive

using polygon_primitive::planar;

}  // namespace hm3::geometry
