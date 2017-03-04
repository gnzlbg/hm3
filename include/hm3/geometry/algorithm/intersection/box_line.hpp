// #pragma once
// /// \file
// ///
// /// Does the box intersect a line?
// #include <hm3/geometry/algorithm/approx.hpp>
// #include <hm3/geometry/algorithm/intersection.hpp>
// #include <hm3/geometry/algorithm/intersection/line_segment.hpp>
// #include <hm3/geometry/primitive/box/box.hpp>
// #include <hm3/geometry/primitive/line/line.hpp>
// #include <hm3/utility/variant.hpp>

// namespace hm3::geometry::box_primitive {

// constexpr bool intersection_test(box<1> const&, line<1> const&) noexcept {
//   return true;
// }

// /// Does the Box \p b and the line \p l intersect?
// template <dim_t Nd, CONCEPT_REQUIRES_(Nd != 1)>
// constexpr bool intersection_test(box<Nd> const& b, line<Nd> const& l)
// noexcept {
//   for (auto&& f : faces(b)) {
//     if (geometry::intersection.test(f, l)) { return true; }
//   }
//   return false;
// }

// constexpr box<1> intersection(box<1> const& b, line<1> const&) noexcept {
//   return b;
// }

// /// Intersection between the box \p b and the line \p l.
// constexpr variant<monostate, segment<2>> intersection(
//  box<2> const& b, line<2> const& l) noexcept {
//   if (not geometry::intersection.test(b, l)) { return monostate{}; }

//   fixed_capacity_vector<point<2>, 2> intersection_points;

//   auto push_back_ip = [&](auto&& p) {
//     unique_push_back(intersection_points, p, geometry::approx);
//   };

//   for (auto&& f : faces(b)) {
//     auto i = geometry::intersection(f, l);

//     visit(
//      [&](auto&& v) {
//        using T = uncvref_t<decltype(v)>;
//        if
//          constexpr(Same<T, monostate>{}) { return; }
//        else if
//          constexpr(Same<T, point<2>>{}) { push_back_ip(v); }
//        else if
//          constexpr(Same<T, segment<2>>{}) {
//            push_back_ip(v.x(0));
//            push_back_ip(v.x(1));
//          }
//        else {
//          static_assert(always_false<T>{}, "non-exhaustive visitor");
//        }
//      },
//      i);
//   }

//   HM3_ASSERT(intersection_points.size() == 2, "");
//   segment<2> s(intersection_points[0], intersection_points[1]);

//   if (not geometry::approx(geometry::direction(s), geometry::direction(l))) {
//     geometry::direction.invert(s);
//   }
//   HM3_ASSERT(geometry::approx(geometry::direction(s),
//   geometry::direction(l)),
//              "");

//   return s;
// }

// }  // namespace hm3::geometry::box_primitive
