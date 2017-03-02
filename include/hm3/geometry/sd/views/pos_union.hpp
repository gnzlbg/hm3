#pragma once
/// \file
///
/// Positive union of two signed-distance fields.
#include <hm3/geometry/sd/core.hpp>

namespace hm3::geometry::sd {

namespace detail {
struct op_pos_union_fn {
  template <typename Point, typename SDF>
  constexpr auto operator()(Point&& x, SDF&& f) const {
    return f(std::forward<Point>(x));
  }

  /// Union of the positive  side of the signed distance fields \p f1 , \p fs...
  template <typename Point, typename SDF1, typename... SDFs>
  constexpr auto operator()(Point&& x, SDF1&& f1, SDFs&&... fs) const {
    return -std::max(-(*this)(x, std::forward<SDF1>(f1)),
                     -(*this)(x, std::forward<SDFs>(fs)...));
  }
};
}  // namespace detail

namespace {
static constexpr auto const& op_pos_union
 = static_const<detail::op_pos_union_fn>::value;
}  // namespace

namespace detail {
/// Union of a signed distance field: OR(f1, f2)
///
/// \note union of the "positive" regions of the signed distance field
struct pos_union_fn {
  template <typename... SDFs>
  constexpr auto operator()(SDFs&&... fs) const {
    return view(op_pos_union, std::forward<SDFs>(fs)...);
  }
};
}  // namespace detail

namespace {
static constexpr auto const& pos_union
 = static_const<detail::pos_union_fn>::value;
}  // namespace

}  // namespace hm3::geometry::sd
