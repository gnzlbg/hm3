#pragma once
/// \file
///
/// OStream operator for vectors.
#include <hm3/geometry/primitive/vec/vec.hpp>

namespace hm3::geometry::vec_primitive {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, vec<Nd> const& p) {
  o << "(";
  for (dim_t d = 0; d < Nd; ++d) {
    o << std::setprecision(std::numeric_limits<num_t>::digits10 + 1) << p(d);
    if (d != Nd - 1) { o << ", "; }
  }
  o << ")";
  return o;
}

}  // namespace hm3::geometry::vec_primitive
