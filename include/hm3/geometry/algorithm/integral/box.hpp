#pragma once
/// \file
///
/// Integral of a Box.
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace integral_box_detail {

struct integral_box_fn {
  /// Volume integral of the box \p s.
  template <typename T>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::volume_integral<T>) const {
    constexpr dim_t ad = ad_v<T>;
    static_assert(Same<associated::t_<T>, trait::box<ad>>{});
    return math::ipow(bounding_length.max(s), ad);
  }

  /// Boundary integral of the box \p s.
  template <typename T>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::boundary_integral<T>) const {
    constexpr auto ad = ad_v<T>;
    static_assert(Same<associated::t_<T>, trait::box<ad>>{});
    switch (ad) {
      case 1: {
        return 2.;
      }
      case 2: {
        return 4. * bounding_length.max(s);
      }
      case 3: {
        return 6. * math::ipow(bounding_length.max(s), 2);
      }
      default: { HM3_UNREACHABLE(); }
    }
  }
};

}  // namespace integral_box_detail

namespace {
constexpr auto const& integral_box
 = static_const<integral_box_detail::integral_box_fn>::value;
}  // namespace

}  // namespace hm3::geometry
