#pragma once
/// \file
///
/// Box: AABB with constant length.
/// TODO: remove at method
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/aabb/bounding_length.hpp>
#include <hm3/geometry/primitive/aabb/is_box.hpp>
#include <hm3/geometry/primitive/aabb/ostream.hpp>
#include <hm3/geometry/primitive/point/point.hpp>
#include <hm3/geometry/primitive/vec.hpp>

namespace hm3::geometry {

/// Box Primitive.
namespace box_primitive {

/// Box: AABB with constant length.
template <dim_t Nd>
struct box : ranked<Nd, Nd> {
  using point_t           = point<Nd>;
  using vec_t             = vec<Nd>;
  using aabb_t            = aabb<Nd>;
  using vertex_index_type = dim_t;

  point_t centroid_;
  num_t length_;

  box()                     = default;
  constexpr box(box const&) = default;
  constexpr box(box&&)      = default;
  constexpr box& operator=(box const&) = default;
  constexpr box& operator=(box&&) = default;

  /// Box from \p x_centroid and \p x_length.
  constexpr box(point_t x_centroid, num_t length)
   : centroid_(x_centroid), length_(length) {
    HM3_ASSERT(length > 0., "box length is {} !> 0.!", length);
    HM3_ASSERT(x_min(*this) == point_t(x_centroid().array() - 0.5 * length),
               "");
    HM3_ASSERT(x_max(*this) == point_t(x_centroid().array() + 0.5 * length),
               "");
  }

  /// Box from AABB \p b that is already a box (e.g. with all equal sides).
  constexpr box(aabb_t const& b) : box(centroid(b), bounding_length(b, 0)) {
    HM3_ASSERT(is_box(b), "AABB not a Box: {}", b);
  }

  /// Box from \p x_min and \p x_max.
  constexpr box(point_t const& x_min, point_t const& x_max)
   : box(aabb_t(x_min, x_max)) {
    HM3_ASSERT(is_box(*this),
               "the points min: {} and max: {} do not span a box", x_min,
               x_max);
  }

 private:
  /// Box from centroid \p x_c and \p lengths.
  static constexpr box<Nd> from_centroid_and_lengths_(point_t const& x_c,
                                                      vec_t const& lengths) {
    return box<Nd>(aabb_t(x_c, lengths));
  }

 public:
  /// Box from centroid \p x_c and \p lengths.
  constexpr box(point_t const& x_c, vec_t const& lengths)
   : box(from_centroid_and_lengths_(x_c, lengths)) {}

  /// Unit box.
  static constexpr box<Nd> unit() noexcept {
    return box<Nd>(point_t::constant(0.5), 1.);
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Nd>
bool operator==(box<Nd> const& l, box<Nd> const& r) noexcept {
  return math::approx(l.length_, r.length_) && l.centroid_ == r.centroid_;
}

template <dim_t Nd>
bool operator!=(box<Nd> const& l, box<Nd> const& r) noexcept {
  return !(l == r);
}

/// Minimum vertex
template <dim_t Nd>
constexpr point<Nd> x_min(box<Nd> const& s) noexcept {
  const auto xc = centroid(s);
  return point<Nd>{xc() - point<Nd>::c(.5 * s.length_)};
}

/// Maximum vertex
template <dim_t Nd>
constexpr point<Nd> x_max(box<Nd> const& s) noexcept {
  const auto xc = centroid(s);
  return point<Nd>{xc() + point<Nd>::c(.5 * s.length_)};
}

template <dim_t Nd>
constexpr bool is_box(box<Nd> const&) noexcept {
  return true;
}

}  // namespace box_primitive

using box_primitive::box;

}  // namespace hm3::geometry
