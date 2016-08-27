#pragma once
/// \file
///
/// Bounding lengths of an AABB.
///
/// TODO: the abs are not required actually!
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>

namespace hm3::geometry::aabb_primitive {

/// Bounding length of the AABB \p s along the \p d spatial component.
template <dim_t Nd>
constexpr num_t bounding_length(aabb<Nd> const& s, dim_t d) noexcept {
  auto res = x_max(s)(d) - x_min(s)(d);
  HM3_ASSERT(math::approx(res, std::abs(res)), "");
  return res;
}

/// Bounding lengths of the AABB \p s along all spatial components.
template <dim_t Nd>
constexpr vec<Nd> all_bounding_lengths(aabb<Nd> const& s) noexcept {
  auto res = vec<Nd>(x_max(s)() - x_min(s)());
  HM3_ASSERT(geometry::approx(res, vec<Nd>(res().array().abs())), "");
  return res;
}

/// Max bounding length of the AABB \p s.
template <dim_t Nd>
constexpr num_t max_bounding_length(aabb<Nd> const& s) noexcept {
  auto res = (x_max(s)() - x_min(s)()).array().maxCoeff();
  HM3_ASSERT(
   math::approx(res, (x_max(s)() - x_min(s)()).array().abs().maxCoeff()), "");
  HM3_ASSERT(
   [&]() {
     for (dim_t d = 0; d < Nd; ++d) {
       HM3_ASSERT(std::abs(x_max(s)(d) - x_min(s)(d)) <= res, "d: {}");
     }
     return true;
   }(),
   "");
  return res;
}

}  // namespace hm3::geometry::aabb_primitive
