#pragma once
/// \file
///
/// Segments of a polygon
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/geometry/segment/segment.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace geometry {

/// Segments of a polygon
template <typename P,
          CONCEPT_REQUIRES_(Polygon<P>{} and std::is_lvalue_reference<P&&>{})>
constexpr auto segments(P&& polygon) noexcept {
  constexpr auto nd = uncvref_t<P>::dimension();
  return vertex_indices(polygon) | view::transform([&](auto l) {
           auto r = (l == size(vertex_indices(polygon)) - 1) ? 0 : l + 1;
           return segment<nd>(vertex(polygon, l), vertex(polygon, r));
         });
}

/// Segments of a polygon
template <typename P,
          CONCEPT_REQUIRES_(Polygon<P>{} and std::is_rvalue_reference<P&&>{})>
constexpr auto segments(P&& polygon) noexcept {
  constexpr auto nd = uncvref_t<P>::dimension();
  return vertex_indices(polygon) | view::transform([polygon](auto l) {
           auto r = (l == size(vertex_indices(polygon)) - 1) ? 0 : l + 1;
           return segment<nd>(vertex(polygon, l), vertex(polygon, r));
         });
}

}  // namespace geometry
}  // namespace hm3
