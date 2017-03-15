#pragma once
/// \file
///
/// Box: AABB with constant length.
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>
#include <hm3/geometry/algorithm/centroid/aabb.hpp>
#include <hm3/geometry/algorithm/is_box.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/concept/box.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/segment.hpp>

namespace hm3::geometry {

/// Box Primitive.
namespace box_primitive {

/// Box: AABB with constant length.
template <dim_t Ad>
struct box {
  using geometry_type = trait::box<Ad>;
  using aabb_t        = aabb<Ad>;
  using point_t       = point<Ad>;
  using vector_t      = vec<Ad>;

 private:
  point_t centroid_;
  num_t length_{};

 public:
  box()                     = default;
  constexpr box(box const&) = default;
  constexpr box(box&&)      = default;
  constexpr box& operator=(box const&) = default;
  constexpr box& operator=(box&&) = default;
  ~box()                          = default;

  /// Box from \p x_centroid and \p x_length.
  constexpr box(point_t x_centroid, num_t length)
   : centroid_(x_centroid), length_(length) {
    HM3_ASSERT(length > 0., "box length is {} !> 0.!", length);
    HM3_ASSERT(x_min(*this) == point_t(x_centroid().array() - 0.5 * length),
               "");
    HM3_ASSERT(x_max(*this) == point_t(x_centroid().array() + 0.5 * length),
               "");
  }

  /// Box from AABB \p b that is already a box (e.g. with all equal sides).
  explicit constexpr box(  // NOLINT(cppcoreguidelines-pro-type-member-init)
   aabb_t const& b)
   : box(centroid_aabb(b), bounding_length_aabb(b, 0)) {
    HM3_ASSERT(is_box(b), "AABB not a Box: {}", b);
  }

  /// Box from \p x_min and \p x_max.
  constexpr box(  // NOLINT(cppcoreguidelines-pro-type-member-init)
   point_t const& x_min, point_t const& x_max)
   : box(aabb_t(x_min, x_max)) {
    HM3_ASSERT(is_box(*this),
               "the points min: {} and max: {} do not span a box", x_min,
               x_max);
  }

 private:
  /// Box from centroid \p x_c and \p lengths.
  static constexpr box<Ad> from_centroid_and_lengths_(point_t const& x_c,
                                                      vector_t const& lengths) {
    return box<Ad>(aabb_t(x_c, lengths));
  }

 public:
  /// Box from centroid \p x_c and \p lengths.
  constexpr box(  // NOLINT(cppcoreguidelines-pro-type-member-init)
   point_t const& x_c, vector_t const& lengths)
   : box(from_centroid_and_lengths_(x_c, lengths)) {}

  /// Unit box.
  static constexpr box<Ad> unit() noexcept {
    return box<Ad>(point_t::constant(0.5), 1.);
  }

  constexpr num_t length() const noexcept { return length_; }
  constexpr point_t centroid() const noexcept { return centroid_; }

  /// \name Model Segment<1>
  ///@{

  CONCEPT_REQUIRES(Ad == 1)
  auto line() const noexcept {
    return segment<Ad>(x_min(*this), x_max(*this)).line();
  }

  CONCEPT_REQUIRES(Ad == 1)
  auto x(dim_t i) const noexcept {
    HM3_ASSERT(i < 2, "vertex index {} is out-of-bounds [0, 2)", i);
    return i == 0 ? x_min(*this) : x_max(*this);
  }

  ///@}  // Model Segment<1>

  /// \name Model Polygon<2>
  ///@{

  CONCEPT_REQUIRES(Ad == 2)
  auto polyline() const noexcept {
    using p_t = ::hm3::geometry::polyline<2>;
    return p_t(edges(*this));
  }

  ///@}

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Ad>
bool operator==(box<Ad> const& l, box<Ad> const& r) noexcept {
  return math::approx(l.length(), r.length()) && l.centroid() == r.centroid();
}

template <dim_t Ad>
bool operator!=(box<Ad> const& l, box<Ad> const& r) noexcept {
  return !(l == r);
}

/// Minimum vertex
template <dim_t Ad>
constexpr point<Ad> x_min(box<Ad> const& s) noexcept {
  return point<Ad>{s.centroid()() - point<Ad>::c(.5 * s.length())};
}

/// Maximum vertex
template <dim_t Ad>
constexpr point<Ad> x_max(box<Ad> const& s) noexcept {
  return point<Ad>{s.centroid()() + point<Ad>::c(.5 * s.length())};
}

template <dim_t Ad>
constexpr bool is_box(box<Ad> const&) noexcept {
  return true;
}

/// Number of vertices in an box.
template <dim_t Ad>
constexpr dim_t vertex_size(box<Ad> const&) noexcept {
  return aabb_constants::vertex_size(Ad);
}

template <dim_t Ad>
constexpr dim_t edge_size(box<Ad> const&) noexcept {
  return aabb_constants::face_size(Ad, 1);
}

/// Vertex \p v of the box \p s.
template <dim_t Ad>
constexpr point<Ad> vertex(box<Ad> const& s, dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(s), "");
  return point<Ad>{
   s.centroid()().array()
   + 0.5 * s.length()
      * aabb_constants::relative_vertex_position<Ad>(v)().array()};
}

/// Vertices of the box \p s.
template <dim_t Ad, dim_t Nvxs = vertex_size(box<Ad>{})>
constexpr const array<point<Ad>, Nvxs> vertices(box<Ad> const& s) noexcept {
  array<point<Ad>, Nvxs> vxs;
  const auto half_length = 0.5 * s.length();
  for (suint_t c = 0; c < Nvxs; ++c) {
    const auto x_p = aabb_constants::relative_vertex_position<Ad>(c);
    vxs[c]         = s.centroid()().array() + half_length * x_p().array();
  }
  return vxs;
}

/// Edge \p e of the 1D box \p b
///
/// note: there is only one edge, the box itself.
template <dim_t Ad>
constexpr auto edge(box<Ad> const& b, dim_t i) noexcept {
  HM3_ASSERT(i < edge_size(b), "index {} out-of-bounds: [0,{})", i,
             edge_size(b));
  auto vxs = aabb_constants::edge_vertices(i, Ad);
  return segment<Ad>(vertex(b, first(vxs)), vertex(b, second(vxs)));
}

/// Number of faces of the box \p b.
constexpr dim_t face_size(box<3> const&) noexcept {
  HM3_ASSERT(aabb_constants::face_size(3, 2) == 6, "fs: {}",
             aabb_constants::face_size(3, 2));
  return aabb_constants::face_size(3, 2);
}

inline polygon<3> face(box<3> const& a, dim_t fidx) noexcept {
  return polygon<3>(make_segment_range(
   view::ints(dim_t{0}, dim_t{4}) | view::transform([&](auto&& i) {
     return vertex(a, aabb_constants::face_vertices(fidx, i));
   })));
}

}  // namespace box_primitive

using box_primitive::box;

}  // namespace hm3::geometry
