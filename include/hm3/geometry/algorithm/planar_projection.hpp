#pragma once
/// \file
///
/// Is a polygon planar? / Project 3D polygon into a 2D plane.
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/collinear.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/vec.hpp>
#include <hm3/utility/optional.hpp>
#include <hm3/utility/tuple.hpp>

namespace hm3::geometry {

namespace planar_projection_detail {

/// Returns 3 non-collinear vertices of the polygon \p p.
template <typename P, typename UP = uncvref_t<P>,
          typename PT = associated::point_t<UP>,
          CONCEPT_REQUIRES_(Polygon<UP>{})>
optional<tuple<PT, PT, PT>> get_three_non_collinear_vertices(P&& p) {
  // Get first two vertices:
  PT v0   = vertex(p, 0);
  PT v1   = vertex(p, 1);
  auto e0 = edge(p, 0);

  // Find the next non-collinear vertex:
  {
    dim_t vidx = 2;
    dim_t eidx = vertex_size(p);
    while (vidx < eidx) {
      PT v2 = vertex(p, vidx);
      decltype(e0) e1(v1, v2);
      if (not collinear(e0, e1)) { return make_tuple(v0, v1, v2); }
      ++vidx;
    }
  }

  return {};
}

struct is_planar_fn {
  /// Is a 2D polygon planar? Always.
  template <typename P, typename UP = uncvref_t<P>,
            dim_t Ad = UP::ambient_dimension(),
            CONCEPT_REQUIRES_(Polygon<UP, 2>{})>
  constexpr bool operator()(P&&, num_t, num_t) const noexcept {
    return true;  // a 2D polygon is always planar
  }

  /// Is a 3D polygon planar? If all its vertices lie on the same plane.
  template <typename P, typename UP = uncvref_t<P>,
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  constexpr bool operator()(P&& p, num_t abs_tol, num_t rel_tol) const
   noexcept {
    // Get 3 non-colinear polygon vertices:
    auto r = get_three_non_collinear_vertices(p);
    HM3_ASSERT(r, "cannot get three non-collinear vertices of the polygon {}",
               p);
    auto[v0, v1, v2] = r.value();

    // Get the plane spanned by the vertices:
    auto plane = Eigen::Hyperplane<num_t, 3>::Through(v0(), v1(), v2());

    // If all vertices of the polygon lie in this plane, the polygon is planar.
    // Otherwise, it is not planar.
    for (auto&& v : vertices(p)) {
      const num_t distance_to_plane = plane.absDistance(v());
      if (not approx(distance_to_plane, 0., abs_tol, rel_tol)) { return false; }
    }
    return true;
  }
};

}  // namespace planar_projection_detail

namespace {

constexpr auto const& is_planar = static_const<
 with_default_tolerance<planar_projection_detail::is_planar_fn>>::value;

}  // namespace

namespace planar_projection_detail {

// Basis: origin + 2 orthogonal vectors
struct basis_3d_t {
  point<3> o;
  vec<3> e[2];
};

struct planar_basis_fn {
  /// Returns a planar basis for the polygon \p p, that is, two orthogonal
  /// vectors in the polygon's plane.
  template <typename P, typename UP = uncvref_t<P>,
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  constexpr auto operator()(P&& p, num_t abs_tol, num_t rel_tol) const noexcept
   -> basis_3d_t {
    HM3_ASSERT(is_planar(p, abs_tol, rel_tol), "polygon {} is not planar", p);

    // Get 3 non-colinear polygon vertices:
    auto r = get_three_non_collinear_vertices(p);
    HM3_ASSERT(r, "cannot get three non-collinear vertices of the polygon {}",
               p);
    auto[v0, vx, vy] = r.value();

    // Define the x-axis
    auto x_axis = (vx() - v0()).normalized().eval();
    // The y-axis is orthogonal to the x-axis and the normal:
    auto y_axis = x_axis
                   .cross(  // normal vector:
                    x_axis.cross(vy() - v0()))
                   .normalized()
                   .eval();

    return basis_3d_t{point<3>(v0()), {vec<3>(x_axis), vec<3>(y_axis)}};
  }
};

}  // namespace planar_projection_detail

namespace {

constexpr auto const& planar_basis = static_const<
 with_default_tolerance<planar_projection_detail::planar_basis_fn>>::value;

}  // namespace

namespace planar_projection_detail {

struct planar_projection_fn {
  /// Planar coordinates of the point \p p.
  template <typename P, typename UP = uncvref_t<P>,
            CONCEPT_REQUIRES_(Point<UP, 3>{})>
  static constexpr auto to(P const& p, basis_3d_t const& b) noexcept -> UP {
    auto lp  = UP::constant(0.);
    auto tmp = (p() - b.o()).eval();
    lp(0)    = (tmp).dot(b.e[0]());
    lp(1)    = (tmp).dot(b.e[1]());
    return UP{lp};
  }

  /// Planar coordinates of the polygon \p p.
  template <typename P, typename UP = uncvref_t<P>,
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  static constexpr auto to(P&& p, basis_3d_t const& b) noexcept -> UP {
    HM3_ASSERT(is_planar(p), "polygon {} is not planar", p);
    // TODO: maybe think of an API for MutablePolygon/MutablePolyline that
    // allows mutating vertices in place?
    UP lp(edges(p) | view::transform([&](auto&& e) {
            e.x(0) = to(e.x(0), b);
            e.x(1) = to(e.x(1), b);
            return e;
          }));
    return lp;
  }

  /// Planar coordinates of the polygon \p p.
  template <typename P, typename UP = uncvref_t<P>,
            CONCEPT_REQUIRES_(Polygon<UP, 3>{})>
  static constexpr auto to(P&& p) noexcept -> UP {
    return to(std::forward<P>(p), planar_basis(p));
  }

  /// From point in non-planar coordinates of the polygon \p p and base \p b
  /// back to 3D space.
  template <typename P, typename UP = uncvref_t<P>,
            CONCEPT_REQUIRES_(Point<UP, 3>{})>
  static constexpr auto from(P&& p, basis_3d_t const& b) noexcept -> UP {
    return UP(b.o() + b.e[0]() * p(0) + b.e[1]() * p(1));
  }
};

}  // namespace planar_projection_detail

namespace {
constexpr auto const& planar_projection
 = static_const<planar_projection_detail::planar_projection_fn>::value;
}  // namespace

}  // namespace hm3::geometry
