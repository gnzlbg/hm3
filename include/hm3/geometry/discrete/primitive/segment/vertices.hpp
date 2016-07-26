#pragma once
/// \file
///
/// Access to vertices of a segment.
#include <hm3/geometry/discrete/access/vertex.hpp>
#include <hm3/geometry/discrete/primitive/segment/segment.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace segment_primitive {

/// Number of vertices in a segment.
template <dim_t Nd>  //
constexpr dim_t vertex_size(segment<Nd> const&) noexcept {
  return 2;
}

/// Vertex \p v of the ssegment \p s.
template <dim_t Nd>  //
constexpr point<Nd> vertex(segment<Nd> const& s, dim_t v) noexcept {
  HM3_ASSERT(v < vertex_size(s), "");
  return v == 0 ? s.x(0) : s.x(1);
}

/// Vertices of the segment \p s.
template <dim_t Nd>  //
constexpr auto const& vertices(segment<Nd> const& s) noexcept {
  return s.xs_;
}

}  // namespace segment_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
