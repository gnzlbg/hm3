#pragma once
/// \file
///
/// Intersection of two signed-distance fields.
#include <hm3/geometry/sd/core.hpp>

namespace hm3::geometry::sd {

namespace detail {
struct op_intersection_fn {
  /// Intersection of a signed distance field: AND(f1, f2)
  template <typename Point, typename SDF>
  constexpr auto operator()(Point&& x, SDF&& f) const {
    return f(std::forward<Point>(x));
  }

  /// Intersection of a signed distance field: AND(f1, f2)
  template <typename Point, typename SDF1, typename... SDFs>
  constexpr auto operator()(Point&& x, SDF1&& f1, SDFs&&... fs) const {
    return std::max((*this)(x, f1), (*this)(x, fs...));
  }
};
}  // namespace detail

namespace {
constexpr auto const& op_intersection
 = static_const<detail::op_intersection_fn>::value;
}  // namespace

namespace detail {
struct pos_intersection_fn {
  template <typename... SDFs>
  constexpr auto operator()(SDFs&&... fs) const {
    return view(op_intersection, std::forward<SDFs>(fs)...);
  }
};
}  // namespace detail

namespace {
constexpr auto const& pos_intersection
 = static_const<detail::pos_intersection_fn>::value;
}  // namespace

}  // namespace hm3::geometry::sd
