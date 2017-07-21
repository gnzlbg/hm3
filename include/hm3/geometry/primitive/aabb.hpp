#pragma once
/// \file
///
/// Axis-Aligned Bounding Box.
#include <hm3/geometry/aabb_constants.hpp>
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>
#include <hm3/geometry/algorithm/centroid/aabb.hpp>
#include <hm3/geometry/concept/aabb.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/io/ascii.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/geometry/primitive/vec.hpp>

namespace hm3::geometry {

/// Axis-Aligned Bounding Box Primitive.
namespace aabb_primitive {

/// Axis-Aligned Bounding Box.
///
/// \tparam Ad Number of spatial dimensions.
template <dim_t Ad>
struct aabb {
  using self          = aabb<Ad>;
  using geometry_type = trait::aabb<Ad>;
  using point_t       = point<Ad>;
  using vector_t      = vec<Ad>;
  using data_t        = array<point_t, 2>;

 private:
  data_t x_;

 public:
  constexpr aabb()            = default;
  constexpr aabb(aabb const&) = default;
  constexpr aabb(aabb&&)      = default;
  constexpr aabb& operator=(aabb const&) = default;
  constexpr aabb& operator=(aabb&&) = default;
  ~aabb()                           = default;

  /// AABB from \p x_min and \p x_max.
  constexpr aabb(point_t const& x_min, point_t const& x_max)
   : x_{{x_min, x_max}} {
    assert_valid();
  }

  /// Minimum vertex
  constexpr point_t const& x_min() const noexcept { return x_[0]; }
  constexpr point_t& x_min() noexcept { return x_[0]; }

  /// Maximum vertex
  constexpr point_t const& x_max() const noexcept { return x_[1]; }
  constexpr point_t& x_max() noexcept { return x_[1]; }

 private:
  /// AABB from centroid \p x_c and \p lengths.
  static constexpr self from_centroid_and_length_(point_t const& x_c,
                                                  vector_t const& lengths) {
    for (dim_t d = 0; d < Ad; ++d) {
      HM3_ASSERT(lengths(d) > 0., "aabb length is {} !> 0!", lengths(d));
    }
    return self(point_t{x_c() - .5 * lengths()},
                point_t{x_c() + .5 * lengths()});
  }

 public:
  /// AABB from centroid \p x_c and \p lengths.
  constexpr aabb(point_t const& x_c, vector_t const& lengths)
   : aabb(from_centroid_and_length_(x_c, lengths)) {
    assert_valid();
  }

  /// Unit AABB
  static constexpr self unit() noexcept {
    return self{point_t::constant(0.), point_t::constant(1.)};
  }

  void assert_valid() const noexcept {
    HM3_ASSERT(x_min() != x_max(), "aabb min/max bounds are equal: {}",
               x_min());

    HM3_ASSERT(((x_min()().array() - x_max()().array()).abs() > 0.).all(),
               "abs(min(i) - max(i)) !>0. => aabb has length zero across one "
               "dimension! | min {} | max {}",
               x_min(), x_max());

    HM3_ASSERT(
     (x_min()().array() < x_max()().array()).all(),
     "min !<= max => aabb corners not properly ordered! | min {} | max {}",
     x_min(), x_max());
  }

  /// \name Model Segment<1>
  ///@{

  CONCEPT_REQUIRES(Ad == 1)
  auto line() const noexcept { return segment<1>(x_min(), x_max()).line(); }

  CONCEPT_REQUIRES(Ad == 1)
  auto x(dim_t i) const noexcept {
    HM3_ASSERT(i < 2, "vertex index {} is out-of-bounds [0, 2)", i);
    return i == 0 ? x_min() : x_max();
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
constexpr bool operator==(aabb<Ad> const& l, aabb<Ad> const& r) noexcept {
  return x_min(l) == x_min(r) and x_max(l) == x_max(r);
}

template <dim_t Ad>
constexpr bool operator!=(aabb<Ad> const& l, aabb<Ad> const& r) noexcept {
  return !(l == r);
}

/// Minimum vertex
template <typename T, typename UT = uncvref_t<T>, dim_t Ad = ad_v<UT>,
          CONCEPT_REQUIRES_(Same<UT, aabb<Ad>>{})>
constexpr decltype(auto) x_min(T&& s) noexcept {
  return std::forward<T>(s).x_min();
}

/// Maximum vertex
template <typename T, typename UT = uncvref_t<T>, dim_t Ad = ad_v<UT>,
          CONCEPT_REQUIRES_(Same<UT, aabb<Ad>>{})>

constexpr decltype(auto) x_max(T&& s) noexcept {
  return std::forward<T>(s).x_max();
}

/// Number of vertices in an AABB.
template <dim_t Ad>
constexpr dim_t vertex_size(aabb<Ad> const&) noexcept {
  return aabb_constants::vertex_size(Ad);
}

/// Vertex \p v of the AABB \p s.
template <dim_t Ad>
constexpr point<Ad> vertex(aabb<Ad> const& s, dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(s), "vidx {} is out-of-bounds [0, {})", v,
             vertex_size(s));
  return point<Ad>{
   centroid_aabb(s)().array()
   + 0.5 * bounding_length_aabb.all(s)().array()
      * aabb_constants::relative_vertex_position<Ad>(v)().array()};
}

/// Vertices of the AABB \p s.
template <dim_t Ad, dim_t Nvxs = vertex_size(aabb<Ad>{})>
constexpr const array<point<Ad>, Nvxs> vertices(aabb<Ad> const& s) noexcept {
  array<point<Ad>, Nvxs> vxs{};
  auto ls                 = bounding_length_aabb.all(s);
  const auto half_lengths = 0.5 * ls();
  const auto x_c          = centroid_aabb(s);
  for (suint_t c = 0; c < Nvxs; ++c) {
    const auto x_p = aabb_constants::relative_vertex_position<Ad>(c);
    vxs[c]         = x_c().array() + half_lengths.array() * x_p().array();
  }
  return vxs;
}

/// Number of 1D edges in an 2D/3D AABB.
template <dim_t Ad>
constexpr dim_t edge_size(aabb<Ad> const&) noexcept {
  return aabb_constants::face_size(Ad, 1);
}

/// Edge \p e of the aabb \p b
template <dim_t Ad>
constexpr auto edge(aabb<Ad> const& b, dim_t i) noexcept {
  HM3_ASSERT(i < edge_size(b), "index {} out-of-bounds: [0,{})", i,
             edge_size(b));
  auto vxs = aabb_constants::edge_vertices(i, Ad);
  return segment<Ad>(vertex(b, first(vxs)), vertex(b, second(vxs)));
}

/// Number of 2D faces in a 3D AABB.
constexpr dim_t face_size(aabb<3> const&) noexcept {
  HM3_ASSERT(aabb_constants::face_size(3, 2) == 6, "fs: {}",
             aabb_constants::face_size(3, 2));
  return aabb_constants::face_size(3, 2);
}

/// Returns the \p fidx face of the aabb \p a.
inline polygon<3> face(aabb<3> const& a, dim_t fidx) noexcept {
  return polygon<3>(make_segment_range(
   view::ints(dim_t{0}, dim_t{4}) | view::transform([&](auto&& i) {
     return vertex(a, aabb_constants::face_vertices(fidx, i));
   })));
}

}  // namespace aabb_primitive

using aabb_primitive::aabb;

}  // namespace hm3::geometry
