#pragma once
/// \file
///
/// Edge access concept.
#include <hm3/geometry/concept/edge.hpp>

namespace hm3::geometry {

namespace concept {

struct EdgeAccess {
  template <typename T, typename UT = uncvref_t<T>,
            typename EIT = associated::edge_index_t<UT>,
            typename ET  = associated::edge_t<UT>>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(                           //
    static_cast<EIT>(0),                     //
    static_cast<EIT>(edge_size(t)),          //
    rc::model_of<Edge>(
     uncvref_t<decltype(static_cast<ET const&>(edge(t, EIT(0))))>{})  //
    ));
};

}  // namespace concept

/// EdgeAccess concept.
///
/// Requires:
///  - traits::edge_index_t<T> -> IndexType
///  - edge_size(t) -> IndexType
///  - edge(t, IndexType{}) -> Edge
template <typename T>
using EdgeAccess = rc::models<concept::EdgeAccess, T>;

}  // namespace hm3::geometry
