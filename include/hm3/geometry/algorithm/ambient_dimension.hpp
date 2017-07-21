#pragma once
/// \file
///
/// Dimension of the ambient space a geometric primitive lives in.
#include <hm3/geometry/concept/associated_dimension.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>

namespace hm3::geometry {

namespace ambient_dimension_detail {

struct ambient_dimension_fn {
  template <dim_t Ad>
  static constexpr dim_vt<Ad> get_dim_(trait::ambient_dimension<Ad>) noexcept {
    return {};
  }

  template <typename T, typename U = associated::t_<T>>
  static constexpr auto get_dim(T&&)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(get_dim_(U{}));

  template <typename T, typename U = associated::t_<T>>
  constexpr auto operator()(T&&) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(get_dim_(U{}));

  template <typename T>
  constexpr auto operator[](T&& t) const noexcept {
    return view::iota(dim_t{0}, dim_t{(*this)(std::forward<T>(t))});
  }

  template <typename T>
  static constexpr char xyz_name(T&&, dim_t d) noexcept {
    constexpr dim_t ad = get_dim(T{});
    static_assert(ad > 0 and ad <= 3, "unimplemented");
    constexpr char n[3] = {'x', 'y', 'z'};
    HM3_ASSERT(d >= 0 and d < 3, "d = {} is out-of-bounds [0, 3)", d);
    return ranges::index(n, d);
  }

  template <typename T>
  static string basis_name(T&&, dim_t d) noexcept {
    constexpr dim_t ad = get_dim(T{});
    static_assert(ad > 0 and ad <= 3, "unimplemented");
    constexpr char n[3] = {'0', '1', '2'};
    HM3_ASSERT(d >= 0 and d < 3, "d = {} is out-of-bounds [0, 3)", d);
    string result({'e', ranges::index(n, d)});
    return result;
  }
};

}  // namespace ambient_dimension_detail

namespace {
constexpr auto const& ambient_dimension
 = static_const<ambient_dimension_detail::ambient_dimension_fn>::value;
}  // namespace

template <typename T>
using ambient_dimension_t = decltype(ambient_dimension(std::declval<T>()));

template <typename T>
static constexpr auto ambient_dimension_v = ambient_dimension_t<uncvref_t<T>>{};

template <dim_t Ad>
struct with_ambient_dimension {
  using geometry_type = trait::ambient_dimension<Ad>;
  static constexpr dim_vt<Ad> ambient_dimension() noexcept { return {}; }
  static constexpr auto ambient_dimensions() noexcept {
    return view::iota(dim_t{0}, dim_t{Ad});
  }
};

template <typename T>
using with_ambient_dimension_t = with_ambient_dimension<ambient_dimension_v<T>>;

}  // namespace hm3::geometry

namespace hm3 {

template <typename T>
using ad_t = geometry::ambient_dimension_t<T>;

template <typename T>
static constexpr auto ad_v = ad_t<T>{};

}  // namespace hm3
