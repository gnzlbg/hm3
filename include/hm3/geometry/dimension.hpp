#pragma once
/// \file
///
/// Implements the Dimensional and NDimensional concepts.
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/config/fatal_error.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

/// Spatial dimension of an object.
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>  //
struct dimensional {
  static_assert(Nd > 0, "");
  static_assert(Nd <= 3, "more than 3 spatial dimensions are never tested");
  /// Spatial dimension.
  static constexpr std::integral_constant<dim_t, Nd> dimension() noexcept {
    return std::integral_constant<dim_t, Nd>{};
  }
  /// Range of spatial dimensions.
  static constexpr auto dimensions() noexcept {
    return view::iota(dim_t{0}, static_cast<dim_t>(dimension()));
  }
  HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(dimensions());

  /// Name of the spatial dimension \p d.
  static constexpr char dimension_name(dim_t d) noexcept {
    HM3_ASSERT(d >= 0 and d < Nd, "dimension {} out of bounds [0, {})", d, Nd);
    char n[3] = {'x', 'y', 'z'};
    return n[d];
  }
};

namespace geometry_detail {

template <dim_t Nd>  //
constexpr auto dimension(dimensional<Nd> const&) noexcept {
  return dimensional<Nd>::dimension();
}

struct dimension_fn {
  template <typename T>
  static constexpr auto impl(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(dimension(std::forward<T>(t)));
  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(t.dimension());
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  // namespace geometry_detail

namespace {
static constexpr auto const& dimension
 = static_const<geometry_detail::dimension_fn>::value;
}

template <typename T>
using dimension_t = decltype(dimension(std::declval<uncvref_t<T>>()));

template <typename T> static constexpr auto dimension_v = dimension_t<T>{};

namespace geometry_detail {

auto dimensions(dim_t nd)
 RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(view::iota(dim_t{0}, nd));

template <typename T>  //
constexpr auto dimensions(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
 view::iota(dim_t{0}, static_cast<dim_t>(hm3::geometry::dimension(t))));

struct dimensions_fn {
  template <typename T>
  static constexpr auto impl(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(dimensions(std::forward<T>(t)));
  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(t.dimensions());
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  //  namespace geometry_detail

namespace {
static constexpr auto const& dimensions
 = static_const<geometry_detail::dimensions_fn>::value;
}

namespace concepts {

namespace rc = ranges::concepts;

struct dimensional {
  template <typename T>
  auto requires_(T&& t) -> decltype(
   rc::valid_expr(rc::convertible_to<dim_t>(dimension(t)),            //
                  rc::model_of<rc::RandomAccessRange>(dimensions(t))  //
                  ));
};

template <typename T, typename IsDimensional, dim_t Nd>
struct ndimensional_ : meta::bool_<false> {};

template <typename T>
struct ndimensional_<T, meta::bool_<true>, math::highest<dim_t>>
 : meta::bool_<true> {};

template <typename T, dim_t Nd>
struct ndimensional_<T, meta::bool_<true>, Nd>
 : meta::bool_<dimension_v<T> == Nd> {};

using Dimensional = dimensional;

}  // namespace concepts

template <typename T>
using Dimensional_
 = meta::bool_<concepts::rc::models<concepts::Dimensional, uncvref_t<T>>{}>;

template <typename T, dim_t Nd = math::highest<dim_t>>
using Dimensional
 = meta::and_<Dimensional_<T>, concepts::ndimensional_<T, Dimensional_<T>, Nd>>;

}  // namespace geometry

using geometry::dimension;
using geometry::dimension_t;
using geometry::dimension_v;
using geometry::dimensions;

}  // namespace hm3
