#pragma once
/// \file
///
/// Polygon ostream operator.
#include <hm3/geometry/discrete/primitive/polygon/concept.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace polygon_primitive {

template <typename OStream, typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
OStream& write_to_ostream(OStream& os, P&& p) {
  os << "[";
  dim_t count = 0;
  for (auto&& v : vertices(p)) {
    os << count << "{ " << v << "}";
    if (count != vertex_size(p) - 1) { os << ", "; }
    ++count;
  }
  os << "]";
  return os;
}

}  // namespace polygon_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
