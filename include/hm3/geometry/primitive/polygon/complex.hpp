#pragma once
/// \file
///
/// Is a polygon complex?
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/concepts.hpp>
namespace hm3::geometry {

namespace polygon_primitive {

namespace complex_detail {

struct complex_fn {
  /// Is the polygon \p p complex?
  ///
  /// A polygon is complex if:
  /// - any two non-consecutive segments intersect, or
  /// - any two consecutive segments intersect somewhere else beside their
  /// - vertices.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
  constexpr bool operator()(P&& p) const noexcept {
    /*
    for (auto&& fi : face_indices(p)) {
      for (auto&& fj : face_indices(p)) {
        if (fi == fj) { continue; }  //

        auto f0 = face(p, fi);
        auto f1 = face(p, fj);

        if (fi + 1 == fj or fi - 1 == fj) {
          // the faces intersect exactly at the vertices (otherwise there is a
          // hole):
          if (!intersect.test(f0, f1)) { return true; }
          auto intersection = intersect(f0, f1);
          if (intersection != f0.x(0) or intersection != f0.x(1)) {
            return true;
          }
        } else {  // cannot intersect at all
          if (intersect.test(fi, fj)) { return true; }
        }
      }
    }
    */
    return false;
  }
};

}  // namespace complex_detail

namespace {
static constexpr auto const& complex
 = static_const<complex_detail::complex_fn>::value;
}

}  // namespace polygon_primitive

using polygon_primitive::complex;

}  // namespace hm3::geometry
