#pragma once
/// \file
///
/// Polyline ostream operator.
#include <hm3/geometry/concepts.hpp>

namespace hm3::geometry::polyline_primitive {

template <typename OStream, typename P, CONCEPT_REQUIRES_(Polyline<P>{})>
OStream& write_to_ostream(OStream& os, P&& p) {
  os << "[";
  dim_t count = 0;
  for (auto&& v : vertices(p)) {
    os << "v" << count << ": " << v;
    if (count != vertex_size(p) - 1) { os << ", "; }
    ++count;
  }
  os << "]";
  return os;
}

}  // namespace hm3::geometry::polyline_primitive
