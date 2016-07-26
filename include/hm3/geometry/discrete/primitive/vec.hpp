#pragma once
/// \file
///
/// Vector primitive
#include <hm3/geometry/discrete/concepts.hpp>
#include <hm3/geometry/discrete/primitive/point/point.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

/// Vec primitive.
namespace vec_primitive {

/// Nd-dimensional Vec.
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>  //
struct vec
 : point_primitive::point<Nd> {  // TODO: the rank of a vec is not zero?
  using point_t = point_primitive::point<Nd>;
  using point_t::point_t;
  using point_t::operator=;

  explicit vec(point_t p) noexcept : point_t(p) {}
  explicit operator point_t() const noexcept {
    return point_t{static_cast<point_t const&>(*this)};
  }

 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/// Length of the vec.
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Same<UV, vec<Nd>>{})>
constexpr num_t length(V&& v) noexcept {
  return v().norm();
}

/// Direction of the vec (the vec itself).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Same<UV, vec<Nd>>{})>
constexpr decltype(auto) direction(V&& v) noexcept {
  return v;
}

/// Normal to the vec (2D only).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Same<UV, vec<Nd>>{})>
constexpr UV normal(V&& v) noexcept {
  return UV{-v(1), v(0)};
}

/// Vec cross-product (3D only).
template <typename V, typename UV = uncvref_t<V>, dim_t Nd = UV::dimension(),
          CONCEPT_REQUIRES_(Nd == 3 and Same<UV, vec<Nd>>{})>
constexpr UV cross(V&& v0, V&& v1) noexcept {
  return UV{v0().cross(v1())};
}

}  // namespace vec_primitive

using vec_primitive::vec;

}  // namespace discrete

using discrete::vec;

}  // namespace geometry
}  // namespace hm3
