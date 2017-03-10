#pragma once
/// \file
///
/// Line segment.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/collinear.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/primitive/line.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

/// Segment primitive.
namespace segment_primitive {

/// Line segment: line bounded by two end-points.
///
/// \tparam Ad   Ambient space dimension.
template <dim_t Ad>  //
struct segment {
  using data_t                  = array<point<Ad>, 2>;
  using geometry_type           = trait::segment<Ad>;
  using num_type                = num_t;
  using point_value_type        = point<Ad>;
  using point_reference         = point_value_type&;
  using point_const_reference   = point_value_type const&;
  using line_type               = geometry::line<Ad>;
  using vector_type             = associated::vector_t<point_value_type>;
  using edge_value_type         = segment<Ad>;
  using edge_reference          = edge_value_type&;
  using edge_const_reference    = edge_value_type const&;
  using concatenated_value_type = polyline<Ad>;

  array<point<Ad>, 2> xs_;

  /// Access the endpoint \p i of the segment.
  ///
  /// \pre \p i must be in range [0, 2).
  constexpr point_reference x(dim_t i) noexcept {
    HM3_ASSERT(i < 2, "index {} out-of-bounds [0, 2)", i);
    return xs_[i];
  }

  /// Access the endpoint \p i of the segment.
  ///
  /// \pre \p i must be in range [0, 2).
  constexpr point_const_reference x(dim_t i) const noexcept {
    HM3_ASSERT(i < 2, "index {} out-of-bounds [0, 2)", i);
    return xs_[i];
  }

  /// Obtains an unbounded line from the segment.
  constexpr line_type line() const noexcept {
    return line_type(x(dim_t{0}), x(dim_t{1}));
  }

  template <typename At>
  void assert_valid(At&& at) const noexcept {
    HM3_ASSERT_AT(
     !approx_point(x(0), x(1), 0., 0.),
     "segment through two equal points:\n x(0) = {}\n x(1) = {}\n!",
     std::forward<At>(at), x(0), x(1));
  }

  /// Construct a segment from the range of points \p rng.
  ///
  /// \pre `size(rng) == 2`.
  template <typename Rng, typename VT = ranges::range_value_t<uncvref_t<Rng>>,
            CONCEPT_REQUIRES_(Range<Rng>{} and !Same<VT, segment<Ad>>{}
                              and Same<point_value_type, VT>{})>
  constexpr segment(Rng&& rng) noexcept {
    HM3_ASSERT(ranges::size(rng) == 2, "");
    auto it = ranges::begin(rng);
    xs_[0]  = *it;
    ++it;
    xs_[1] = *it;
    assert_valid(HM3_AT_);
  }

  /// Line segment that goes through the 2 points \p b and \p e : [b, e]
  constexpr segment(point_value_type from, point_value_type to)
   : xs_{{from, to}} {
    assert_valid(HM3_AT_);
  }
  /// Line segment at \p p in direction \p dir : [p, p+dir]
  constexpr segment(point_value_type p, vector_type dir) noexcept
   : xs_{{p, point_value_type(p() + dir())}} {
    HM3_ASSERT(dir != vector_type::constant(0.),
               "cannot create segment from a point {} and zero direction {}!",
               p, dir);
  }

  constexpr segment(data_t d) : xs_(std::move(d)) { assert_valid(HM3_AT_); }

  constexpr segment()               = default;
  constexpr segment(segment const&) = default;
  constexpr segment(segment&&)      = default;
  constexpr segment& operator=(segment const&) = default;
  constexpr segment& operator=(segment&&) = default;
  ~segment()                              = default;

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <dim_t Ad>
constexpr bool operator==(segment<Ad> const& l, segment<Ad> const& r) noexcept {
  return l.x(0) == r.x(0) and l.x(1) == r.x(1);
}

template <dim_t Ad>
constexpr bool operator!=(segment<Ad> const& l, segment<Ad> const& r) noexcept {
  return !(l == r);
}

/// \name VertexAccess
///@{

/// Number of vertices in a segment
template <dim_t Ad>
constexpr dim_t vertex_size(segment<Ad> const&) noexcept {
  return 2;
}

/// Vertex \p v of the segment \p p
template <typename P, typename Pu = uncvref_t<P>,
          CONCEPT_REQUIRES_(std::is_same<Pu, point<ambient_dimension(Pu{})>>{})>
constexpr decltype(auto) vertex(P&& p,
                                associated::vertex_index_t<P> v) noexcept {
  HM3_ASSERT(v == 0, "");
  return std::forward<P>(p);
}

/// Vertex \p v of the segment \p s.
template <dim_t Ad>
constexpr point<Ad> vertex(segment<Ad> const& s, dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(s), "segment vertex {} out of bounds [0, 2)", v);
  return s.x(v);
}

///@} // VertexAccess

/// \name EdgeAccess
///@{

/// Number of edges in a segment (always 1).
template <dim_t Ad>
constexpr dim_t edge_size(segment<Ad> const&) noexcept {
  return 1;
}

/// Edge \p f of the segment \p s.
template <dim_t Ad>
constexpr segment<Ad>& edge(segment<Ad>& s, dim_t f) noexcept {
  HM3_ASSERT(f < edge_size(s), "segment edge {} out-of-bounds [0, 1)", f);
  return s;
}

/// Edge \p f of the segment \p s.
template <dim_t Ad>
constexpr segment<Ad> const& edge(segment<Ad> const& s, dim_t f) noexcept {
  HM3_ASSERT(f < edge_size(s), "segment edge {} out-of-bounds [0, 1)", f);
  return s;
}

/// Edge \p f of the segment \p s.
template <dim_t Ad>
constexpr segment<Ad> edge(segment<Ad>&& s, dim_t f) noexcept {
  HM3_ASSERT(f < edge_size(s), "segment edge {} out-of-bounds [0, 1)", f);
  return std::move(s);
}

///@}  // EdgeAccess

template <dim_t Ad>
constexpr optional<segment<Ad>> merge_collinear_adjacent_segments(
 segment<Ad> const& s0, segment<Ad> const& s1) {
  return segment<Ad>(s0.x(0), s1.x(1));
}

}  // namespace segment_primitive

using segment_primitive::segment;

/// Makes a closed segment range: last point equals the first
template <typename Rng>
auto make_segment_range(Rng&& vertex_rng) {
  HM3_ASSERT(ranges::size(vertex_rng) > 1,
             "at least two vertices are required!");
  return view::ints(std::size_t{0}, std::size_t(ranges::size(vertex_rng)))
         | view::transform([=](auto idx) {
             auto nidx
              = idx == std::size_t(ranges::size(vertex_rng)) - 1 ? 0 : idx + 1;
             using S
              = associated::edge_t<ranges::range_value_t<uncvref_t<Rng>>>;
             return S(ranges::at(vertex_rng, idx),
                      ranges::at(vertex_rng, nidx));
           });
}

/// Makes an open segment range
template <typename Rng>
auto make_open_segment_range(Rng&& rng) {
  HM3_ASSERT(ranges::size(rng) > 1, "at least two vertices are required!");
  return view::ints(std::size_t{0}, std::size_t(ranges::size(rng)) - 1)
         | view::transform([rng](auto idx) {
             auto nidx = idx + 1;
             using S
              = associated::edge_t<ranges::range_value_t<uncvref_t<Rng>>>;
             return S(ranges::at(rng, idx), ranges::at(rng, nidx));
           });
}

}  // namespace hm3::geometry
