#pragma once
/// \file
///
/// Vertex access concept.
#include <hm3/geometry/concept/vertex.hpp>

namespace hm3::geometry {

namespace concept {

struct VertexAccess {
  template <typename T, typename UT = uncvref_t<T>,
            typename IT = associated::vertex_index_t<UT>,
            typename PT = associated::point_t<UT>>
  static auto requires_(T&& t) -> decltype(                        //
   rc::valid_expr(                                                 //
    static_cast<IT>(0),                                            //
    static_cast<IT>(vertex_size(t)),                               //
    rc::model_of<Vertex>(uncvref_t<decltype(vertex(t, IT(0)))>{})  //
    ));
};

}  // namespace concept

/// VertexAccess concept.
///
/// Requires:
///  - traits::vertex_index_t<T> -> IndexType
///  - vertex_size(t) -> IndexType
///  - vertex(t, IndexType{}) -> Vertex
template <typename T>
using VertexAccess = concept::rc::models<concept::VertexAccess, T>;

}  // namespace hm3::geometry
