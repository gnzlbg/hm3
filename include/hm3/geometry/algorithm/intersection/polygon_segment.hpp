#pragma once
/// \file
///
/// Intersection of a segment with a polygon.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Intersection of the segment \p s with the polygon \p p.
template <dim_t Nd, typename P, CONCEPT_REQUIRES_(Polygon<P, Nd>{})>
variant<monostate, point<Nd>, segment<Nd>> intersection(segment<Nd> const& s,
                                                        P&& p) noexcept {
  HM3_ASSERT(convex(p), "polygon {} is not convex! (see comment)");
  // note: if the polygon is not convex:
  // - intersection should be handled in a different function, that returns
  // variant<monostate, small_vector<point<Nd>>, small_vector<segment<Nd>>>
  // - maybe we should have a tag "maybe_convex" to statically dispatch on this
  // - maybe we should just have an if(convex(p)) { convex_intersection(p); }
  //   else {non_convex_intersection(p); } to do both,

  // If multiple consecutive segments of the polygon form a line,
  // multiple points can be part of the intersection.
  small_vector<point<Nd>, 4> points;
  auto push_point = [&points](auto&& v) {
    if (end(points) == ranges::find(points, v)) { points.push_back(v); };
  };
  for (auto&& ps : faces(p)) {
    auto r = geometry::intersection(s, ps);
    visit(
     [&](auto&& i) {
       using T = uncvref_t<decltype(i)>;
       if
         constexpr(Same<T, point<Nd>>{}) { push_point(i); }
       else if
         constexpr(Same<T, segment<Nd>>{}) {
           push_point(i.x(0));
           push_point(i.x(1));
         }
       else if
         constexpr(Same<T, monostate>{}) { return; }
       else {
         static_assert(fail<T>{}, "non-exhaustive visitor");
       }
     },
     r);
  }

  bool p0_inside = geometry::intersection.test(p, s.x(0));
  bool p1_inside = geometry::intersection.test(p, s.x(1));

  auto make_segment = [&s](auto&& p0, auto&& p1) {
    // returns a segment that preserves the direction of the input segment
    auto l      = s.line();
    num_t ts[2] = {parameter(l, p0).value(), parameter(l, p1).value()};
    if (ts[0] <= ts[1]) { return segment<Nd>::through(p0, p1); }
    return segment<Nd>::through(p1, p0);
  };

  switch (size(points)) {
    case 0: {  // segment completely inside or no intersection
      if (p0_inside or p1_inside) { return s; }
      return monostate{};
    }
    case 1: {
      if (!p0_inside and !p1_inside) { return points[0]; }
      point<Nd> lp = p0_inside ? s.x(0) : s.x(1);
      return make_segment(lp, points[0]);
    }
    default: {
      HM3_UNREACHABLE();
      HM3_FATAL_ERROR("polygon: {}\nsegment: {}\nintersection points: {}\n", p,
                      s, view::all(points));
    }
    case 2: {
      return make_segment(points[0], points[1]);
    }
  }
}

/// Does the segment \p s intersect with the polygon \p p.
template <dim_t Nd, typename P, CONCEPT_REQUIRES_(Polygon<P, Nd>{})>
bool intersection_test(segment<Nd> const& s, P&& p) noexcept {
  auto ir = intersection(s, p);
  return visit(
   [](auto&& v) {
     using T = uncvref_t<decltype(v)>;
     if
       constexpr(Same<T, monostate>{}) { return false; }
     return true;
   },
   ir);
}

}  // namespace hm3::geometry::segment_primitive
