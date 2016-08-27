#pragma once
/// \file
///
/// Access to "faces" of a polygon (that is, its edges).
#include <hm3/geometry/access/face.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::polyline_primitive {

/// Number of faces of the polyline \p p.
template <typename P, CONCEPT_REQUIRES_(Polyline<P>{})>
constexpr dim_t face_size(P&& p) noexcept {
  HM3_ASSERT(vertex_size(p) > 1, "");
  return vertex_size(p) - 1;
}

/// Face \p f of the polyline \p p.
template <typename P, dim_t Nd = uncvref_t<P>::dimension(),
          CONCEPT_REQUIRES_(Polyline<P, Nd>{})>
constexpr segment<Nd> face(P&& p, dim_t f) noexcept {
  const auto no_vertices = vertex_size(p);
  HM3_ASSERT(f < no_vertices - 1, "face {} out of bounds [0, {})", f,
             no_vertices - 1);
  return segment<Nd>(vertex(p, f), vertex(p, f + 1));
}

/// Faces of the polyline \p p.
template <typename P,
          CONCEPT_REQUIRES_(Polyline<P>{} and !std::is_rvalue_reference<P&&>{})>
constexpr auto faces(P&& p) noexcept {
  return view::iota(dim_t{0}, face_size(p))
         | view::transform([&p](auto&& fidx) { return face(p, fidx); });
}

/// Faces of the polyline \p p.
template <typename P,
          CONCEPT_REQUIRES_(Polyline<P>{} and std::is_rvalue_reference<P&&>{})>
constexpr auto faces(P&& p) noexcept {
  return view::iota(dim_t{0}, face_size(p))
         | view::transform([p](auto&& fidx) { return face(p, fidx); });
}

}  // namespace hm3::geometry::polyline_primitive
