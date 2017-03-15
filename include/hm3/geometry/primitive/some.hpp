#pragma once
/// \file
///
/// The `some` type can store enough primitives to represent the geometry of any
/// primitve object.
///
/// It is mainly useful for debugging and easily interfacing with VTK.
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/polyhedron.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/segment.hpp>

namespace hm3::geometry::some_primitive {

/// \name Geometry types stored in the `some`
///@{

template <dim_t Ad>
struct some_types {};

template <>
struct some_types<1> {
  using type = meta::list<point<1>, polyline<1>>;
};

template <>
struct some_types<2> {
  using type = meta::list<point<2>, polyline<2>, polygon<2>>;
};

template <>
struct some_types<3> {
  using type
   = meta::list<point<3>, polyline<3>, polygon<3>, polyhedron, aabb<3>, box<3>>;
};

template <dim_t Ad>
using some_types_t = typename some_types<Ad>::type;

template <dim_t Ad>
using some_variant_t = meta::apply<meta::quote<variant>, some_types_t<Ad>>;

///@}  // Geometry types stored in `some`.

template <dim_t Ad>
struct some : some_variant_t<Ad> {
  using geometry_type = trait::ambient_dimension<Ad>;
  using base_t        = some_variant_t<Ad>;
  using base_t::base_t;
  using base_t::operator=;
  using self            = some<Ad>;
  using p_t             = point<Ad>;
  using s_t             = segment<Ad>;
  using pl_t            = polyline<Ad>;

  some()            = default;
  some(some const&) = default;
  some(some&&)      = default;
  some& operator=(some const&) = default;
  some& operator=(some&&) = default;
  ~some()                 = default;

  /// Converts from any point into some's point type
  template <typename P, CONCEPT_REQUIRES_(Point<uncvref_t<P>>{})>
  static p_t from(P&& p) noexcept {
    return p_t(p());
  }

  /// Converts from any segment into some's segment type
  template <typename S, CONCEPT_REQUIRES_(Segment<uncvref_t<S>>{})>
  static s_t from_edge(S&& s) noexcept {
    return s_t(from(s.x(0)), from(s.x(1)));
  }

  /// Converts from any segment into some's polyline type
  template <typename S, CONCEPT_REQUIRES_(Segment<uncvref_t<S>>{})>
  static pl_t from(S&& s) noexcept {
    return pl_t(edges(s)
                | view::transform([](auto&& e) { return from_edge(e); }));
  }

  /// Converts from any polygon into some's polygon type
  template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
  static auto from(P& p) noexcept {
    return polygon<Ad>(
     edges(p) | view::transform([](auto&& e) { return from_edge(e); }));
  }

  /// Converts from any polygon into some's polygon type
  template <typename P, CONCEPT_REQUIRES_(Polyline<P>{})>
  static auto from(P& p) noexcept {
    return pl_t(edges(p)
                | view::transform([](auto&& e) { return from_edge(e); }));
  }

  /// Converts from any polygon into some's polygon type
  template <typename P, CONCEPT_REQUIRES_(Polyhedron<P>{} and not Box<P>{}
                                          and not AABB<P>{})>
  static auto from(P& p) noexcept {
    return polyhedron(faces(p)
                      | view::transform([](auto&& f) { return from(f); }));
  }

  /// Converts from any box into some's box type
  template <typename P,
            CONCEPT_REQUIRES_(Polyhedron<P>{} and Box<P>{} and not AABB<P>{})>
  static auto from(P& p) noexcept {
    return box<3>(from(p.centroid()), p.length());
  }

  /// Converts from any aabb into some's aabb type
  template <typename P, CONCEPT_REQUIRES_(Polyhedron<P>{} and AABB<P>{})>
  static auto from(P& p) noexcept {
    return aabb<3>(from(x_min(p)), from(x_max(p)));
  }

  /// Converts from any ray into some's segment type
  template <typename S, CONCEPT_REQUIRES_(Ray<uncvref_t<S>>{})>
  static auto from(S&& s) noexcept {
    p_t x_begin = p_t(s.origin()());
    p_t x_end   = p_t(
     x_begin() + s.direction()() * 0.5 * std::numeric_limits<float>::max());
    return from(s_t(x_begin, x_end));
  }

  /// Converts from any line into some's segment type
  template <typename S, CONCEPT_REQUIRES_(Line<uncvref_t<S>>{})>
  static auto from(S&& s) noexcept {
    p_t x_begin = p_t(
     s.origin()() - s.direction()() * 0.5 * std::numeric_limits<float>::max());
    p_t x_end = p_t(
     s.orogin()() + s.direction()() * 0.5 * std::numeric_limits<float>::max());
    return from(s_t(x_begin, x_end));
  }

  /// Convert from a supported type:
  template <typename T>
  explicit some(T t) : base_t(self::from(t)) {}

  template <typename T>
  some& operator=(T t) {
    some a(t);
    (*this) = a;
    return *this;
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Ad>
bool operator==(some<Ad> const& a, some<Ad> const& b) noexcept {
  return a == b;
}

template <dim_t Ad>
bool operator!=(some<Ad> const& a, some<Ad> const& b) noexcept {
  return a != b;
}

/// Number of vertices in a some.
template <dim_t Ad>
constexpr dim_t vertex_size(some<Ad> const& a) noexcept {
  return visit(a, [](auto&& i) { return vertex_size(i); });
}

/// Vertex \p v of the some geometry \p a.
template <dim_t Ad>
constexpr associated::point_t<some<Ad>> vertex(some<Ad> const& a,
                                               dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(a), "some geometry vertex {} out-of-bounds [0,{})",
             v, vertex_size(a));
  return visit(a, [&v](auto&& i) { return vertex(i, v); });
}

/// Axis-Aligned Bounding Box of the any geometry \p a.
template <dim_t Ad>
constexpr associated::aabb_t<some<Ad>> axis_aligned_bounding_box(
 some<Ad> const& a) noexcept {
  return visit([](auto&& i) { return bounding_volume.aabb(i); }, a);
}

}  // namespace hm3::geometry::some_primitive
