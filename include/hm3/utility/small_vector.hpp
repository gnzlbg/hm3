#pragma once
/// \file
///
/// A small vector.
#include <boost/container/small_vector.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

using boost::container::small_vector;

}  // namespace hm3

namespace boost::container {

template <typename OStream, typename T, std::size_t N, typename A>
OStream& operator<<(OStream& os, small_vector<T, N, A> const& v) {
  os << hm3::view::all(v);
  return os;
}

}  // namespace boost::container
