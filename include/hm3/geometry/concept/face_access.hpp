#pragma once
/// \file
///
/// Face access concept.
#include <hm3/geometry/concept/face.hpp>

namespace hm3::geometry {

namespace concept {

struct FaceAccess  // NOLINT(readability-identifier-naming)
{
  template <typename T, typename UT = uncvref_t<T>,
            typename EIT = associated::face_index_t<UT>,
            typename ET  = associated::face_t<UT>>
  static auto requires_(T&& t) -> decltype(  //
   rc::valid_expr(                           //
    static_cast<EIT>(0),                     //
    static_cast<EIT>(face_size(t)),          //
    rc::model_of<Face>(
     uncvref_t<decltype(static_cast<ET const&>(face(t, EIT(0))))>{})  //
    ));
};

}  // namespace concept

/// FaceAccess concept.
///
/// Requires:
///  - traits::face_index_t<T> -> IndexType
///  - face_size(t) -> IndexType
///  - face(t, IndexType{}) -> Face
template <typename T>
using FaceAccess = rc::models<concept::FaceAccess, T>;

}  // namespace hm3::geometry
