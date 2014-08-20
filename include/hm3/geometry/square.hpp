#pragma once
/// \file square.hpp Square
#include <hm3/geometry/point.hpp>
#include <hm3/utility/constants.hpp>
#include <hm3/utility/floating_point.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/range.hpp>
#include <hm3/types.hpp>
//#include <hm3/tree/relations/tree.hpp>

namespace hm3 {

namespace geometry {

/// Is the box spanned by \p x_min and \p x_max square?
template <int_t nd>
constexpr auto is_square(point<nd> const& x_min, point<nd> const& x_max)
 -> bool {
  auto lengths = (x_max() - x_min()).eval();
  HM3_ASSERT(
   [=]() {
     for (int_t d = 0; d != nd; ++d) {
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
template <int_t nd>
constexpr auto square_center(point<nd> const& x_min, point<nd> const& x_max)
 -> point<nd> {
  HM3_ASSERT(is_square(x_min, x_max), "box is not square!");
  return (x_min() + (x_max() - x_min()) * 0.5).eval();
}

//// Length of the square box spanned by \p x_min and \p x_max
template <int_t nd>
constexpr auto square_length(point<nd> const& x_min, point<nd> const& x_max)
 -> num_t {
  HM3_ASSERT(is_square(x_min, x_max), "box is not square!");
  return x_max(0) - x_min(0);
}

/// Square
template <int_t nd> struct square {
  using point_t = point<nd>;

  point_t x_center_;
  num_t length_;

  square() = default;
  constexpr square(square const&) = default;
  constexpr square(square&&) = default;
  constexpr square& operator=(square const&) = default;
  constexpr square& operator=(square&&) = default;

  /// Square from \p x_min and \p x_max.
  constexpr square(const point_t& x_min, const point_t& x_max)
   : x_center_(square_center(x_min, x_max))
   , length_(square_length(x_min, x_max)) {}

  /// Square from \p x_center and \p x_length.
  constexpr square(const point_t& x_center, const num_t length)
   : x_center_(x_center), length_(length) {}

  /// Number of m-dimensional edges
  static constexpr auto no_edges(const int_t m) noexcept -> int_t {
    if (nd < m || m < 0) { return 0; }
    return math::ipow(int_t{2}, nd - m) * math::binomial_coefficient(nd, m);
  }

  /// Number of corners (one-dimensional edges)
  static constexpr auto no_corners() noexcept -> sint_t {
    return math::ipow(int_t{2}, nd);
  }

  static auto corner_positions() noexcept {
    return view::iota(0, no_corners());
  }

  static auto unit() noexcept {
    return square<nd>{geometry::point<nd>::constant(0.),
                      geometry::point<nd>::constant(1.)};
  }
};

/// Square dimension
template <int_t nd>
constexpr auto dimension(square<nd> const&)
 HM3_RETURNS(std::integral_constant<int_t, nd>{});

/// Square length
template <int_t nd>
constexpr auto length(square<nd> const& s) HM3_RETURNS(s.length_);

/// Square centroid coordinates
template <int_t nd>
constexpr auto center(square<nd> const& s) HM3_RETURNS(s.x_center_);

/// Square volume
template <int_t nd> constexpr auto volume(square<nd> const& s) noexcept {
  const auto l = length(s);
  num_t vol    = 1;
  for (int_t d = 0; d != nd; ++d) { vol *= l; }
  return vol;
}

/// Bounds (min, max)
template <int_t nd>
constexpr std::array<point<nd>, 2> bounds(square<nd> const& s) noexcept {
  std::array<point<nd>, 2> b;
  const num_t length_2 = length(s) / 2.;
  const auto xc        = center(s);
  b[0]() = xc() - point<nd>::c(length_2);
  b[1]() = xc() + point<nd>::c(length_2);
  return b;
}

template <int_t nd>
bool operator==(square<nd> const& l, square<nd> const& r) noexcept {
  return math::approx(length(l), length(r)) && center(l) == center(r);
}

template <int_t nd>
bool operator!=(square<nd> const& l, square<nd> const& r) noexcept {
  return !(l == r);
}

/// Square corners relative position (+-1)
///
///  Order: right hand rule
///
///       6         7
///        *-------*
///       /|      /|
///      / |     / |       x(1)   x(2)
///   2 *-------* 3|        ^   ^
///     |  *----|--*        |  /
///     | / 4   | / 5       | /
///     |/      |/          |/
///     *-------*           *------> x(0)
///     0       1
///
constexpr auto relative_corner_position(sidx_t corner_pos, sidx_t dimension)
 -> sint_t {
  if (dimension == 0) {
    return (corner_pos + 1) / 2 % 2 ? 1 : -1;
  } else if (dimension == 1) {
    return (corner_pos / 2) % 2 ? 1 : -1;
  } else if (dimension == 2) {
    return (corner_pos / 4) % 4 ? +1 : -1;
  } else {
    return invalid;
  }
}

template <int_t nd, int_t... is>
constexpr auto relative_corner_position(sint_t corner_pos,
                                        std::integer_sequence<int_t, is...>)
 -> point<nd> {
  return point<nd>{
   static_cast<num_t>(relative_corner_position(corner_pos, is))...};
}

template <int_t nd>
constexpr auto relative_corner_position(sint_t corner_pos) -> point<nd> {
  return relative_corner_position<nd>(corner_pos,
                                      std::make_integer_sequence<int_t, nd>{});
}

/// Square corners
template <int_t nd> constexpr auto corners(square<nd> const& s) {
  std::array<point<nd>, square<nd>::no_corners()> corners;
  const auto half_length = 0.5 * length(s);
  RANGES_FOR (auto&& c, square<nd>::corner_positions()) {
    const auto x_c = center(s);
    const auto x_p = relative_corner_position<nd>(c);
    corners[c]     = x_c() + half_length * x_p();
  }
  return corners;
}

template <class OStream, int_t nd>
OStream& operator<<(OStream& o, square<nd> const& s) {
  auto b = bounds(s);
  o << "(center: " << center(s) << ", length: " << length(s)
    << ", min: " << b[0] << ", max: " << b[1] << ")";
  return o;
}

}  // namespace geometry
}  // namespace hm3
