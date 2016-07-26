#pragma once
/// \file
///
/// Intersection of Axis-Aligned Bounding Boxes
#include <hm3/geometry/discrete/algorithm/intersection.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace aabb_primitive {

/// Returns whether the intersection between two AABBs is not empty.
template <dim_t Nd>
constexpr bool non_empty_intersection(aabb<Nd> const& a,
                                      aabb<Nd> const& b) noexcept {
  auto const& a_min = x_min(a);
  auto const& a_max = x_max(a);
  auto const& b_min = x_min(b);
  auto const& b_max = x_max(b);
  return (a_min().array() <= b_max().array()).all()
         and (a_max().array() >= b_min().array()).all();
}

/// Returns whether the intersection between two AABBs is empty.
template <dim_t Nd>
constexpr bool empty_intersection(aabb<Nd> const& a,
                                  aabb<Nd> const& b) noexcept {
  return !non_empty_intersection(a, b);
}

/// Computes the intersection between two AABBs.
///
/// There are 3 cases, given two AABB's a and b:
///
/// Case A: a and b don't intersect => {} (empty set)
///
/// Case B: one of them _fully_ contains the other:
/// - Case B1: b contains a => {a} (aabb)
/// - Case B2: a contains b => {b} (aabb)
/// - Case B3: a contains a => {a} (aabb)
///
/// Case C: a and b partially overlapp along a
/// - Case C1: volume => 3D(aabb), 2D(aabb), 1D(aabb)
/// - Case C2: face   => 3D(square), 2D(segment), 1D(point)
/// - Case C3: edge   => 3D(segment), 2D(point)
/// - Case C4: corner => 3D(point)
///
/*
template <dim_t Nd>  //
struct result_t {
  enum type_t { empty = 0, contained = 1, overlap = 2 };

  using intersection_t = std::
   conditional_t<Nd == 1, variant<aabb<Nd>, point<Nd>>,
                 std::conditional_t<Nd == 2,
                                    variant<aabb<Nd>, segment<Nd>, point<Nd>>,
                                    std::conditional_t<Nd == 3,
                                                       variant<aabb<Nd>,
                                                               segment<Nd>,
                                                               polygon<Nd, 4>,
                                                               point<Nd>>,
                                                       void>>>;

  type_t type_ = type_t::empty;
  intersection_t intersection_{};

  type_t type() const noexcept { return type_; }
  intersection_t intersection() const noexcept {
    HM3_ASSERT(type() != empty, "");
    return intersection_;
  }
};
*/

}  // namespace aabb_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
