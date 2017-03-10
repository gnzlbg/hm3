#pragma once
/// \file
///
/// Implicit signed-distance functions for planes
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/sd/transformations/origin_rotation.hpp>
#include <hm3/geometry/sd/transformations/translation.hpp>

namespace hm3::geometry::sd {

namespace plane_detail {

struct plane_fn {
  /// Signed distance function for an plane at the origin with normal vector \p
  /// n. The signed distance is positive in positive normal direction.
  template <typename P, typename V>
  constexpr num_t operator()(P&& x, V&& n) const noexcept {
    HM3_ASSERT(approx_number(n.norm(), 1.), "vector {} is not normalized", n);
    return n.dot(x);
  }
};

}  // namespace plane_detail

namespace {
constexpr auto const& plane = static_const<plane_detail::plane_fn>::value;
}

/// Oriented plane at point centroid with normal vec. Signed distance is
/// positive in positive normal direction.
template <dim_t Ad>
struct plane_at_origin : signed_distance<Ad> {
  vec<Ad> normal = vec<Ad>::unit(0);

  constexpr plane_at_origin() = default;

  template <typename V>
  constexpr plane_at_origin(V normal_) noexcept : normal(std::move(normal_())) {
    static_assert(Vector<V>{});
    HM3_ASSERT(approx_number(normal().norm(), 1.),
               "vector {} is not normalized", normal);
  }

  template <typename P>
  constexpr auto operator()(P const& x) const noexcept {
    return invoke_unwrapped(plane, x, normal);
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Ad>
using plane_at_point = translation<plane_at_origin<Ad>>;

template <dim_t Ad>
using rotated_plane_at_point
 = translation<origin_rotation<plane_at_origin<Ad>>>;

}  // namespace hm3::geometry::sd
