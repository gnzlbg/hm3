#pragma once
/// \file
///
/// Simplify a geometric primitive.
#include <hm3/geometry/algorithm/simplify/polyline.hpp>

namespace hm3::geometry {

namespace simplify_detail {

struct simplify_fn {
  /// Simplify of the polyline \p p.
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, trait::polyline<Ad>) noexcept {
    static_assert(Polyline<uncvref_t<P>>{});
    return simplify_polyline(std::forward<P>(p));
  }

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             associated::v_<T>));
};

}  // namespace simplify_detail

namespace {
constexpr auto const& simplify
 = static_const<simplify_detail::simplify_fn>::value;
}  // namespace

}  // namespace hm3::geometry
