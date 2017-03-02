#pragma once
/// \file
///
/// Origin rotation transformation: rotates a signed-distance field around the
/// origin.
#include <hm3/geometry/sd/core.hpp>
#include <hm3/math/rotations.hpp>

namespace hm3::geometry::sd {

namespace rotation_detail {

struct rotate_around_origin_2d_fn {
  /// Rotates the signed-distance field to be evaluated on the point \p p around
  /// the origin by \p angle radians.
  template <typename P>
  constexpr auto operator()(P&& p, num_t angle) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(math::inverse_rotation_matrix_2d(angle)
                                        * std::forward<P>(p));
};

struct rotate_around_origin_3d_fn {
  /// Rotates the signed-distance field to be evaluated on the point \p p around
  /// the origin by \p angles (angle_x, angle_y, angle_z) radians.
  template <typename P, typename V>
  constexpr auto operator()(P&& p, V angles) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(math::inverse_rotation_matrix_3d(angles)
                                        * std::forward<P>(p));
};

}  // namespace rotation_detail

namespace {
static constexpr auto const& rotate_around_origin_2d
 = static_const<rotation_detail::rotate_around_origin_2d_fn>::value;
static constexpr auto const& rotate_around_origin_3d
 = static_const<rotation_detail::rotate_around_origin_3d_fn>::value;
}  // namespace

/// Adapts a Signed-Distance function with a rotation around the origin.
template <typename Sd>
struct origin_rotation : Sd {
  static_assert(ad_v<Sd>> 1);
  meta::if_c<ad_v<Sd> == 2, num_t, vec<3>> origin_rotation_angles_{};

  template <typename... Args, CONCEPT_REQUIRES_(ad_v<Sd> == 2)>
  origin_rotation(num_t angle, Args&&... args)
   : Sd(std::forward<Args>(args)...), origin_rotation_angles_(angle) {}

  template <typename V, typename... Args, CONCEPT_REQUIRES_(ad_v<Sd> == 2)>
  origin_rotation(V angles, Args&&... args)
   : Sd(std::forward<Args>(args)...)
   , origin_rotation_angles_(std::move(angles())) {
    static_assert(Vector<V>{});
  }

  void set_origin_rotation_angle(num_t angle, dim_t d = 2) noexcept {
    if
      constexpr(ad_v<Sd> == 2) {
        HM3_ASSERT(
         d == 2,
         "rotation axis {} not possible in 2D (only z-axis rotations allowed)",
         d);
        origin_rotation_angles_ = angle;
      }

    if
      constexpr(ad_v<Sd> == 3) {
        HM3_ASSERT(d < 3, "rotation axis {} out-of-bounds [0, 3)", d);
        origin_rotation_angles_(d) = angle;
      }
  }

  template <typename V>
  void set_origin_rotation_angles(V v) noexcept {
    static_assert(Vector<V, 3>{});
    if
      constexpr(ad_v<Sd> == 2) {
        HM3_ASSERT(unwrap(v)(0) == 0. and unwrap(v)(1) == 0.,
                   "In 2D only rotations around the z-axis are possible");
        origin_rotation_angles_ = unwrap(v)(2);
      }

    if
      constexpr(ad_v<Sd> == 3) { origin_rotation_angles_ = unwrap(v); }
  }

  template <typename P>
  constexpr auto operator()(P const& x) const noexcept {
    if
      constexpr(ad_v<Sd> == 2) {
        return static_cast<Sd const&>(*this)(
         invoke_unwrapped(rotate_around_origin_2d, x, origin_rotation_angles_));
      }

    if
      constexpr(ad_v<Sd> == 3) {
        return static_cast<Sd const&>(*this)(
         invoke_unwrapped(rotate_around_origin_3d, x, origin_rotation_angles_));
      }
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace hm3::geometry::sd
