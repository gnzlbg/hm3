#pragma once
/// \file
///
/// Triangle vertices
#include <hm3/geometry/triangle/triangle.hpp>

namespace hm3 {
namespace geometry {

/// Vertex of a triangle
template <typename T, typename Tu = uncvref_t<T>,
          CONCEPT_REQUIRES_(std::is_same<Tu, triangle<Tu::dimension()>>{})>
constexpr decltype(auto) vertex(T&& t, suint_t v) noexcept {
  HM3_ASSERT(v >= 0 and v < 3, "");
  return t.vertices[v];
}

/// Vertices of a triangle
template <typename T, typename Tu = uncvref_t<T>,
          CONCEPT_REQUIRES_(std::is_same<Tu, triangle<Tu::dimension()>>{}
                            and std::is_rvalue_reference<T>{})>
constexpr auto vertices(T&& t) noexcept {
  return array<Tu, 3>{t.vertices};
}

/// Vertices of a triangle
template <typename T, typename Tu = uncvref_t<T>,
          CONCEPT_REQUIRES_(std::is_same<Tu, triangle<Tu::dimension()>>{}
                            and !std::is_rvalue_reference<T>{})>
constexpr auto vertices(T&& t) noexcept {
  return t.vertices;
}

}  // namespace geometry
}  // namespace hm3
