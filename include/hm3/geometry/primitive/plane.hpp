#pragma once
/// \file
///
/// Unbounded plane
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/primitive/line.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/vec.hpp>

namespace hm3::geometry {

namespace plane_primitive {

/// Plane (unbounded)
///
/// \tparam Ad Number of spatial dimensions.
template <dim_t Ad>
struct plane {
  using geometry_type     = trait::plane<Ad>;
  using point_t           = point<Ad>;
  using point_value_type  = point_t;
  using vec_t             = vec<Ad>;
  using vector_value_type = vec_t;
  using storage_t         = Eigen::Hyperplane<num_t, Ad>;
  using bounded_t
   = meta::if_c<Ad == 1, point<Ad>, meta::if_c<Ad == 2, segment<Ad>, void>>;

  storage_t plane_;

  /// Constructs a plane from the plane normal vector \p n and a point \p p in
  /// the plane.
  plane(vec_t n, point_t p) : plane_(n(), p()) {
    HM3_ASSERT(approx_number(n().norm(), 1.),
               "the plane normal vector {} is not normalized", n);
  }

  /// Constructs a plane from the plane normal vector \p n and a point \p p in
  /// the plane.
  plane(point_t p, vec_t n) : plane(n, p) {}

  // Constructs a plane in 1D space passing through one points
  CONCEPT_REQUIRES(Ad == 1)
  plane(array<point_t, Ad> ps) : plane(vec_t::unit(0), ps[0]) {}
  // Constructs a plane in 2D space passing through two points
  CONCEPT_REQUIRES(Ad == 2)
  plane(array<point_t, Ad> ps) : plane_(storage_t::Through(ps[0](), ps[1]())) {}
  // Constructs a plane in 3D space passing through three points
  CONCEPT_REQUIRES(Ad == 3)
  plane(array<point_t, Ad> ps)
   : plane_(storage_t::Through(ps[0](), ps[1](), ps[2]())) {}

  plane()             = default;
  plane(plane const&) = default;
  plane(plane&&)      = default;

  plane& operator=(plane const&) = default;
  plane& operator=(plane&&) = default;

  vec_t normal() const noexcept { return vec_t{plane_.normal()}; }
  num_t signed_distance(point_t p) const noexcept {
    return plane_.signedDistance(p());
  }
  num_t abs_distance(point_t p) const noexcept {
    return plane_.absDistance(p());
  }
  num_t distance_to_origin() const noexcept { return plane_.offset(); }

  point_t projection(point_t o) const noexcept {
    return plane_.projection(o());
  }

  auto const& operator()() const noexcept { return plane_; }
  auto& operator()() noexcept { return plane_; }

  CONCEPT_REQUIRES(Ad == 1)
  point<1> point() const noexcept { return {distance_to_origin()}; }

  CONCEPT_REQUIRES(Ad == 2)
  line<2> line() const noexcept {
    return {projection(point_t::constant(0.)), rotate_90_cw(normal())};
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/// Is the representation of the planes \p a and \p b equal?
template <dim_t Ad>
bool operator==(plane<Ad> const& a, plane<Ad> const& b) noexcept {
  return a.plane_.isApprox(b.plane_);
}

template <dim_t Ad>
bool operator!=(plane<Ad> const& a, plane<Ad> const& b) noexcept {
  return !(a == b);
}

}  // namespace plane_primitive

using plane_primitive::plane;

}  // namespace hm3::geometry
