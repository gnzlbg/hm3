#pragma once
/// \file
///
/// Is a polygon convex?
#include <hm3/geometry/primitive/polygon/planar.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

namespace convex_detail {

struct convex_fn {
  template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
  constexpr bool operator()(P&& p) const noexcept {
    HM3_ASSERT(planar.is(p), "polygon {} is not planar", p);

    auto no_faces = face_size(p);

    auto compute_angle = [&](auto&& f0, auto&& f1) {
      return std::acos(
       direction(f0)().normalized().dot(direction(f1)().normalized()));
    };

    for (dim_t f = 0; f < no_faces - 1; ++f) {
      auto f0 = face(p, f);
      auto f1 = face(p, (f == no_faces - 1) ? 0 : f + 1);
      if (std::abs(compute_angle(f0, f1)) > math::pi) { return false; }
    }
    return true;
  }
};

}  // namespace convex_detail

namespace {
constexpr auto const& convex = static_const<convex_detail::convex_fn>::value;
}  // namespace

}  // polygon_primitive

using polygon_primitive::convex;

}  // namespace hm3::geometry
