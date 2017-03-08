#pragma once
/// \file
///
/// AmbientDimension concept.
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/concept/fwd.hpp>
#include <hm3/math/core.hpp>

namespace hm3::geometry {

namespace concept {

struct AmbientDimension {
  template <typename T>
  auto requires_(T&& t) -> decltype(
   rc::valid_expr(rc::convertible_to<dim_t>(ambient_dimension(t)),
                  rc::model_of<rc::RandomAccessRange>(ambient_dimension[t])  //
                  ));
};

namespace detail {

// The following optionally enforces that dimension(T) -> dim_t == Ad.

static constexpr auto dimension_independent = math::highest<dim_t>;

template <typename T, typename IsDimensional, dim_t Ad,
          template <class> class F>
struct ndimensional_ : meta::bool_<false> {};

template <typename T, template <class> class F>
struct ndimensional_<T, meta::bool_<true>, dimension_independent, F>
 : meta::bool_<true> {};

template <typename T, dim_t Ad, template <class> class F>
struct ndimensional_<T, meta::bool_<true>, Ad, F> : meta::bool_<F<T>{} == Ad> {
};

}  // namespace detail

}  // namespace concept

/// Ambient Dimension concept.
///
/// AmbientDimension<T, Ad = dimension_independent> requires:
/// - ambient_dimension(T) -> dim_t.
/// - ambient_dimensions(T) -> [dim_t].
///
/// AmbientDimension<T, Ad> further requires:
/// - ambient_dimension(T) == Ad.
///
/// \tparam Ad Ambient dimension.
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using AmbientDimension = concept::detail::ndimensional_<
 T, meta::bool_<rc::models<concept::AmbientDimension, T>{}>, Ad,
 ambient_dimension_t>;

}  // namespace hm3::geometry
