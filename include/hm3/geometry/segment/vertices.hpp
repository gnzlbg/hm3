#pragma once
/// \file
///
/// Line segment vertices
#include <hm3/geometry/segment/segment.hpp>

namespace hm3 {
namespace geometry {

/// Vertex of a segment
template <typename L, typename Lu = uncvref_t<L>,
          CONCEPT_REQUIRES_(std::is_same<Lu, segment<Lu::dimension()>>{})>
constexpr decltype(auto) vertex(L&& l, suint_t v) noexcept {
  HM3_ASSERT(v == 0 || v == 1, "");
  return v == 0 ? l.x_0 : l.x_1;
}

/// Vertices of a segment
template <typename L, typename Lu = uncvref_t<L>,
          CONCEPT_REQUIRES_(std::is_same<Lu, segment<Lu::dimension()>>{}
                            and std::is_rvalue_reference<L>{})>
constexpr auto vertices(L&& l) noexcept {
  return array<Lu, 2>{l.x_0, l.x_1};
}

/// Vertices of a segment
template <typename L, typename Lu = uncvref_t<L>,
          CONCEPT_REQUIRES_(std::is_same<Lu, segment<Lu::dimension()>>{}
                            and !std::is_rvalue_reference<L>{})>
constexpr auto vertices(L&& l) noexcept {
  return view::iota(0, 2) | view::transform([l_ = ref(l)](auto&& v)->decltype(
                             auto) { return vertex(l_.get(), v); });
}

}  // namespace geometry
}  // namespace hm3
