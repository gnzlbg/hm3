#pragma once
/// \file
///
/// Intersection of a segment with a polygon.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/algorithm/is_convex.hpp>
#include <hm3/geometry/algorithm/line_intersection_parameter.hpp>
#include <hm3/ext/variant.hpp>

namespace hm3::geometry {

namespace intersection_polygon_segment_detail {

struct intersection_polygon_segment_fn {
  /// Intersection of the segment \p s with the polygon \p p.
  template <typename P, typename S, typename PT = associated::point_t<S>>
  variant<monostate, PT, S> operator()(P&& p, S const& s, num_t abs_tol,
                                       num_t rel_tol) const noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    static_assert(Segment<S>{});
    static_assert(ad_v<S> == ad_v<P>);
    static_assert(Same<associated::point_t<P>, PT>{});
    using e_t = associated::edge_t<P>;
    HM3_ASSERT(is_convex(p), "polygon {} is not convex! (not implemented)", p);

    // If multiple consecutive segments of the polygon form a line,
    // multiple points can be part of the intersection.
    fixed_capacity_vector<PT, 4> points;

    auto equal_points
     = [=](auto&& i, auto&& j) { return approx_point(i, j, abs_tol, rel_tol); };

    auto push_point
     = [&](auto&& v) { unique_push_back(points, v, equal_points); };
    for (auto&& ps : edges(p)) {
      auto r = intersection_segment_segment(ps, s);
      visit(
       [&](auto&& i) {
         using T = uncvref_t<decltype(i)>;
         if
           constexpr(Same<T, PT>{}) { push_point(i); }
         else if
           constexpr(Same<T, e_t>{}) {
             push_point(i.x(0));
             push_point(i.x(1));
           }
         else if
           constexpr(Same<T, monostate>{}) { return; }
         else {
           static_assert(always_false<T>{}, "non-exhaustive visitor");
         }
       },
       r);
    }

    bool p0_inside
     = intersection_test_polygon_point(p, s.x(0), abs_tol, rel_tol);
    bool p1_inside
     = intersection_test_polygon_point(p, s.x(1), abs_tol, rel_tol);

    auto make_segment = [&](auto&& p0, auto&& p1) {
      // returns a segment that preserves the direction of the input segment
      auto l      = s.line();
      num_t ts[2] = {line_intersection_parameter(l, p0).value(),
                     line_intersection_parameter(l, p1).value()};
      if (approx.leq(ts[0], ts[1], abs_tol, rel_tol)) { return S(p0, p1); }
      return S(p1, p0);
    };

    switch (size(points)) {
      case 0: {  // segment completely inside or no intersection
        if (p0_inside or p1_inside) { return s; }
        return monostate{};
      }
      case 1: {  // segment interescts a single point
        if (!p0_inside and !p1_inside) { return points[0]; }
        PT lp = p0_inside ? s.x(0) : s.x(1);
        // the segment can be either outside, s.t. the result is the single
        // point:
        if (equal_points(lp, points[0])) { return points[0]; }
        // or inside, s.t. the result is the segment up to the intersection
        // point:
        return make_segment(lp, points[0]);
      }
      case 2: {  // segment intersects two points, can happen along an edge:
        HM3_ASSERT(!equal_points(points[0], points[1]), "cannot happen");
        return make_segment(points[0], points[1]);
      }
      default: {  // segment intersects multipe points, can happen along an edge
                  // with colinear vertices:
        auto p0 = points.back();
        points.pop_back();
        auto p1 = points.back();
        points.pop_back();
        auto rs = S(p0, p1);
        while (!points.empty()) {
          auto n = points.back();
          points.pop_back();
          auto t_ = line_intersection_parameter(rs, n, on_segment_line);
          HM3_ASSERT(  // not colinear ?!
           t_, "point {} is not on the line spanned by the segment {} ??", n,
           rs);
          auto t = t_.value();
          if (t < 0.) { rs = S(n, rs.x(1)); }
          if (t > 1.) { rs = S(rs.x(0), n); }
        }
        return make_segment(rs.x(0), rs.x(1));
      }
    }
  }
};

}  // namespace intersection_polygon_segment_detail

namespace {
static constexpr auto const& intersection_polygon_segment
 = static_const<with_default_tolerance<
  intersection_polygon_segment_detail::intersection_polygon_segment_fn>>::value;
}

namespace intersection_test_polygon_segment_detail {

struct intersection_test_polygon_segment_fn {
  /// Does the segment \p s intersect with the polygon \p p.
  template <typename P, typename S>
  bool operator()(P&& p, S const& s, num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    static_assert(Segment<S>{});
    static_assert(ad_v<S> == ad_v<P>);
    auto ir = intersection_polygon_segment(p, s, abs_tol, rel_tol);
    return visit(
     [](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, monostate>{}) { return false; }
       return true;
     },
     ir);
  }
};

}  // namespace intersection_test_polygon_segment_detail

namespace {
static constexpr auto const& intersection_test_polygon_segment = static_const<
 with_default_tolerance<intersection_test_polygon_segment_detail::
                         intersection_test_polygon_segment_fn>>::value;
}

}  // namespace hm3::geometry::segment_primitive
