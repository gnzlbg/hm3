#pragma once
/// \file
///
/// Negative union of two signed-distance fields.
#include <hm3/geometry/sd/core.hpp>

namespace hm3::geometry::sd {

namespace detail {
struct op_neg_union_fn {
  template <typename Point, typename SDF>
  constexpr auto operator()(Point&& x, SDF&& f) const {
    return f(std::forward<Point>(x));
  }

  /// Union of negative side of the figned distance fields \p f1 , \p fs...
  template <typename Point, typename SDF1, typename... SDFs>
  constexpr auto operator()(Point&& x, SDF1&& f1, SDFs&&... fs) const {
    return std::min((*this)(x, std::forward<SDF1>(f1)),
                    (*this)(x, std::forward<SDFs>(fs)...));
  }
};
}  // namespace detail

namespace {
constexpr auto const& op_neg_union
 = static_const<detail::op_neg_union_fn>::value;
}  // namespace

namespace detail {
/// Union of the negative sides of the signed distance fields f1, ..., fn.
struct neg_union_fn {
  template <typename... SDFs>
  constexpr auto operator()(SDFs&&... fs) const {
    return view(op_neg_union, std::forward<SDFs>(fs)...);
  }
};
}  // namespace detail

namespace {
constexpr auto const& neg_union = static_const<detail::neg_union_fn>::value;
}  // namespace

}  // namespace hm3::geometry::sd
