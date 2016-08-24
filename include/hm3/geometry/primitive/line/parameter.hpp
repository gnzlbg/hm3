#pragma once
/// \file
///
/// Line parameter from point in line
#include <hm3/geometry/primitive/line/distance.hpp>
#include <hm3/geometry/primitive/line/line.hpp>
#include <hm3/geometry/primitive/line/ostream.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry::line_primitive {

namespace parameter_detail {

template <dim_t Nd>
constexpr num_t t(line<Nd> const& l, point<Nd> const& p, dim_t d) noexcept {
  auto denom = l.x(1)(d) - l.x(0)(d);
  HM3_ASSERT(!math::approx(std::abs(denom), 0.),
             "denom is zero (l: {}, p: {}, d: {})", l, p, d);
  return (p(d) - l.x(0)(d)) / denom;
}

}  // namespace parameter_detail

/// Either value of the line parameter if the point \p lies in the line \p l
/// or nothing.
template <dim_t Nd>
constexpr optional<num_t> parameter(line<Nd> const& l,
                                    point<Nd> const& p) noexcept {
  if (distance.approx(distance.minimum(l, p), 0.)) {
    for (dim_t d = 0; d < Nd; ++d) {
      if (!math::approx(l.direction()(d), 0.)) {
        return parameter_detail::t(l, p, d);
      }
    }
    HM3_ASSERT(false, "line: {}, direction vector has norm zero", l);
  }
  return {};
}

inline optional<std::pair<num_t, num_t>> parameter(point<1> const&,
                                                   vec<1> const&,
                                                   point<1> const&,
                                                   vec<1> const&) noexcept {
  return {};
}

inline optional<std::pair<num_t, num_t>> parameter(line<1> const&,
                                                   line<1> const&) {
  return {};
}

/// Intersection between two 2D lines \p l1 and \p l2.
inline optional<std::pair<num_t, num_t>> parameter(point<2> const& p1,
                                                   vec<2> const& v1,
                                                   point<2> const& p2,
                                                   vec<2> const& v2) noexcept {
  auto w = vec<2>(p2() - p1());
  auto c = perp_product(v1, v2);

  if (math::approx(c, 0.)) { return {}; }

  auto t = perp_product(w, v2) / c;
  auto s = perp_product(w, v1) / c;

  return std::make_pair(t, s);
}

/// Intersection between two 2D lines \p l1 and \p l2.
inline optional<std::pair<num_t, num_t>> parameter(line<2> const& l1,
                                                   line<2> const& l2) noexcept {
  return parameter(l1.origin(), l1.direction(), l2.origin(), l2.direction());
}

/// Intersection between two 3D lines
///
/// "Intersection of Two Lines in Three-Space", p. 304, Graphics Gems I.
///
/// Given two lines, L1 = P1 + t V1, L2 = P2 + s V2, the intersection parameters
/// are:
///
/// t = det{(P2 - P1), V2, V1 x V2} / |V1 x V2|^2
/// s = det{(P2 - P1), V1, V1 x V2} / |V1 x V2|^2
///
/// If the lines are parallel, |V1 x V2| == 0.
inline optional<std::pair<num_t, num_t>> parameter(point<3> const& p1,
                                                   vec<3> const& v1,
                                                   point<3> const& p2,
                                                   vec<3> const& v2) noexcept {
  auto w = vec<3>(p2() - p1());
  auto c = v1().cross(v2());

  auto cn = c.squaredNorm();

  if (math::approx(cn, 0.)) { return {}; }

  Eigen::Matrix3d m;
  m.row(0) = w();
  m.row(1) = v2();
  m.row(2) = c;

  num_t t = m.determinant() / cn;

  m.row(1) = v1();

  num_t s = m.determinant() / cn;

  return std::make_pair(t, s);
}

/// Intersection between two 3D lines \p l1 and \p l2.
inline optional<std::pair<num_t, num_t>> parameter(line<3> const& l1,
                                                   line<3> const& l2) noexcept {
  return parameter(l1.origin(), l1.direction(), l2.origin(), l2.direction());
}

}  // namespace hm3::geometry::line_primitive
