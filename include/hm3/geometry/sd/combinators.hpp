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
struct op_inverse_t {
  template <typename Point, typename SDFunction>
  constexpr auto operator()(Point&& x, SDFunction&& f) const {
    return -f(x);
  }
};
}  // namespace detail

constexpr auto&& op_inverse = static_const<detail::op_inverse_t>::value;

namespace detail {
struct neg_t {
  template <typename SDFunction>
  constexpr auto operator()(SDFunction&& f) const {
    return adapt(op_inverse, std::forward<SDFunction>(f));
  }
};
}  // namespace detail

constexpr auto&& neg = static_const<detail::neg_t>::value;

namespace detail {
struct op_union_t {
  template <typename Point, typename SDFunction1, typename SDFunction2>
  constexpr auto operator()(Point&& x, SDFunction1&& f1,
                            SDFunction2&& f2) const {
    return std::min(f1(x), f2(x));
  }

  /// Union of a signed distance field: OR(f1, f2)
  template <typename Point, typename SDFunction1, typename... SDFunctions>
  constexpr auto operator()(Point&& x, SDFunction1&& f1,
                            SDFunctions&&... fs) const {
    return (*this)(f1, op_union(x, fs...));
  }
};
}  // namespace detail

constexpr auto&& op_union = static_const<detail::op_union_t>::value;

namespace detail {
/// Union of a signed distance field: OR(f1, f2)
///
/// \note this is the union of the "negative regions" of the signed distance
/// field
struct add_t {
  template <typename... SDFunctions>
  constexpr auto operator()(SDFunctions&&... fs) const {
    return adapt(op_union, std::forward<SDFunctions>(fs)...);
  }
};
}  // namespace detail

constexpr auto&& add = static_const<detail::add_t>::value;

namespace detail {
struct op_union_plus_t {
  template <typename Point, typename SDFunction1, typename SDFunction2>
  constexpr auto operator()(Point&& x, SDFunction1&& f1,
                            SDFunction2&& f2) const {
    return -std::max(-f1(x), -f2(x));
  }

  /// Union of a signed distance field: OR(f1, f2)
  template <typename Point, typename SDFunction1, typename... SDFunctions>
  constexpr auto operator()(Point&& x, SDFunction1&& f1,
                            SDFunctions&&... fs) const {
    return (*this)(f1, op_union_plus(x, fs...));
  }
};
}  // namespace detail

constexpr auto&& op_union_plus = static_const<detail::op_union_plus_t>::value;

namespace detail {
/// Union of a signed distance field: OR(f1, f2)
///
/// \note union of the "positive" regions of the signed distance field
struct add_plus_t {
  template <typename... SDFunctions>
  constexpr auto operator()(SDFunctions&&... fs) const {
    return adapt(op_union_plus, std::forward<SDFunctions>(fs)...);
  }
};
}  // namespace detail

constexpr auto&& add = static_const<detail::add_t>::value;

namespace detail {
struct op_intersection_t {
  /// Intersection of a signed distance field: AND(f1, f2)
  template <typename Point, typename SDFunction1, typename SDFunction2>
  constexpr auto operator()(Point&& x, SDFunction1& f1, SDFunction2& f2) const {
    return std::max(f1(x), f2(x));
  }

  /// Intersection of a signed distance field: AND(f1, f2)
  template <typename Point, typename SDFunction1, typename... SDFunctions>
  constexpr auto operator()(Point&& x, SDFunction1&& f1,
                            SDFunctions&&... fs) const {
    return (*this)(x, f1, op_intersection(x, fs...));
  }
};
}  // namespace detail

constexpr auto&& op_intersection
 = static_const<detail::op_intersection_t>::value;

namespace detail {
struct common_t {
  template <typename... SDFunctions>
  constexpr auto operator()(SDFunctions&&... fs) const {
    return adapt(op_intersection, std::forward<SDFunctions>(fs)...);
  }
};
}  // namespace detail

constexpr auto&& common = static_const<detail::common_t>::value;

namespace detail {

struct op_difference_t {
  /// Difference of a signed distance field: AND(f1, -f2)
  template <typename Point, typename SDFunction1, typename SDFunction2>
  constexpr auto operator()(Point&& x, SDFunction1& f1, SDFunction2& f2) const {
    return std::max(f1(x), -f2(x));
  }
};
}  // namespace detail

constexpr auto&& op_difference = static_const<detail::op_difference_t>::value;

namespace detail {
struct sub_t {
  template <typename SDFunction0, typename SDFunction1>
  constexpr auto operator()(SDFunction0&& f0, SDFunction1&& f1) const {
    return adapt(op_difference, std::forward<SDFunction0>(f0),
                 std::forward<SDFunction1>(f1));
  }
};
}  // namespace detail

constexpr auto&& sub = static_const<detail::sub_t>::value;

}  // namespace sd
}  // namespace geometry
}  // namespace hm3
