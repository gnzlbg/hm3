#pragma once
/// \file
///
/// Properly aligned std::vector.
#include <hm3/utility/matrix/eigen.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

template <typename OStream, typename T, typename A>
OStream& operator<<(OStream& os, std::vector<T, A> const& v) {
  os << ranges::view::all(v);
  return os;
}

}  // namespace hm3
