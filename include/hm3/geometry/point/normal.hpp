#pragma once
/// \file
///
/// Normal of a 1D point
#include <hm3/geometry/point/point.hpp>

namespace hm3 {
namespace geometry {

/// Normal of a 1D point
template <typename P, typename Pu = uncvref_t<P>,
          CONCEPT_REQUIRES_(std::is_same<Pu, point<Pu::dimension()>>{}
                            and Pu::dimension() == 1)>
constexpr point<1> normal(P&&) noexcept {
  return point<1>::constant(1.);
}

}  // namespace geometry
}  // namespace hm3
