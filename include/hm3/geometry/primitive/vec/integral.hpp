#pragma once
/// \file
///
/// Integral over a vector.
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/primitive/vec/vec.hpp>

namespace hm3::geometry::vec_primitive {

/// Length of the vec.
template <dim_t Nd>
constexpr num_t integral(vec<Nd> const& v, path_integral<vec<Nd>>) noexcept {
  return v().norm();
}

}  // namespace hm3::geometry::vec_primitive
