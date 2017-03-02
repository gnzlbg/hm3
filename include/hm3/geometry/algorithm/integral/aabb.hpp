#pragma once
/// \file
///
/// Integral of a Aabb.
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {
namespace integral_aabb_detail {

struct integral_aabb_fn {
  /// Volume integral of the aabb \p s.
  template <typename T>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::volume_integral<T>) const {
    constexpr auto ad = ad_v<T>;
    static_assert(Same<associated::t_<T>, trait::aabb<ad>>{});
    num_t vol    = 1;
    const auto l = bounding_length.all(s);
    for (dim_t d = 1; d < ad; ++d) { vol *= l(d); }
    return vol;
  }

  /// Boundary integral of the aabb \p s.
  template <typename T>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::boundary_integral<T>) const {
    constexpr auto ad = ad_v<T>;
    static_assert(Same<associated::t_<T>, trait::aabb<ad>>{});
    switch (ad) {
      case 1: {
        return 2.;  // no_faces * area of faces == 2 * 1
      }
      case 2: {
        const auto ls = bounding_length.all(s);
        return 2. * (ls(0) + ls(1));
      }
      case 3: {
        const auto ls   = bounding_length.all(s);
        const auto lxly = ls(0) * ls(1);
        const auto lylz = ls(1) * ls(2);
        const auto lxlz = ls(0) * ls(2);
        return 2. * (lxly + lylz + lxlz);
      }
      default: { HM3_UNREACHABLE(); }
    }
  }
};

}  // namespace integral_aabb_detail

namespace {
static constexpr auto const& integral_aabb
 = static_const<integral_aabb_detail::integral_aabb_fn>::value;
}  // namespace

}  // namespace hm3::geometry
