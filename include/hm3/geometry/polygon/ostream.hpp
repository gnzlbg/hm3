#pragma once
/// \file
///
/// Polygon ostream operator
#include <hm3/geometry/polygon/polygon.hpp>

namespace hm3 {
namespace geometry {

template <typename OStream, dim_t Nd, ppidx_t MaxNp>
OStream& operator<<(OStream& os, polygon<Nd, MaxNp> const& pol) {
  os << "[";
  ppidx_t c = 0;

  for (auto&& p : pol) {
    os << "x_" << c << "{ ";
    for (auto d : dimensions(Nd)) {
      os << p(d);
      if (d != Nd - 1) { os << ", "; }
    }
    os << "}";
    if (c != pol.size() - 1) { os << ", "; }
    ++c;
  }

  os << "]";
  return os;
}

}  // namespace geometry
}  // namespace hm3
