#pragma once
/// \file
///
/// ASCII OStream writer.
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/face.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/concept/polyhedron.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/concept/ray.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/concept/vector.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/utility/ascii.hpp>
#include <limits>

namespace hm3::ascii_fmt {

namespace geometry_detail {

using namespace ::hm3::geometry;

template <typename OStream, typename P, dim_t Ad = ad_v<P>>
constexpr OStream& to_ascii_impl(OStream& o, P const& p, trait::vector<Ad>) {
  static_assert(Vector<P>{});
  o << "(";
  for (dim_t d = 0; d < ambient_dimension(p); ++d) {
    if (d > 0) { o << ","; }
    o << std::setprecision(std::numeric_limits<num_t>::digits10 + 1) << p(d);
  }
  o << ")";

  return o;
}

template <typename OStream, typename P, dim_t Ad = ad_v<P>>
constexpr OStream& to_ascii_impl(OStream& o, P const& p, trait::point<Ad>) {
  static_assert(Point<P>{});
  o << "[";
  for (dim_t d = 0; d < ambient_dimension(p); ++d) {
    if (d > 0) { o << ","; }
    o << std::setprecision(std::numeric_limits<num_t>::digits10 + 1) << p(d);
  }
  o << "]";
  return o;
}

template <typename OStream, typename L, dim_t Ad = ad_v<L>>
constexpr OStream& to_ascii_impl(OStream& o, L const& l, trait::line<Ad>) {
  static_assert(Line<L>{});
  o << "{l | o: ";
  to_ascii(o, l.origin());
  o << ", d: ";
  to_ascii(o, l.direction());
  o << "}";
  return o;
}

template <typename OStream, typename L, dim_t Ad = ad_v<L>>
constexpr OStream& to_ascii_impl(OStream& o, L const& l, trait::ray<Ad>) {
  static_assert(Ray<L>{});
  o << "{r | o: ";
  to_ascii(o, l.origin());
  o << ", d: ";
  to_ascii(o, l.direction());
  o << "}";
  return o;
}

template <typename OStream, typename A, dim_t Ad = ad_v<A>>
constexpr OStream& to_ascii_impl(OStream& o, A const& a, trait::aabb<Ad>) {
  o << "{ aabb | min: ";
  to_ascii(o, x_min(a));
  o << ", max: ";
  to_ascii(o, x_max(a));
  o << "}";
  return o;
}

template <typename OStream, typename B, dim_t Ad = ad_v<B>>
constexpr OStream& to_ascii_impl(OStream& o, B const& b, trait::box<Ad>) {
  o << "{ box | min: ";
  to_ascii(o, x_min(b));
  o << ", max: ";
  to_ascii(o, x_max(b));
  o << "}";
  return o;
}

template <typename OStream, typename L, dim_t Ad = ad_v<L>>
constexpr OStream& to_ascii_impl(OStream& o, L const& l, trait::segment<Ad>) {
  static_assert(Segment<L>{});
  o << "{s | x(0): ";
  to_ascii(o, l.x(0));
  o << ", x(1): ";
  to_ascii(o, l.x(1));
  o << "}";
  return o;
}

template <typename OStream, typename L, dim_t Ad = ad_v<L>>
constexpr OStream& to_ascii_impl(OStream& o, L const& l, trait::polyline<Ad>) {
  static_assert(Polyline<L>{});

  o << "{pl | \n";
  suint_t eidx = 0;
  for (auto&& e : edges(l)) {
    o << "  e(" << eidx << "): ";
    to_ascii(o, e);
    o << "\n";
    ++eidx;
  }
  o << "}";
  return o;
}

template <typename OStream, typename P, dim_t Ad = ad_v<P>>
constexpr OStream& to_ascii_impl(OStream& o, P const& p, trait::polygon<Ad>) {
  static_assert(Polygon<P>{});
  o << "{pg | \n";
  suint_t eidx = 0;
  for (auto&& e : edges(p)) {
    o << "  e(" << eidx << "): ";
    to_ascii(o, e);
    o << "\n";
    ++eidx;
  }
  o << "}";
  return o;
}

template <typename OStream, typename P, dim_t Ad = ad_v<P>>
constexpr OStream& to_ascii_impl(OStream& o, P const& p, trait::plane<Ad>) {
  o << "{plane | n: ";
  to_ascii(p.normal());
  o << ", x: ";
  to_ascii(p.distance_to_origin());
  o << "}";
  return o;
}

template <typename OStream, typename P>
constexpr OStream& to_ascii_impl(OStream& o, P const& p, trait::polyhedron) {
  static_assert(Polyhedron<P>{});
  o << "{ph | \n";
  suint_t eidx = 0;
  for (auto&& e : faces(p)) {
    o << "  f(" << eidx << "): ";
    to_ascii(o, e);
    o << "\n";
    ++eidx;
  }
  o << "}";
  return o;
}

}  // namespace geometry_detail

template <typename OStream, typename T>
constexpr auto to_ascii(OStream& o, T&& t, geometry const&)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(geometry_detail::to_ascii_impl(
  o, std::forward<T>(t), geometry_detail::associated::t_<T>{}));

}  // namespace hm3::ascii_fmt
