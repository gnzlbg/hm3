#pragma once
/// \file
///
/// Direction of a vector.
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/primitive/vec/vec.hpp>

namespace hm3::geometry::vec_primitive {

/// Direction of the vec (the vec itself).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Same<UV, vec<Nd>>{})>
constexpr decltype(auto) direction(V&& v) noexcept {
  return std::forward<V>(v);
}

}  // namespace hm3::geometry::vec_primitive
