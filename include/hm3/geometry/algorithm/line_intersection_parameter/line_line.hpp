#pragma once
/// \file
///
/// Parameter of the intersection between two lines.
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/cross.hpp>
#include <hm3/geometry/algorithm/distance/minimum_line_point.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace intersection_parameter_line_line_detail {

struct intersection_parameter_line_line_fn {
  /// Intersection between two 1D lines.
  template <typename P, typename V,
            CONCEPT_REQUIRES_(Point<P, 1>{} and Vector<V, 1>{})>
  constexpr optional<pair<num_t, num_t>> operator()(P const&, V const&,
                                                    P const&, V const&, num_t,
                                                    num_t) const {
    return {};
  }

  /// Intersection between two 2D lines (p0, v0) and (p1, v1).
  template <typename P, typename V,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Vector<V, 2>{})>
  constexpr optional<pair<num_t, num_t>> operator()(P const& p0, V const& v0,
                                                    P const& p1, V const& v1,
                                                    num_t abs_tol,
                                                    num_t rel_tol) const {
    auto w = V(p1() - p0());
    auto c = perp_product(v0, v1);

    if (approx_number(c, 0., abs_tol, rel_tol)) { return {}; }

    auto t = perp_product(w, v1) / c;
    auto s = perp_product(w, v0) / c;

    return make_pair(t, s);
  }

  /// Intersection between two 3D lines (p0, v0) and (p1, v1).
  ///
  /// "Intersection of Two Lines in Three-Space", p. 304, Graphics Gems I.
  ///
  /// Given two lines, L0 = P0 + t V0, L1 = P1 + s V1, the intersection
  /// parameters
  /// are:
  ///
  /// t = det{(P1 - P0), V1, V0 x V1} / |V0 x V1|^2
  /// s = det{(P1 - P0), V0, V0 x V1} / |V0 x V1|^2
  ///
  /// If the lines are parallel => |V0 x V1| == 0.
  template <typename P, typename V,
            CONCEPT_REQUIRES_(Point<P, 3>{} and Vector<V, 3>{})>
  optional<pair<num_t, num_t>> operator()(P const& p0, V const& v0, P const& p1,
                                          V const& v1, num_t abs_tol,
                                          num_t rel_tol) const {
    auto w = V(p1() - p0());
    auto c = v0().cross(v1());

    auto cn = c.squaredNorm();

    if (approx_number(cn, 0., abs_tol, rel_tol)) { return {}; }

    Eigen::Matrix3d m;
    m.row(0) = w();
    m.row(1) = v1();
    m.row(2) = c;

    num_t t = m.determinant() / cn;

    m.row(1) = v0();

    num_t s = m.determinant() / cn;

    return make_pair(t, s);
  }

  /// Line parameters of the intersection point between the lines \p l0 and \p
  /// l1.
  template <typename L, CONCEPT_REQUIRES_(Line<L>{})>
  constexpr optional<pair<num_t, num_t>> operator()(L const& l0, L const& l1,
                                                    num_t abs_tol,
                                                    num_t rel_tol) const
   noexcept {
    return (*this)(l0.origin(), l0.direction(), l1.origin(), l1.direction(),
                   abs_tol, rel_tol);
  }
};

}  // namespace intersection_parameter_line_line_detail

namespace {
constexpr auto const& intersection_parameter_line_line = static_const<
 with_default_tolerance<intersection_parameter_line_line_detail::
                         intersection_parameter_line_line_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
