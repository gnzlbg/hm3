#pragma once
/// \file
///
/// Normal of a two-dimensional vector
#include <hm3/geometry/algorithm/normal.hpp>
#include <hm3/geometry/primitive/vec/vec.hpp>

namespace hm3::geometry::vec_primitive {

/// Normal to the vec (2D only).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Same<UV, vec<Nd>>{})>
constexpr UV normal(V&& v) noexcept {
  return UV{-v(1), v(0)};
}

}  // namespace hm3::geometry::vec_primitive
