#pragma once
/// \file
///
/// The dimension of a geometric element within its ambient space.
#include <hm3/geometry/algorithm/ambient_dimension.hpp>

namespace hm3::geometry {

namespace element_dimension_detail {

struct element_dimension_fn {
  template <dim_t Ad>
  static constexpr dim_vt<Ad> get_dim_(trait::element_dimension<Ad>) noexcept {
    return {};
  }

  template <typename T>
  static constexpr auto get_dim(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(t.rank());

  template <typename T, typename U = associated::t_<T>>
  static constexpr auto get_dim(T&&, int = 0)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(get_dim_(U{}));

  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(get_dim(std::forward<T>(t), 0));

  template <typename T>
  constexpr auto operator[](T&& t) const noexcept {
    return view::iota(dim_t{0}, dim_t{(*this)(std::forward<T>(t))});
  }

  /// Name of the type of the geometric element of dimension \p r.
  template <typename T>
  static constexpr const char* name(T&& t, dim_t r) noexcept {
    dim_t ed = get_dim(std::forward<T>(t), 0);
    HM3_ASSERT(ed > 0 and ed <= 3, "unimplemented");
    HM3_ASSERT(r >= 0 and r < ed, "element dimension {} out of bounds [0, {})",
               r, ed);
    switch (r) {
      case 3: {
        return "volume";
      }
      case 2: {
        return "surface";
      }
      case 1: {
        return "curve";
      }
      case 0: {
        return "point";
      }
      default: { HM3_UNREACHABLE(); }
    }
  }
};

}  // namespace element_dimension_detail

namespace {
constexpr auto const& element_dimension
 = static_const<element_dimension_detail::element_dimension_fn>::value;
}

template <typename T>
using element_dimension_t = decltype(element_dimension(std::declval<T>()));

template <typename T>
static constexpr auto element_dimension_v = element_dimension_t<T>{};

}  // namespace hm3::geometry

namespace hm3 {

template <typename T>
using ed_t = geometry::element_dimension_t<T>;

template <typename T>
static constexpr auto ed_v = ed_t<T>{};

}  // namespace hm3
