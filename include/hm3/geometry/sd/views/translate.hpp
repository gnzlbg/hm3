#pragma once
/// \file
///
/// Inverse of a signed distance field
#include <hm3/geometry/sd/core.hpp>

namespace hm3::geometry::sd {

namespace detail {

struct op_translate_fn {
  template <typename Point, typename V, typename SDF, dim_t Ad>
  constexpr auto operator()(Point&& x, V&& v, dim_vt<Ad>, SDF&& f) const {
    return f(math::inverse_translation(unwrap(v), dim_v<Ad>) * unwrap(x));
  }
};
}  // namespace detail

namespace {
static constexpr auto const& op_translate
 = static_const<detail::op_translate_fn>::value;
}  // namespace

namespace detail {
struct translate_fn {
  template <typename SDF, typename V>
  constexpr auto operator()(SDF&& f, V pos) const {
    return view(
     [pos](auto&& x, auto&& f) { return op_translate(x, pos, dim_v<V>, f); },
     std::forward<SDF>(f));
  }
};
}  // namespace detail

namespace {
static constexpr auto const& translate
 = static_const<detail::translate_fn>::value;
}  // namespace

}  // namespace hm3::geometry::sd
