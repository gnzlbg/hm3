#pragma once
/// \file
///
/// Is a polygon degenerate?
#include <hm3/geometry/discrete/primitive/polygon/concept.hpp>
#include <hm3/geometry/discrete/primitive/segment.hpp>  // TODO: move to concept once add edge iterators
#include <iostream>                                     // TODO: remove
namespace hm3 {
namespace geometry {
namespace discrete {
namespace polygon_primitive {

/// Does the polygon \p p has colinear vertices?
///
/// A polygon has colinear vertices if:
/// - two consecutive vertices are equal, or
/// - the direction of two consecutive segments is equal.
///
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<UP>{})>
constexpr bool has_colinear_vertices(P&& p) noexcept {
  auto no_vertices = vertex_size(p);

  auto line_dir = [&](dim_t i, dim_t j) {
    return direction(segment<Nd>::through(vertex(p, i), vertex(p, j)));
  };

  if (vertex(p, 0) == vertex(p, 1)) { return true; }
  auto past_dir = line_dir(0, 1);
  for (dim_t v = 1; v < no_vertices - 1; ++v) {
    auto new_dir = line_dir(v, v + 1);
    if (vertex(p, v) == vertex(p, v + 1) or past_dir == new_dir) {
      return true;
    }
    past_dir = new_dir;
  }

  return false;
}

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
    HM3_ASSERT(!has_colinear_vertices(p),
               "the vertices {}, {}, and {}, are collinear!", v0, v1, v2);

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
    // The normal is orthogonal to the x-axis and the vector to the 3rd point:
    auto y_axis = x_axis
                   .cross(vy() - v0())  // -> normal vector
                   // The y-axis is orthogonal to the x-axis and the normal:
                   .cross(x_axis)
                   .normalized()
                   .eval();
    return basis_t{v0, {point<3>(x_axis), point<3>(y_axis)}};
  }

  /// Planar coordinates of the point \p p.
  static auto to(point<3> p, basis_t const& b) noexcept -> point<3> {
    point<3> lp;
    lp(0) = (p() - b.o()).dot(b.e[0]());
    lp(1) = (p() - b.o()).dot(b.e[1]());
    lp(2) = 0.;
    return lp;
  }

  /// Planar coordinates of the polygon \p p.
  template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  static constexpr auto to(P&& p, basis_t const& b) noexcept -> UP {
    HM3_ASSERT(is(p), "polygon {} is not planar", p);

    UP lp(p);
    for (auto&& i : vertex_indices(p)) {
      vertex(lp, i)(0) = (vertex(p, i)() - b.o()).dot(b.e[0]());
      vertex(lp, i)(1) = (vertex(p, i)() - b.o()).dot(b.e[1]());
      vertex(lp, i)(2) = 0.0;
    }
    return p;
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

namespace {
static constexpr auto const& planar = static_const<planar_fn>::value;
}

namespace vertex_order_detail {

/// Returns true if the xy components of the vertices of the polygon \p p are
/// sorted in counter clock-wise (ccw) order, false otherwise.
template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr bool counter_clockwise_2d(P&& p) {
  auto&& vs    = vertices(p);
  const auto e = size(vs) - 1;
  num_t tmp    = 0.;
  for (dim_t i = 0; i < e; ++i) {
    tmp += (vs[i + 1](0) - vs[i](0)) * (vs[i + 1](1) + vs[i](1));
  }
  {  // wrap around
    tmp += (vs[0](0) - vs[e](0)) * (vs[0](1) + vs[e](1));
  }
  return tmp < 0;
}

struct vertex_order_fn {
  /// Returns true if the vertices of the polygon \p p are sorted in counter
  /// clock-wise (ccw) order, false otherwise.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
  static constexpr bool counter_clockwise(P&& p) {
    return counter_clockwise_2d(std::forward<P>(p));
  }

  // Counter-cloclwise doesn't make sense in 3D:
  /// Returns true if the vertices of the polygon \p p are sorted in counter
  /// clock-wise (ccw) order, false otherwise.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
  static constexpr bool counter_clockwise(P&& p) {
    return counter_clockwise_2d(planar.to(std::forward<P>(p)));
  }

  /// Returns true if the vertices of the polygon \p p are sorted in
  /// clock-wise (cw) order, false otherwise.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
  static constexpr bool clockwise(P&& p) {
    return !counter_clockwise(std::forward<P>(p));
  }
};

}  // namespace vertex_order_detail

namespace {
static constexpr auto const& vertex_order
 = static_const<vertex_order_detail::vertex_order_fn>::value;
}

/// Is the polygon \p p degenerate?
///
/// A polygon is degenerate if:
/// - it has repeated vertices
/// - any two consecutive segments are colinear (TODO: allow to fix this?)
/// - any two segments intersect (not at the vertices)
/// - any two non-consecutive segments intersect at the vertices
template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr bool degenerate(P&&) noexcept {
  return false;
}

}  // polygon_primitive

using polygon_primitive::vertex_order;

}  // discrete
}  // geometry
}  // hm3
