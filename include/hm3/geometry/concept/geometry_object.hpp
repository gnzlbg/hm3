#pragma once
/// \file
///
/// Geometry object concept.
#include <hm3/geometry/concept/ambient_dimension.hpp>
#include <hm3/geometry/concept/element_dimension.hpp>

namespace hm3::geometry {

namespace concept {
struct GeometryObject  // NOLINT(readability-identifier-naming)
 : rc::refines<AmbientDimension, ElementDimension> {
  template <typename T>
  auto requires_(T&& t) -> void;
};

}  // namespace concept

/// GeometryObject concept.
///
/// GeometryObject<T, Ad = dimension_independent
///                   Ed = dimension_independent>
/// refines:
/// - AmbientDimension<T>.
/// - ElementDimension<T>.
///
/// \tparam Ad Ambient dimension.
/// \tparam Ed Element dimension.
template <typename T, dim_t Ad = concept::detail::dimension_independent,
          dim_t Ed = concept::detail::dimension_independent>
using GeometryObject
 = meta::and_<AmbientDimension<T, Ad>, ElementDimension<T, Ad, Ed>>;

}  // namespace hm3::geometry
