#pragma once
/// \file
///
/// Intersection of line segments with signed distance fields.
#include <hm3/ext/variant.hpp>
#include <hm3/geometry/algorithm/sd_relative_position.hpp>
#include <hm3/geometry/algorithm/sd_sign.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/concept/signed_distance.hpp>
#include <hm3/math/interpolation/linear.hpp>

namespace hm3::geometry::sd {

namespace intersection_segment_detail {

struct intersection_segment_fn {
  /// Intersection of the line segment \p s with the zero-th level of the signed
  /// distance field \p sdf .
  template <typename S, typename SD>
  constexpr auto operator()(S const& s, SD&& sdf, num_t abs_tol,
                            num_t rel_tol) const noexcept
   -> variant<monostate, associated::point_t<S>, S> {
    using p_t = associated::point_t<S>;
    static_assert(SignedDistance<SD, p_t>{});

    array<num_t, 2> sdv{{sdf(s.x(0)), sdf(s.x(1))}};
    array<math::signum_t, 2> sgv{
     {sign(sdv[0], abs_tol, rel_tol), sign(sdv[1], abs_tol, rel_tol)}};

    switch (relative_position(sgv)) {
      case intersected: {
        if (sgv[0] == sgv[1]) {
          HM3_ASSERT(sgv[0] == math::signum_t::zero(), "");
          return s;
        }
        return math::ip::linear::point_with_value(0., s, sdv);
      }
      case inside:
      case outside: {
        return monostate{};
      }
    }
  }
};

}  // namespace intersection_segment_detail

namespace {
static constexpr auto const& intersection_segment
 = static_const<with_default_tolerance<
  intersection_segment_detail::intersection_segment_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd
