#pragma once
/// \file
///
/// Rectangle type
#include <hm3/geometry/dimension/dimensional.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/geometry/rectangle/bounds.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/config/fatal_error.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

/// Rectangle
template <uint_t Nd>  //
struct rectangle : dimensional<Nd> {
  using point_t  = point<Nd>;
  using vector_t = vector<Nd>;

  point_t x_min_, x_max_;

  rectangle()                           = default;
  constexpr rectangle(rectangle const&) = default;
  constexpr rectangle(rectangle&&)      = default;
  constexpr rectangle& operator=(rectangle const&) = default;
  constexpr rectangle& operator=(rectangle&&) = default;

  /// Rectangle from \p x_min and \p x_max.
  constexpr rectangle(point_t const& x_min, point_t const& x_max)
   : x_min_(x_min), x_max_(x_max) {}

  /// Rectangle from centroid and lengths
  static constexpr rectangle<Nd> at(point_t const& x_c,
                                    vector_t const& lengths) {
    return rectangle<Nd>(point_t{x_c() - .5 * lengths()},
                         point_t{x_c() + .5 * lengths()});
  }

  /// Number of m-dimensional edges
  static constexpr auto no_edges(uint_t m) noexcept -> suint_t {
    if (Nd < m || m < 0) { return 0; }
    return math::ipow(uint_t{2}, Nd - m) * math::binomial_coefficient(Nd, m);
  }

  /// Number of corners (one-dimensional edges)
  static constexpr auto no_corners() noexcept -> suint_t {
    return math::ipow(uint_t{2}, Nd);
  }
};

template <uint_t Nd> constexpr bool empty(rectangle<Nd> const&) {
  return false;
}

/// Bounds (min, max)
template <uint_t Nd>
constexpr rectangle_bounds<Nd> bounds(rectangle<Nd> const& s) noexcept {
  return {s.x_min_, s.x_max_};
}

template <uint_t Nd>
constexpr point<Nd> x_min(rectangle<Nd> const& s) noexcept {
  return s.x_min_;
}

template <uint_t Nd>
constexpr point<Nd> x_max(rectangle<Nd> const& s) noexcept {
  return s.x_max_;
}

/// Rectangle length
template <uint_t Nd>
constexpr num_t length(rectangle<Nd> const& s, uint_t d) noexcept {
  return s.x_max_(d) - s.x_min_(d);
}

/// Square lengths (they all equal the same value)
template <uint_t Nd>
constexpr vector<Nd> lengths(rectangle<Nd> const& s) noexcept {
  return vector<Nd>(s.x_max_() - s.x_min_());
}

template <uint_t Nd>
bool operator==(rectangle<Nd> const& l, rectangle<Nd> const& r) noexcept {
  return l.x_min_ == r.x_min_ and l.x_max_ == r.x_max_;
}

template <uint_t Nd>
bool operator!=(rectangle<Nd> const& l, rectangle<Nd> const& r) noexcept {
  return !(l == r);
}

/// Rectangle centroid coordinates
template <uint_t Nd>
constexpr point<Nd> centroid(rectangle<Nd> const& s) noexcept {
  return point<Nd>(s.x_min_() + .5 * (s.x_max_() - s.x_min_()));
}

}  // namespace geometry
}  // namespace hm3
