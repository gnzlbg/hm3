#pragma once
/// \file
///
/// Vector.
#include <hm3/geometry/discrete/concepts.hpp>
#include <hm3/geometry/discrete/primitive/point/point.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

/// Vector primitive.
namespace vector_primitive {

/// Nd-dimensional Vector.
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>  //
struct vector
 : point_primitive::point<Nd> {  // TODO: the rank of a vector is not zero?
  using point_t = point_primitive::point<Nd>;
  using point_t::point_t;
  using point_t::operator=;

  explicit vector(point_t p) noexcept : point_t(p) {}
  explicit operator point_t() const noexcept {
    return point_t{static_cast<point_t const&>(*this)};
  }
};

/// Length of the vector.
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Same<UV, vector<Nd>>{})>
constexpr num_t length(V&& v) noexcept {
  return v().norm();
}

/// Direction of the vector (the vector itself).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Same<UV, vector<Nd>>{})>
constexpr decltype(auto) direction(V&& v) noexcept {
  return v;
}

/// Normal to the vector (2D only).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Same<UV, vector<Nd>>{})>
constexpr UV normal(V&& v) noexcept {
  return UV{-v(1), v(0)};
}

/// Vector cross-product (3D only).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 3 and Same<UV, vector<Nd>>{})>
constexpr UV cross(V&& v0, V&& v1) noexcept {
  return UV{v0().cross(v1())};
}

}  // namespace vector_primitive

using vector_primitive::vector;

}  // namespace discrete

using discrete::vector;

}  // namespace geometry
}  // namespace hm3
