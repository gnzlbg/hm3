// #pragma once
// /// \file
// ///
// /// Does the box intersect a plane?
// #include <hm3/geometry/algorithm/intersection.hpp>
// #include <hm3/geometry/algorithm/intersection/box_line.hpp>
// #include <hm3/geometry/primitive/box/box.hpp>
// #include <hm3/geometry/primitive/plane/plane.hpp>

// namespace hm3::geometry::box_primitive {

// bool intersection_test(box<1> const& b, plane<1> const& p) noexcept {
//   return intersection_test(b, p.as_point());
// }

// bool intersection_test(box<2> const& b, plane<2> const& p) noexcept {
//   return intersection_test(b, p.as_line());
// }

// /// Intersection between the box \p b and the line \p l.
// auto intersection(box<1> const& b, plane<1> const& p) noexcept {
//   return intersection(b, p.as_point());
// }

// /// Intersection between the box \p b and the line \p l.
// auto intersection(box<2> const& b, plane<2> const& p) noexcept {
//   return intersection(b, p.as_line());
// }

// }  // namespace hm3::geometry::box_primitive
