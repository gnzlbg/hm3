#pragma once
/// \file
///
/// Vertex concept.
#include <hm3/geometry/concept/geometry_object.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace concept {

struct Vertex : rc::refines<GeometryObject, rc::Regular, rc::WeaklyOrdered> {
  template <typename T, typename UT = uncvref_t<T>,
            typename NT = associated::num_type_t<UT>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::point<ambient_dimension_v<UT>>>),
   rc::convertible_to<NT>(t(dim_t{0})),         //
   rc::convertible_to<NT>(t[dim_t{0}]),         //
   rc::convertible_to<UT>(UT::unit(dim_t{0})),  //
   rc::convertible_to<UT>(UT::constant(NT{})),  //
   rc::convertible_to<UT>(UT::zero()),          //
   rc::convertible_to<UT>(UT::ones()),          //
   rc::convertible_to<NT>(*ranges::begin(t))    //
   ));
};

}  // namespace concept

/// Vertex concept:
///
/// Refines: GeometryObject, Regular, WeaklyOrdered, RandomAccessRange (of
/// num_type's)
///
/// Requires:
/// - operator()(dim_t i) -> num_type_t: i-th coordinate value
/// - operator[](dim_t i) -> num_type_t: i-th coordinate value
/// - T::unit(dim_t i) -> T: i-th basis vector
/// - T::constant(num_type_t v) -> T: vertex with all coordinate values equal to
/// v - T::zero() -> T: vertex with all coordinate values equal to zero -
/// T::ones() -> T: vertex with all coordinate values equal to one
///
template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Vertex = meta::and_<GeometryObject<T, Ad, 0>,
                          concept::rc::models<concept::Vertex, T>>;

}  // namespace hm3::geometry
