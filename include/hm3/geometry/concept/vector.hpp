#pragma once
/// \file
///
/// Vector concept.
#include <hm3/geometry/concept/geometry_object.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace concept {

struct Vector : rc::refines<GeometryObject, rc::Regular, rc::RandomAccessRange,
                            rc::WeaklyOrdered> {
  template <typename T, typename UT = uncvref_t<T>,
            typename NT = associated::num_type_t<UT>>
  auto requires_(T&& t) -> decltype(rc::valid_expr(
   rc::is_true(trait::check<UT, trait::vector<ambient_dimension_v<UT>>>),  //
   rc::convertible_to<NT>(t(dim_t{0})),                                    //
   rc::convertible_to<NT>(t[dim_t{0}]),                                    //
   rc::convertible_to<UT>(UT::unit(dim_t{0})),                             //
   rc::convertible_to<UT>(UT::zero()),                                     //
   rc::convertible_to<UT>(UT::ones()),                                     //
   rc::convertible_to<UT>(t.normalized())                                  //
   ));
};

}  // namespace concept

template <typename T, dim_t Ad = concept::detail::dimension_independent>
using Vector
 = meta::and_<GeometryObject<T, Ad, 1>, rc::models<concept::Vector, T>>;

}  // namespace hm3::geometry
