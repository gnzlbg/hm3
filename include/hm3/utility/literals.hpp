#pragma once
/// \file literals.hpp
#include <cstddef>

namespace hm3 {

constexpr std::size_t operator"" _z(unsigned long long int t) { return t; }

}  // namespace hm3
