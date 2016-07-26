#pragma once
/// \file
///
/// Implements the Ranked geometry concept.
#include <hm3/geometry/dimension.hpp>

namespace hm3 {
namespace geometry {

/// Ranked geometric object.
///
/// \tparam Nd Number of spatial dimensions.
/// \tparam R Rank (Nd >= R).
///
template <dim_t Nd, dim_t R>  //
struct ranked : dimensional<Nd> {
  static_assert(Nd >= R, "The rank of a geometric object cannot exceed the "
                         "number of spatial dimensions");
  /// Geometry rank.
  static constexpr std::integral_constant<dim_t, R> rank() noexcept {
    return std::integral_constant<dim_t, R>{};
  }
  /// Range of ranks.
  static constexpr auto ranks() noexcept {
    return view::iota(dim_t{0}, static_cast<dim_t>(rank()));
  }
  HM3_STATIC_ASSERT_RANDOM_ACCESS_SIZED_RANGE(ranks());

  /// Name of the geometry rank \p r.
  static constexpr char const* rank_name(dim_t r) noexcept {
    HM3_ASSERT(r >= 0 and r < R, "rank {} out of bounds [0, {})", r, R);
    auto v = Nd - r;
    switch (v) {
      case 0: {
        return "volume";
      }
      case 1: {
        return "surface";
      }
      case 2: {
        return "line";
      }
      case 3: {
        return "point";
      }
      default: {
        HM3_FATAL_ERROR("unknown dimension/rank combination {}/{}", Nd, r);
      }
    }
  }
};

namespace geometry_detail {

template <dim_t Nd, dim_t R>  //
constexpr auto rank(ranked<Nd, R> const&) noexcept {
  return ranked<Nd, R>::rank();
}

struct rank_fn {
  template <typename T>
  static constexpr auto impl(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(rank(std::forward<T>(t)));
  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(t.rank());
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  // namespace geometry_detail

namespace {
static constexpr auto const& rank
 = static_const<geometry_detail::rank_fn>::value;
}

template <typename T>
using rank_t = decltype(rank(std::declval<uncvref_t<T>>()));

template <typename T> static constexpr auto rank_v = rank_t<T>{};

namespace geometry_detail {

template <typename T>  //
constexpr auto ranks(T&& t) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
 view::iota(dim_t{0}, static_cast<dim_t>(hm3::geometry::rank(t))));

struct ranks_fn {
  template <typename T>
  static constexpr auto impl(T&& t, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(ranks(std::forward<T>(t)));
  template <typename T>
  static constexpr auto impl(T&& t, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(t.ranks());
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), 0));
};

}  //  namespace geometry_detail

namespace {
static constexpr auto const& ranks
 = static_const<geometry_detail::ranks_fn>::value;
}

namespace concepts {

namespace rc = ranges::concepts;

struct ranked : rc::refines<Dimensional> {
  template <typename T>
  auto requires_(T&& t)
   -> decltype(rc::valid_expr(rc::convertible_to<dim_t>(rank(t)),            //
                              rc::model_of<rc::RandomAccessRange>(ranks(t))  //
                              ));
};

using Ranked = ranked;
}  // namespace concepts

template <typename T, dim_t Nd = math::highest<dim_t>,
          dim_t R = math::highest<dim_t>>
using Ranked
 = meta::and_<concepts::rc::models<concepts::Ranked, uncvref_t<T>>,
              Dimensional<T, Nd>,
              meta::bool_<R == math::highest<dim_t>
                           ? true
                           : decltype(rank(uncvref_t<T>())){} == R>>;

}  // namespace geometry
}  // namespace hm3
