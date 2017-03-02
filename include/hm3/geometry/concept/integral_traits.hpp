#pragma once
/// \file
///
/// Integral traits.
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/geometry/algorithm/element_dimension.hpp>

namespace hm3::geometry::trait {

/// Volume: ambient dimension == element dimension.
template <typename T>
using is_volume = meta::bool_<ambient_dimension_v<T> == element_dimension_v<T>>;

/// Surface: element dimension == ambient dimension - 1.
template <typename T>
using is_surface
 = meta::bool_<ambient_dimension_v<T> - 1 == element_dimension_v<T>>;

/// Path: element dimension == 1.
template <typename T>
using is_path = meta::bool_<element_dimension_v<T> == 1>;

/// Dimension of the integral to be computed
template <dim_t Id>
using integral_dimension = dim_vt<Id>;

/// Volume integral dimension == Ed.
template <typename T, CONCEPT_REQUIRES_(is_volume<T>{})>
using volume_integral = integral_dimension<ambient_dimension_v<T>>;

/// Surface integral dimension == Ed.
template <typename T, CONCEPT_REQUIRES_(is_surface<T>{})>
using surface_integral = integral_dimension<element_dimension_v<T>>;

/// Path integral dimension == Ed.
template <typename T, CONCEPT_REQUIRES_(is_path<T>{})>
using path_integral = integral_dimension<element_dimension_v<T>>;

/// Boundary integral: over the Ed - 1 dimensional "surface" of the object.
template <typename T>
using boundary_integral = integral_dimension<element_dimension_v<T> - 1>;

}  // namespace hm3::geometry::trait
