#pragma once
/// \file
///
/// Access to vertices of a segment.
#include <hm3/geometry/access/vertex.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Number of vertices in a segment.
template <dim_t Nd>
constexpr dim_t vertex_size(segment<Nd> const&) noexcept {
  return 2;
}

/// Vertex \p v of the ssegment \p s.
template <dim_t Nd>
constexpr point<Nd> vertex(segment<Nd> const& s, dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(s), "segment vertex {} out of bounds [0, 2)", v);
  return s.x(v);
}

/// Vertices of the segment \p s.
template <dim_t Nd>
constexpr auto const& vertices(segment<Nd> const& s) noexcept {
  return s.xs_;
}

}  // namespace hm3::geometry::segment_primitive
