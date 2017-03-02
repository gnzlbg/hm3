#pragma once
/// \file
///
/// Intersection of Axis-Aligned Bounding Boxes
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/algorithm/make_aa_quad.hpp>
#include <hm3/geometry/concept/aabb.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_aabb_aabb_detail {

struct intersection_test_aabb_aabb_fn {
  /// Returns whether the two AABBs \p a and \p b intersect.
  template <typename T, typename U>
  constexpr bool operator()(T&& a, U&& b) const noexcept {
    static_assert(AABB<T>{});
    static_assert(AABB<U>{});
    static_assert(ad_v<T> == ad_v<U>);
    return (x_min(a)().array() <= x_max(b)().array()).all()
           and (x_max(a)().array() >= x_min(b)().array()).all();
  }
};

}  // namespace namespace intersection_test_aabb_aabb_detail

namespace {
static constexpr auto const& intersection_test_aabb_aabb = static_const<
 intersection_test_aabb_aabb_detail::intersection_test_aabb_aabb_fn>::value;
}

namespace intersection_aabb_aabb_detail {

struct intersection_aabb_aabb_fn {
  /// 1D intersection between two bounding boxes \p a and \p b.
  template <typename T, typename U, typename PT = associated::point_t<T>>
  static constexpr auto impl(T&& a, U&& b, trait::aabb<1>) noexcept
   -> variant<monostate, PT, uncvref_t<T>> {
    static_assert(ad_v<T> == 1);

    auto result = intersection_test_segment_segment_detail::intersection_1d(
     x_min(a)(0), x_max(a)(0), x_min(b)(0), x_max(b)(0));
    if (!result) { return monostate{}; }
    auto r  = result.value();
    auto x0 = PT{first(r)};
    auto x1 = PT{second(r)};
    if (x0 == x1) { return x0; }
    return uncvref_t<T>(x0, x1);
  }

  /// 2D intersection between two bounding boxes \p a and \p b.
  template <typename T, typename U, typename PT = associated::point_t<T>,
            typename ET = associated::edge_t<T>>
  static constexpr auto impl(T&& a, U&& b, trait::aabb<2>) noexcept
   -> variant<monostate, PT, ET, uncvref_t<T>> {
    constexpr auto ad = ad_v<T>;
    static_assert(ad == 2);

    if (!intersection_test_aabb_aabb(a, b)) { return monostate{}; }

    PT x0
     = (x_min(a)().array() <= x_min(b)().array()).all() ? x_min(b) : x_min(a);
    PT x1
     = (x_max(a)().array() >= x_max(b)().array()).all() ? x_max(b) : x_max(a);

    bool equal_components[ad] = {};

    for (dim_t d = 0; d < ad; ++d) {
      equal_components[d] = approx_number(x0(d), x1(d));
    }

    suint_t no_equal_components = ranges::count(equal_components, true);

    // all equal => corner point:
    if (no_equal_components == ad) { return x0; }
    // Ad - 1 equal => face (2D: segment)
    if (no_equal_components == 1) { return ET(x0, x1); }

    // otherwise => aabb:
    return uncvref_t<T>(x0, x1);
  }

  /// 3D intersection between two bounding boxes \p a and \p b.
  ///
  /// The points of the quad are ordered such that the quad normal points in the
  /// positive Cartesian direction perpendicular to the quad plane.
  template <typename T, typename U, typename PT = associated::point_t<T>,
            typename ET = associated::edge_t<T>,
            typename QT = associated::fixed_polygon_t<T, 4>>
  static constexpr auto impl(T&& a, U&& b, trait::aabb<3>) noexcept
   -> variant<monostate, PT, ET, uncvref_t<T>, QT> {
    constexpr dim_t ad = ad_v<T>;
    static_assert(ad == 3);

    if (!intersection_test_aabb_aabb(a, b)) { return monostate{}; }

    PT x0, x1;
    for (dim_t d = 0; d < ad; ++d) {
      x0(d) = x_min(a)(d) <= x_min(b)(d) ? x_min(b)(d) : x_min(a)(d);
      x1(d) = x_max(a)(d) >= x_max(b)(d) ? x_max(b)(d) : x_max(a)(d);
    }

    bool equal_components[ad] = {};

    for (dim_t d = 0; d < ad; ++d) {
      equal_components[d] = approx_number(x0(d), x1(d));
    }

    suint_t no_equal_components = ranges::count(equal_components, true);

    // all equal => corner point:
    if (no_equal_components == ad) { return x0; }
    // ad - 1 = 2 equal => edge (3D: segment)
    if (no_equal_components == ad - 1) { return ET(x0, x1); }

    // ad - 2 = 1 equal => face (3D: quad)
    if (no_equal_components == ad - 2) {
      // find non-equal component
      dim_t c = ranges::find(equal_components, true) - begin(equal_components);
      HM3_ASSERT(c < 3, "c: {}", c);
      // creates an axis-aligned quad, spanned by x0 and x1, along the axis c.
      return make_aa_quad<QT>(x0, x1, c);
    }

    // otherwise => aabb:
    return uncvref_t<T>(x0, x1);
  }

  template <typename T, typename U>
  constexpr bool operator()(T&& a, U&& b) const noexcept {
    static_assert(AABB<T>{});
    static_assert(AABB<U>{});
    static_assert(ad_v<T> == ad_v<U>);
    static_assert(Same<uncvref_t<T>, uncvref_t<U>>{});
    return impl(std::forward<T>(a), std::forward<U>(b), associated::v_<T>);
  }
};

}  // namespace intersection_aabb_aabb_detail

namespace {
static constexpr auto const& intersection_aabb_aabb = static_const<
 intersection_aabb_aabb_detail::intersection_aabb_aabb_fn>::value;
}

}  // namespace hm3::geometry::aabb_primitive
