#pragma once
/// \file
///
/// Access to vertices of a point
#include <hm3/geometry/point/point.hpp>

namespace hm3 {
namespace geometry {

/// Vertex of a point
template <typename P, typename Pu = uncvref_t<P>,
          CONCEPT_REQUIRES_(std::is_same<Pu, point<Pu::dimension()>>{})>
constexpr decltype(auto) vertex(P&& p, suint_t v) noexcept {
  HM3_ASSERT(v == 0, "");
  return std::forward<P>(p);
}

/// Vertices of a point
template <typename P, typename Pu = uncvref_t<P>,
          CONCEPT_REQUIRES_(std::is_same<Pu, point<Pu::dimension()>>{}
                            and std::is_rvalue_reference<P>{})>
constexpr auto vertices(P&& p) noexcept {
  return array<Pu, 1>{std::forward<P>(p)};
}

/// Vertices of a point
template <typename P, typename Pu = uncvref_t<P>,
          CONCEPT_REQUIRES_(std::is_same<Pu, point<Pu::dimension()>>{}
                            and !std::is_rvalue_reference<P>{})>
constexpr auto vertices(P&& p) noexcept {
  return view::single(ranges::ref(std::forward<P>(p)))
         | view::transform(ranges::unwrap_reference);
}

}  // namespace geometry
}  // namespace hm3
