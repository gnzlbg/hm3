#pragma once
/// \file
///
/// Combine signed distance functions into new ones
///
#include <hm3/geometry/sd/adapt.hpp>
#include <hm3/types.hpp>

namespace hm3 {
namespace geometry {
namespace sd {

namespace detail {
/// Inverse of a signed distance field
struct op_inverse_fn {
  template <typename Point, typename SDF>
  constexpr auto operator()(Point&& x, SDF&& f) const {
    return -f(std::forward<Point>(x));
  }
};
}  // namespace detail

namespace {
static constexpr auto const& op_inverse
 = static_const<detail::op_inverse_fn>::value;
}  // namespace

namespace detail {
struct invert_fn {
  template <typename SDF> constexpr auto operator()(SDF&& f) const {
    return adapt(op_inverse, std::forward<SDF>(f));
  }
};
}  // namespace detail

namespace {
static constexpr auto const& invert = static_const<detail::invert_fn>::value;
}  // namespace

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
static constexpr auto const& op_neg_union
 = static_const<detail::op_neg_union_fn>::value;
}  // namespace detail

namespace detail {
/// Union of the negative sides of the signed distance fields f1, ..., fn.
struct neg_union_fn {
  template <typename... SDFs> constexpr auto operator()(SDFs&&... fs) const {
    return adapt(op_neg_union, std::forward<SDFs>(fs)...);
  }
};
}  // namespace detail

namespace {
static constexpr auto const& neg_union
 = static_const<detail::neg_union_fn>::value;
}  // namespace

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
  template <typename... SDFs> constexpr auto operator()(SDFs&&... fs) const {
    return adapt(op_pos_union, std::forward<SDFs>(fs)...);
  }
};
}  // namespace detail

namespace {
static constexpr auto const& pos_union
 = static_const<detail::pos_union_fn>::value;
}  // namespace

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
static constexpr auto const& op_intersection
 = static_const<detail::op_intersection_fn>::value;
}  // namespace

namespace detail {
struct intersection_fn {
  template <typename... SDFs> constexpr auto operator()(SDFs&&... fs) const {
    return adapt(op_intersection, std::forward<SDFs>(fs)...);
  }
};
}  // namespace detail

namespace {
static constexpr auto const& intersection
 = static_const<detail::intersection_fn>::value;
}  // namespace

namespace detail {

struct op_difference_fn {
  /// Difference of a signed distance field: AND(f1, -f2)
  template <typename Point, typename SDF1, typename SDF2>
  constexpr auto operator()(Point&& x, SDF1&& f1, SDF2&& f2) const {
    return std::max(f1(x), -f2(x));
  }
};
}  // namespace detail

namespace {
static constexpr auto const& op_difference
 = static_const<detail::op_difference_fn>::value;
}  // namespace

namespace detail {
struct difference_fn {
  template <typename SDF0, typename SDF1>
  constexpr auto operator()(SDF0&& f0, SDF1&& f1) const {
    return adapt(op_difference, std::forward<SDF0>(f0), std::forward<SDF1>(f1));
  }
};
}  // namespace detail

namespace {
static constexpr auto const& difference
 = static_const<detail::difference_fn>::value;
}  // namespace

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
