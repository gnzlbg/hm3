#pragma once
/// \file
///
/// Access to "faces" of a segment (that is, its vertices).
#include <hm3/geometry/access/face.hpp>
#include <hm3/geometry/primitive/segment/vertices.hpp>

namespace hm3::geometry::segment_primitive {

/// Number of faces in a segment.
template <dim_t Nd>  //
constexpr dim_t face_size(segment<Nd> const&) noexcept {
  return vertex_size(segment<Nd>{});
}

/// Face \p f of the segment \p s.
template <dim_t Nd>  //
constexpr point<Nd> face(segment<Nd> const& s, dim_t f) noexcept {
  HM3_ASSERT(f < face_size(s), "segment face {} out-of-bounds [0, 2)", f);
  return vertex(s, f);
}

/// Faces of the segment \p s.
template <dim_t Nd>  //
constexpr auto const& faces(segment<Nd> const& s) noexcept {
  return vertices(s);
}

}  // namespace hm3::geometry::segment_primitive
