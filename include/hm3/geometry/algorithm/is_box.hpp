#pragma once
/// \file
///
/// Is a primitive a box?
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>

namespace hm3::geometry {

namespace is_box_detail {

struct is_box_fn {
  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const&, num_t, num_t, trait::box<Ad>) noexcept {
    return true;
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto impl(T const& t, num_t abs_tol, num_t rel_tol,
                             trait::aabb<Ad>) noexcept {
    auto l = bounding_length_aabb.all(t);
    return ranges::all_of(
     l, [&](auto i) { return approx_number(i, l(0), abs_tol, rel_tol); });
  }

  template <typename T>
  constexpr auto operator()(T&& t, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), abs_tol,
                                             rel_tol, associated::v_<T>));
};

}  // namespace is_box_detail

namespace {
static constexpr auto const& is_box
 = static_const<with_default_tolerance<is_box_detail::is_box_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
