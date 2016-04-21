#pragma once
/// \file
///
/// Square type
#include <hm3/geometry/dimension/dimensional.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/geometry/rectangle/bounds.hpp>
#include <hm3/geometry/rectangle/concept.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/config/fatal_error.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

/// Is the box spanned by \p x_min and \p x_max square?
template <dim_t Nd>
constexpr bool is_square(point<Nd> const& x_min, point<Nd> const& x_max) {
  auto lengths = (x_max() - x_min()).eval();
  HM3_ASSERT(
   [=]() {
     for (dim_t d = 0; d < Nd; ++d) {
       HM3_ASSERT(lengths(d) > 0,
                  "Negative length! Box's length '{}' < 0 along dimension '{}'",
                  lengths(d), d);
     }
     return true;
   }(),
   "negative length");
  return all_of(lengths, [&](auto i) { return math::approx(i, lengths(0)); });
}

/// Centroid coordinates of square the box spanned by \p x_min and \p x_max
template <dim_t Nd>
constexpr point<Nd> square_centroid(point<Nd> const& x_min,
                                    point<Nd> const& x_max) {
  HM3_ASSERT(is_square(x_min, x_max),
             "shape spanned by points min: {} and max: {} is not a square!",
             x_min, x_max);
  return (x_min() + (x_max() - x_min()) * 0.5).eval();
}

//// Length of the square box spanned by \p x_min and \p x_max
template <dim_t Nd>
constexpr num_t square_length(point<Nd> const& x_min, point<Nd> const& x_max) {
  HM3_ASSERT(is_square(x_min, x_max),
             "shape spanned by points min: {} and max: {} is not a square!",
             x_min, x_max);
  return x_max(0) - x_min(0);
}

/// Square
template <dim_t Nd>  //
struct square : dimensional<Nd> {
  using point_t = point<Nd>;

  point_t x_centroid_;
  num_t length_;

  square()                        = default;
  constexpr square(square const&) = default;
  constexpr square(square&&)      = default;
  constexpr square& operator=(square const&) = default;
  constexpr square& operator=(square&&) = default;

  /// Square from \p x_min and \p x_max.
  constexpr square(point_t const& x_min, point_t const& x_max)
   : x_centroid_(square_centroid(x_min, x_max))
   , length_(square_length(x_min, x_max)) {}

  /// Square from \p x_centroid and \p x_length.
  constexpr square(point_t x_centroid, num_t length)
   : x_centroid_(std::move(x_centroid)), length_(length) {}

  /// Number of m-dimensional edges
  static constexpr auto no_edges(dim_t m) noexcept -> dim_t {
    if (Nd < m || m < 0) { return 0; }
    return math::ipow(dim_t{2}, Nd - m) * math::binomial_coefficient(Nd, m);
  }

  /// Number of corners (one-dimensional edges)
  static constexpr ppidx_t no_corners() noexcept {
    return math::ipow(dim_t{2}, Nd);
  }
};

/// Square length
template <dim_t Nd>
constexpr num_t length(square<Nd> const& s, dim_t = 0) noexcept {
  return s.length_;
}

/// Square lengths (they all equal the same value)
template <dim_t Nd> constexpr point<Nd> lengths(square<Nd> const& s) noexcept {
  return point<Nd>::constant(length(s));
}

namespace concepts {

/// A 3D square is not a polygon but a polyhedra:
template <> struct is_polygon<::hm3::geometry::square<3>> : std::false_type {};

/// Squares are not only rectangles but also squares:
template <dim_t Nd>
struct is_square<::hm3::geometry::square<Nd>> : std::true_type {};

}  // namespace concepts

template <dim_t Nd>
bool operator==(square<Nd> const& l, square<Nd> const& r) noexcept {
  return math::approx(length(l), length(r)) && centroid(l) == centroid(r);
}

template <dim_t Nd>
bool operator!=(square<Nd> const& l, square<Nd> const& r) noexcept {
  return !(l == r);
}

template <dim_t Nd> constexpr bool empty(square<Nd> const&) { return false; }

/// Square centroid coordinates
template <dim_t Nd> constexpr auto centroid(square<Nd> const& s) noexcept {
  return s.x_centroid_;
}

/// Bounds (min, max)
template <dim_t Nd>
constexpr rectangle_bounds<Nd> bounds(square<Nd> const& s) noexcept {
  rectangle_bounds<Nd> b;
  const num_t length_2 = length(s) / 2.;
  const auto xc        = centroid<Nd>(s);
  b.min()              = xc() - point<Nd>::c(length_2);
  b.max()              = xc() + point<Nd>::c(length_2);
  return b;
}

template <dim_t Nd> constexpr point<Nd> x_min(square<Nd> const& s) noexcept {
  return bounds(s).min;
}

template <dim_t Nd> constexpr point<Nd> x_max(square<Nd> const& s) noexcept {
  return bounds(s).max;
}

}  // namespace geometry
}  // namespace hm3
