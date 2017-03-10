#pragma once
/// \file
///
/// ElementDimension concept.
#include <hm3/geometry/algorithm/element_dimension.hpp>
#include <hm3/geometry/concept/ambient_dimension.hpp>

namespace hm3::geometry {

namespace concept {

struct ElementDimension  // NOLINT(readability-identifier-naming)
 : rc::refines<AmbientDimension> {
  template <typename T>
  auto requires_(T&& t) -> decltype(
   rc::valid_expr(rc::convertible_to<dim_t>(element_dimension(t)),           //
                  rc::model_of<rc::RandomAccessRange>(element_dimension[t])  //
                  ));
};

}  // namespace concept

/// Element Dimension concept.
///
/// ElementDimension<T> refines:
/// - ambient_dimension<T>.
///
/// ElementDimension<T, Ad = dimension_independent,
///                     Ed = dimension_independent>
/// requires:
/// - element_dimension(T) -> dim_t.
/// - element_dimensions(T) -> [dim_t].
///
/// \tparam Ad Ambient dimension.
/// \tparam Ed Element dimension.
template <typename T, dim_t Ad = concept::detail::dimension_independent,
          dim_t Ed = concept::detail::dimension_independent>
using ElementDimension
 = meta::and_<AmbientDimension<T, Ad>,
              concept::detail::ndimensional_<
               T, meta::bool_<rc::models<concept::ElementDimension, T>{}>, Ed,
               element_dimension_t>>;

}  // namespace hm3::geometry
