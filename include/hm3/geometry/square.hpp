#pragma once
/// \file
///
/// Square
#include <hm3/geometry/dimensions.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/constants.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

namespace geometry {

/// Is the box spanned by \p x_min and \p x_max square?
template <int_t Nd>
constexpr auto is_square(point<Nd> const& x_min, point<Nd> const& x_max)
 -> bool {
  auto lengths = (x_max() - x_min()).eval();
  HM3_ASSERT(
   [=]() {
     for (int_t d = 0; d != Nd; ++d) {
       HM3_ASSERT(lengths(d) > 0,
                  "Negative length! Box's length '{}' < 0 along dimension '{}'",
                  lengths(d), d);
     }
     return true;
   }(),
   "negative length");
  return all_of(lengths, [&](auto i) { return math::approx(i, lengths(0)); });
}

/// Center coordinates of square the box spanned by \p x_min and \p x_max
template <int_t Nd>
constexpr auto square_center(point<Nd> const& x_min, point<Nd> const& x_max)
 -> point<Nd> {
  HM3_ASSERT(is_square(x_min, x_max), "box is not square!");
  return (x_min() + (x_max() - x_min()) * 0.5).eval();
}

//// Length of the square box spanned by \p x_min and \p x_max
template <int_t Nd>
constexpr auto square_length(point<Nd> const& x_min, point<Nd> const& x_max)
 -> num_t {
  HM3_ASSERT(is_square(x_min, x_max), "box is not square!");
  return x_max(0) - x_min(0);
}

/// Square
template <int_t Nd> struct square : dimensional<Nd> {
  using point_t = point<Nd>;

  point_t x_center_;
  num_t length_;

  square()                        = default;
  constexpr square(square const&) = default;
  constexpr square(square&&)      = default;
  constexpr square& operator=(square const&) = default;
  constexpr square& operator=(square&&) = default;

  /// Square from \p x_min and \p x_max.
  constexpr square(point_t const& x_min, point_t const& x_max)
   : x_center_(square_center(x_min, x_max))
   , length_(square_length(x_min, x_max)) {}

  /// Square from \p x_center and \p x_length.
  constexpr square(point_t x_center, num_t length)
   : x_center_(std::move(x_center)), length_(length) {}

  /// Number of m-dimensional edges
  static constexpr auto no_edges(int_t m) noexcept -> int_t {
    if (Nd < m || m < 0) { return 0; }
    return math::ipow(int_t{2}, Nd - m) * math::binomial_coefficient(Nd, m);
  }

  /// Number of corners (one-dimensional edges)
  static constexpr auto no_corners() noexcept -> sint_t {
    return math::ipow(int_t{2}, Nd);
  }

  static auto corner_positions() noexcept {
    return view::iota(0, no_corners());
  }

  static auto unit() noexcept {
    return square<Nd>{geometry::point<Nd>::constant(0.),
                      geometry::point<Nd>::constant(1.)};
  }
};

/// Square dimension
template <int_t Nd> constexpr auto dimension(square<Nd> const&) {
  return std::integral_constant<int_t, Nd>{};
}

/// Square length
template <int_t Nd> constexpr auto length(square<Nd> const& s) noexcept {
  return s.length_;
}

/// Square centroid coordinates
template <int_t Nd> constexpr auto center(square<Nd> const& s) noexcept {
  return s.x_center_;
}

/// Square centroid coordinates
template <int_t Nd> constexpr auto centroid(square<Nd> const& s) noexcept {
  return s.x_center_;
}

/// Square volume
template <int_t Nd> constexpr auto volume(square<Nd> const& s) noexcept {
  const auto l = length(s);
  num_t vol    = 1;
  for (int_t d = 0; d != Nd; ++d) { vol *= l; }
  return vol;
}

/// Bounds (min, max)
template <int_t Nd> constexpr auto bounds(square<Nd> const& s) noexcept {
  struct {
    point<Nd> min;
    point<Nd> max;
  } b{};
  const num_t length_2 = length(s) / 2.;
  const auto xc        = center(s);
  b.min()              = xc() - point<Nd>::c(length_2);
  b.max()              = xc() + point<Nd>::c(length_2);
  return b;
}

template <int_t Nd> constexpr point<Nd> x_min(square<Nd> const& s) noexcept {
  return bounds(s)[0];
}

template <int_t Nd> constexpr point<Nd> x_max(square<Nd> const& s) noexcept {
  return bounds(s)[1];
}

template <int_t Nd>
bool operator==(square<Nd> const& l, square<Nd> const& r) noexcept {
  return math::approx(length(l), length(r)) && center(l) == center(r);
}

template <int_t Nd>
bool operator!=(square<Nd> const& l, square<Nd> const& r) noexcept {
  return !(l == r);
}

/// Square corners relative position (+-1)
///
/// Order: right hand rule (counter clock wise)
///
///       7         6
///        *-------*
///       /|      /|
///      / |     / |       x(1)   x(2)
///   3 *-------* 2|        ^   ^
///     |  *----|--*        |  /
///     | / 4   | / 5       | /
///     |/      |/          |/
///     *-------*           *------> x(0)
///     0       1
///
constexpr sint_t relative_corner_position(sidx_t corner_pos,
                                          sidx_t dimension) noexcept {
  switch (dimension) {
    case 0: {
      return (corner_pos + 1) / 2 % 2 ? 1 : -1;
    }
    case 1: {
      return (corner_pos / 2) % 2 ? 1 : -1;
    }
    case 2: {
      return (corner_pos / 4) % 4 ? +1 : -1;
    }
    default: { HM3_FATAL_ERROR("unknown dimension {}", dimension); }
  }
}

template <int_t Nd, int_t... Is>
constexpr point<Nd> relative_corner_position(
 sint_t corner_pos, std::integer_sequence<int_t, Is...>) noexcept {
  return point<Nd>{
   static_cast<num_t>(relative_corner_position(corner_pos, Is))...};
}

template <int_t Nd>
constexpr point<Nd> relative_corner_position(sint_t corner_pos) noexcept {
  return relative_corner_position<Nd>(corner_pos,
                                      std::make_integer_sequence<int_t, Nd>{});
}

/// Square corners
template <int_t Nd> constexpr auto corners(square<Nd> const& s) noexcept {
  std::array<point<Nd>, square<Nd>::no_corners()> corners;
  const auto half_length = 0.5 * length(s);
  RANGES_FOR (auto&& c, square<Nd>::corner_positions()) {
    const auto x_c = center(s);
    const auto x_p = relative_corner_position<Nd>(c);
    corners[c]     = x_c() + half_length * x_p();
  }
  return corners;
}

/// Does the square \p s contain the point \p?
template <int_t Nd>
constexpr bool contains(square<Nd> const& s, point<Nd> const& p) noexcept {
  auto b = bounds(s);
  for (auto d : dimensions(Nd)) {
    if (b.min[d] > p[d] || b.max[d] < p[d]) { return false; }
  }
  return true;
}

template <typename OStream, int_t Nd>
OStream& operator<<(OStream& o, square<Nd> const& s) {
  auto b = bounds(s);
  o << "(center: " << center(s) << ", length: " << length(s)
    << ", min: " << b.min << ", max: " << b.max << ")";
  return o;
}

}  // namespace geometry
}  // namespace hm3
