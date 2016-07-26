#pragma once
/// \file
///
/// Axis-Aligned Bounding Box
#include <hm3/geometry/dimension.hpp>
#include <hm3/geometry/discrete/primitive/point.hpp>
#include <hm3/geometry/discrete/primitive/vector.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {

namespace aabb_primitive {

/// Axis-Aligned Bounding Box.
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>  //
struct aabb : ranked<Nd, Nd> {
  using self              = aabb<Nd>;
  using point_t           = point<Nd>;
  using vector_t          = vector<Nd>;
  using vertex_index_type = dim_t;

  array<point_t, 2> x;

  constexpr aabb()            = default;
  constexpr aabb(aabb const&) = default;
  constexpr aabb(aabb&&)      = default;
  constexpr aabb& operator=(aabb const&) = default;
  constexpr aabb& operator=(aabb&&) = default;

  /// AABB from \p x_min and \p x_max.
  constexpr aabb(point_t const& x_min, point_t const& x_max)
   : x{{x_min, x_max}} {
    assert_valid();
  }

  /// AABB from centroid and lengths.
  static constexpr self at(point_t const& x_c, vector_t const& lengths) {
    for (dim_t d = 0; d < Nd; ++d) {
      HM3_ASSERT(lengths(d) > 0., "aabb length is {} !> 0!", lengths(d));
    }
    return self(point_t{x_c() - .5 * lengths()},
                point_t{x_c() + .5 * lengths()});
  }

  /// Unit AABB
  static constexpr self unit() noexcept {
    return self{point_t::constant(0.), point_t::constant(1.)};
  }

  void assert_valid() const noexcept {
    HM3_ASSERT(x[0] != x[1], "aabb min/max bounds are equal: {}", x[0]);
    HM3_ASSERT((x[0]().array() <= x[1]().array()).all(),
               "min !<= max | min {} | max {}", x[0], x[1]);
  }
};

template <dim_t Nd>
constexpr bool operator==(aabb<Nd> const& l, aabb<Nd> const& r) noexcept {
  return l.x[0] == r.x[0] and l.x[1] == r.x[1];
}

template <dim_t Nd>
constexpr bool operator!=(aabb<Nd> const& l, aabb<Nd> const& r) noexcept {
  return !(l == r);
}

/// Minimum vertex
template <typename T, typename UT = uncvref_t<T>, dim_t Nd = UT::dimension(),
          typename Ret
          = std::conditional_t<std::is_rvalue_reference<T>{}, point<Nd>,
                               decltype(std::declval<T>().x[0])>,
          CONCEPT_REQUIRES_(Same<UT, aabb<Nd>>{})>
constexpr Ret x_min(T&& s) noexcept {
  return s.x[0];
}

/// Maximum vertex
template <typename T, typename UT = uncvref_t<T>, dim_t Nd = UT::dimension(),
          typename Ret
          = std::conditional_t<std::is_rvalue_reference<T>{}, point<Nd>,
                               decltype(std::declval<T>().x[0])>,
          CONCEPT_REQUIRES_(Same<UT, aabb<Nd>>{})>
constexpr Ret x_max(T&& s) noexcept {
  return s.x[1];
}

}  // namespace aabb_primitive

using aabb_primitive::aabb;

}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
