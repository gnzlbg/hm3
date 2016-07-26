#pragma once
/// \file
///
/// Integral over discrete primitives.
#include <hm3/geometry/rank.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {

namespace detail {

/// Rank of the integral to be computed.
template <dim_t IR> using integral_rank = std::integral_constant<dim_t, IR>;

/// Volume integral.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam R  Geometry rank == Nd.
///
/// The integral rank equals the number of spatial dimensions.
///
template <dim_t Nd, dim_t R, CONCEPT_REQUIRES_(Nd == R)>
using volume_rank = integral_rank<R>;

template <typename T>
using volume_integral = volume_rank<dimension_v<T>, rank_v<T>>;

/// Area integral.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam R  Geometry rank == Nd - 1 (only Nd-"surfaces" have area).
///
/// Integral rank: equal the number of spatial dimensions / geometry rank.
///
template <dim_t Nd, dim_t R, CONCEPT_REQUIRES_(Nd - 1 == R)>
using surface_rank = integral_rank<R>;

template <typename T>
using surface_integral = surface_rank<dimension_v<T>, rank_v<T>>;

/// Boundary integral.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam R  Geometry rank.
///
/// Integral rank: equal R - 1.
///
/// \note If Geometry rank == 0 (i.e., for points), the boundary is zero.
///
template <dim_t Nd, dim_t R>
using boundary_rank = integral_rank<R == 0 ? math::highest<dim_t> : R - 1>;

template <typename T>
using boundary_integral = boundary_rank<dimension_v<T>, rank_v<T>>;

/// Path integral.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam R  Geometry rank == 1 (only paths have a length).
///
/// Integral rank: equal R.
///
template <dim_t Nd, dim_t R, CONCEPT_REQUIRES_(R == 1)>
using path_rank = integral_rank<R>;

template <typename T>
using path_integral = path_rank<dimension_v<T>, rank_v<T>>;

struct integral_fn {
  template <typename T, typename IntRank, CONCEPT_REQUIRES_(IntRank{} != 0)>
  static constexpr auto impl(T&& t, IntRank)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral(std::forward<T>(t),
                                                 IntRank{}));

  template <typename T, typename IntRank, CONCEPT_REQUIRES_(IntRank{} == 0)>
  static constexpr auto impl(T&&, IntRank) {
    return 1.;
  }

  template <typename T, typename IntRank,
            CONCEPT_REQUIRES_(IntRank{} == math::highest<dim_t>)>
  static constexpr auto impl(T&&, IntRank) {
    return 0.;
  }

  template <typename T>
  static constexpr auto path(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             path_integral<T>{}));

  template <typename T>
  static constexpr auto boundary(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             boundary_integral<T>{}));

  template <typename T>
  static constexpr auto area(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             surface_integral<T>{}));

  template <typename T>
  static constexpr auto volume(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             volume_integral<T>{}));

  template <typename T, typename IntRank>
  constexpr auto operator()(T&& t, IntRank) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), IntRank{}));
};

}  // namespace detail

using detail::integral_rank;
using detail::volume_integral;
using detail::surface_integral;
using detail::path_integral;
using detail::boundary_integral;

namespace {
static constexpr auto const& integral
 = static_const<detail::integral_fn>::value;
}  // namespace

namespace detail {

struct area_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral.area(std::forward<T>(t)));
};

}  // namespace detail

namespace {
static constexpr auto const& area = static_const<detail::area_fn>::value;
}  // namespace

namespace detail {

struct perimeter_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral.boundary(std::forward<T>(t)));
};

}  // namespace detail

namespace {
static constexpr auto const& perimeter
 = static_const<detail::perimeter_fn>::value;
}  // namespace

namespace detail {

struct surface_area_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral.boundary(std::forward<T>(t)));
};

}  // namespace detail

namespace {
static constexpr auto const& surface_area
 = static_const<detail::surface_area_fn>::value;
}  // namespace

namespace detail {

struct volume_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral.volume(std::forward<T>(t)));
};

}  // namespace detail

namespace {
static constexpr auto const& volume = static_const<detail::volume_fn>::value;
}  // namespace

namespace detail {

struct length_fn {
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(integral.path(std::forward<T>(t)));
};

}  // namespace detail

namespace {
static constexpr auto const& length = static_const<detail::length_fn>::value;
}  // namespace

}  // namespace discrete

using discrete::integral;
using discrete::area;
using discrete::perimeter;
using discrete::surface_area;
using discrete::volume;
using discrete::length;

}  // namespace geometry
}  // namespace hm3
