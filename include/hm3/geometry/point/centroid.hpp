#pragma once
/// \file
///
/// Centroid of a point
#include <hm3/geometry/point/point.hpp>

namespace hm3 {
namespace geometry {

/// Centroid of a point
template <typename P, typename Pu = uncvref_t<P>,
          CONCEPT_REQUIRES_(std::is_same<Pu, point<Pu::dimension()>>{})>
constexpr decltype(auto) centroid(P&& p) noexcept {
  return std::forward<P>(p);
}

}  // namespace geometry
}  // namespace hm3
