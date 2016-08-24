#pragma once
/// \file
///
/// Line.
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/geometry/primitive/vec.hpp>

namespace hm3::geometry {

/// Line primitive.
namespace line_primitive {

struct not_normalized_t {};

static constexpr not_normalized_t not_normalized{};

/// Line (unbounded)
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>
struct line : ranked<Nd, 1> {
  using point_t   = point<Nd>;
  using vector_t  = vec<Nd>;
  using storage_t = Eigen::ParametrizedLine<num_t, Nd>;

  storage_t l_;

  /// Point at \p t * 1. from the origin along the line.
  point_t x(num_t t) const noexcept { return point_t{l_.pointAt(t)}; }

  /// Line line that goes through the 2 points \p b and \p e : [b, e]
  static line through(point_t b, point_t e) noexcept {
    HM3_ASSERT(b != e, "line through two equal points: {} == {}!", b, e);
    line l;
    l.l_ = storage_t::Through(b(), e());
    return l;
  }

  /// Origin of the line.
  point_t origin() const noexcept { return point_t{l_.origin()}; }

  /// Direction vector of the line.
  vector_t direction() const noexcept { return vector_t{l_.direction()}; }

  /// Access the underlying Eigen::ParametrizedLine.
  auto const& operator()() const noexcept { return l_; }

  /// Constructs a line from an \p origin and a \p direction.
  ///
  /// \pre The \p direction vector needs to be normalized.
  constexpr line(point_t origin, vector_t direction) noexcept
   : l_(origin(), direction()) {
    HM3_ASSERT(math::approx(direction().norm(), 1.),
               "Direction must be normalized!\n\ndirection: {}, norm: {}",
               direction, num_t{direction().norm()});
  }

  /// Constructs a line with the direction vector not normalized:
  constexpr line(point_t origin, vector_t direction, not_normalized_t) noexcept
   : l_(origin(), direction()) {}

  constexpr line()            = default;
  constexpr line(line const&) = default;
  constexpr line(line&&)      = default;
  constexpr line& operator=(line const&) = default;
  constexpr line& operator=(line&&) = default;

  segment<Nd> as_segment(num_t line_length = 1e200) const noexcept {
    return segment<Nd>::through(x(-.5 * line_length), x(.5 * line_length));
  }
};

/// Is the representation of the lines \p a and \p b equal?
template <dim_t Nd>
bool operator==(line<Nd> const& a, line<Nd> const& b) noexcept {
  return a.origin() == b.origin() and a.direction() == b.direction();
}

template <dim_t Nd>
bool operator!=(line<Nd> const& a, line<Nd> const& b) noexcept {
  return !(a == b);
}

}  // namespace line_primitive

using line_primitive::line;
using line_primitive::not_normalized;

namespace concepts {

// A line is infinite (unbounded).
template <dim_t Nd> struct is_bounded<line<Nd>> : std::false_type {};
}

}  // namespace hm3::geometry
