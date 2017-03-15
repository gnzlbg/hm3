#pragma once
/// \file
///
/// Signed-distance function (common boilerplate and how to make it).
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/primitive_hierarchy.hpp>
#include <hm3/utility/ref.hpp>

namespace hm3::geometry::sd {

/// Signed-distance field.
template <dim_t Ad>
struct signed_distance : with_ambient_dimension<Ad> {
  using geometry_type = trait::signed_distance<Ad>;
};

/// Wrapper over signed distance functions:
template <typename F, dim_t Ad>
struct signed_distance_function : signed_distance<Ad> {
  ref_t<F> f_;

  template <typename U>
  explicit signed_distance_function(U&& u) : f_(std::forward<U>(u)) {}

  template <typename P>
  constexpr num_t operator()(P&& p) const noexcept {
    return f_.get()(std::forward<P>(p));
  }
};

template <dim_t Ad, typename F>
auto make(dim_vt<Ad>, F&& f) noexcept {
  return signed_distance_function<F&&, Ad>(std::forward<F>(f));
}

template <typename F>
auto make(F&& f) noexcept {
  return make(ad_v<F>, std::forward<F>(f));
}

}  // namespace hm3::geometry::sd
