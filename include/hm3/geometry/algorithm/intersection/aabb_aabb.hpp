#pragma once
/// \file
///
/// Intersection of Axis-Aligned Bounding Boxes
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::aabb_primitive {

/// Returns whether the two AABBs \p a and \p b intersect.
template <dim_t Nd>
constexpr bool intersection_test(aabb<Nd> const& a,
                                 aabb<Nd> const& b) noexcept {
  return (x_min(a)().array() <= x_max(b)().array()).all()
         and (x_max(a)().array() >= x_min(b)().array()).all();
}

/// 1D intersection between two bounding boxes \p a and \p b.
inline variant<monostate, point<1>, aabb<1>> intersection(
 aabb<1> const& a, aabb<1> const& b) noexcept {
  using p_t   = point<1>;
  auto result = segment_primitive::segment_intersection_detail::intersection_1d(
   x_min(a)(0), x_max(a)(0), x_min(b)(0), x_max(b)(0));
  if (!result) { return monostate{}; }

  auto r  = result.value();
  auto x0 = p_t{r.first()};
  auto x1 = p_t{r.second()};
  if (x0 == x1) { return x0; }
  return aabb<1>(x0, x1);
}

/// 2D intersection between two bounding boxes \p a and \p b.
inline variant<monostate, point<2>, segment<2>, aabb<2>> intersection(
 aabb<2> const& a, aabb<2> const& b) noexcept {
  static constexpr dim_t Nd = 2;
  using p_t                 = point<Nd>;
  using s_t                 = segment<Nd>;

  if (!geometry::intersection.test(a, b)) { return monostate{}; }

  p_t x0
   = (x_min(a)().array() <= x_min(b)().array()).all() ? x_min(b) : x_min(a);
  p_t x1
   = (x_max(a)().array() >= x_max(b)().array()).all() ? x_max(b) : x_max(a);

  bool equal_components[Nd];

  for (dim_t d = 0; d < Nd; ++d) {
    equal_components[d] = geometry::approx(x0(d), x1(d));
  }

  suint_t no_equal_components = ranges::count(equal_components, true);

  // all equal => corner point:
  if (no_equal_components == Nd) { return x0; }
  // Nd - 1 equal => face (2D: segment)
  if (no_equal_components == 1) { return s_t(x0, x1); }

  // otherwise => aabb:
  return aabb<Nd>(x0, x1);
}

/// 3D intersection between two bounding boxes \p a and \p b.
///
/// The points of the quad are ordered such that the quad normal points in the
/// positive Cartesian direction perpendicular to the quad plane.
inline variant<monostate, point<3>, segment<3>, aabb<3>, quad<3>> intersection(
 aabb<3> const& a, aabb<3> const& b) noexcept {
  static constexpr dim_t Nd = 3;

  using p_t = point<Nd>;
  using s_t = segment<Nd>;

  if (!geometry::intersection.test(a, b)) { return monostate{}; }

  p_t x0, x1;
  for (dim_t d = 0; d < Nd; ++d) {
    x0(d) = x_min(a)(d) <= x_min(b)(d) ? x_min(b)(d) : x_min(a)(d);
    x1(d) = x_max(a)(d) >= x_max(b)(d) ? x_max(b)(d) : x_max(a)(d);
  }

  bool equal_components[Nd];

  for (dim_t d = 0; d < Nd; ++d) {
    equal_components[d] = geometry::approx(x0(d), x1(d));
  }

  suint_t no_equal_components = ranges::count(equal_components, true);

  // all equal => corner point:
  if (no_equal_components == Nd) { return x0; }
  // Nd - 1 = 2 equal => edge (3D: segment)
  if (no_equal_components == Nd - 1) { return s_t(x0, x1); }

  // Nd - 2 = 1 equal => face (3D: quad)
  if (no_equal_components == Nd - 2) {
    // find non-equal component
    dim_t c = ranges::find(equal_components, true) - begin(equal_components);
    HM3_ASSERT(c < 3, "c: {}", c);
    // creates an axis-aligned quad, spanned by x0 and x1, along the axis c.
    return make_aa_quad(x0, x1, c);
  }

  // otherwise => aabb:
  return aabb<Nd>(x0, x1);
}

}  // namespace hm3::geometry::aabb_primitive
