#pragma once
/// \file
///
/// Includes the nlohmann;s json library.
#include <json.hpp>

namespace nlohmann {

template <typename OStream>
constexpr OStream& to_ascii(OStream& os, json const& t) {
  os << t.dump(2);
  return os;
}

}  // namespace nlohmann
