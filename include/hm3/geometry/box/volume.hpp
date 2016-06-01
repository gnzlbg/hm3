/// \file
///
/// AABB vertices
#include <hm3/geometry/box/box.hpp>

namespace hm3 {
namespace geometry {

/// Volume of the box \p s
template <dim_t Nd> constexpr auto volume(box<Nd> const& s) noexcept {
  auto vol     = length(s);
  const auto l = vol;
  for (dim_t d = 1; d < Nd; ++d) { vol *= l; }
  return vol;
}

}  // namespace geometry
}  // namespace hm3
