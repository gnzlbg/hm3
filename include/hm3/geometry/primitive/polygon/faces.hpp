#pragma once
/// \file
///
/// Access to "faces" of a polygon (that is, its edges).
#include <hm3/geometry/access/face.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::polygon_primitive {

/// Number of faces of the polygon \p p.
template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr dim_t face_size(P&& p) noexcept {
  return vertex_size(p);
}

/// Face \p f of the polygon \p p.
template <typename P, dim_t Nd = uncvref_t<P>::dimension(),
          CONCEPT_REQUIRES_(Polygon<P, Nd>{})>
constexpr segment<Nd> face(P&& p, dim_t f) noexcept {
  const auto no_vertices = vertex_size(p);
  HM3_ASSERT(f < no_vertices, "face {} out of bounds [0, {})", f, no_vertices);
  const auto vx0 = f;
  const auto vx1 = (vx0 == no_vertices - 1) ? 0 : f + 1;
  return segment<Nd>(vertex(p, vx0), vertex(p, vx1));
}

/// Faces of the polygon \p p.
template <typename P,
          CONCEPT_REQUIRES_(Polygon<P>{} and !std::is_rvalue_reference<P&&>{})>
constexpr auto faces(P&& p) noexcept {
  return view::iota(dim_t{0}, face_size(p))
         | view::transform([&p](auto&& fidx) { return face(p, fidx); });
}

/// Faces of the polygon \p p.
template <typename P,
          CONCEPT_REQUIRES_(Polygon<P>{} and std::is_rvalue_reference<P&&>{})>
constexpr auto faces(P&& p) noexcept {
  return view::iota(dim_t{0}, face_size(p))
         | view::transform([p](auto&& fidx) { return face(p, fidx); });
}

}  // namespace hm3::geometry::polygon_primitive
