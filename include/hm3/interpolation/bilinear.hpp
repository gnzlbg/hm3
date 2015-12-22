// #pragma once
// /// \file
// ///
// /// Bi-linear interpolation in n spatial dimensions (n > 1): 4 points required
// #include <hm3/interpolation/linear.hpp>

// namespace hm3 {
// namespace ip {

// namespace bilinear {

// template <typename D, CONCEPT_REQUIRES_(Interpolable<D>{})>
// void assert_is_valid_ccw_dataset(D const& d) {
//   HM3_ASSERT(size(d.points()) == 4,
//              "linear interpolation requires 2 points but {} points provided",
//              size(d.points()));
//   HM3_ASSERT(size(d.values()) == 4,
//              "linear interpolation requires 2 values but {} values provided",
//              size(d.values()));

//   auto&& p0 = d.points()[0];
//   auto&& p1 = d.points()[1];
//   auto&& p2 = d.points()[2];
//   auto&& p3 = d.points()[3];

//   HM3_ASSERT(math::approx(p0(1), p1(1)), "");
//   HM3_ASSERT(math::approx(p0(0), p2(0)), "");
//   HM3_ASSERT(math::approx(p2(1), p3(1)), "");
//   HM3_ASSERT(math::approx(p1(0), p3(0)), "");
// }

// /// Points must be in counter-clock wise order:
// ///
// ///  3---2
// ///  |   |
// ///  0---1
// template <uint_t Nd, typename D, CONCEPT_REQUIRES_(Interpolable<D>{})>
// auto ccw_interpolate(point<Nd> const& x, D const& d) {
//   assert_is_valid_ccw_dataset(d);

//   auto&& p0 = d.points()[0];
//   auto&& p1 = d.points()[1];
//   auto&& p2 = d.points()[2];

//   auto&& v0 = d.values()[0];
//   auto&& v1 = d.values()[1];
//   auto&& v2 = d.values()[2];
//   auto&& v3 = d.values()[3];

//   auto tmp0 = linear::interpolate(x(0), p0(0), p1(0), v0, v1);
//   auto tmp1 = linear::interpolate(x(0), p0(0), p1(0), v3, v2);

//   return linear::interpolate(x(1), p0(1), p2(1), tmp0, tmp1)
// }

// template <typename D, CONCEPT_REQUIRES_(Interpolable<D>{})>
// void assert_is_valid_z_dataset(D const& d) {
//   HM3_ASSERT(size(d.points()) == 4,
//              "linear interpolation requires 2 points but {} points provided",
//              size(d.points()));
//   HM3_ASSERT(size(d.values()) == 4,
//              "linear interpolation requires 2 values but {} values provided",
//              size(d.values()));

//   auto&& p0 = d.points()[0];
//   auto&& p1 = d.points()[1];
//   auto&& p2 = d.points()[2];
//   auto&& p3 = d.points()[3];

//   HM3_ASSERT(math::approx(p0(1), p1(1)), "");
//   HM3_ASSERT(math::approx(p0(0), p2(0)), "");
//   HM3_ASSERT(math::approx(p2(1), p3(1)), "");
//   HM3_ASSERT(math::approx(p1(0), p3(0)), "");
// }

// /// Points must be in z-order:
// ///
// ///  2---3
// ///  |   |
// ///  0---1
// template <uint_t Nd, typename D, CONCEPT_REQUIRES_(Interpolable<D>{})>
// auto z_interpolate(point<Nd> const& x, D const& d) {
//   assert_is_valid_dataset(d);

//   auto&& p0 = d.points()[0];
//   auto&& p1 = d.points()[1];
//   auto&& p2 = d.points()[2];

//   auto&& v0 = d.values()[0];
//   auto&& v1 = d.values()[1];
//   auto&& v2 = d.values()[2];
//   auto&& v3 = d.values()[3];

//   auto tmp0 = linear::interpolate(x(0), p0(0), p1(0), v0, v1);
//   auto tmp1 = linear::interpolate(x(0), p0(0), p1(0), v2, v3);

//   return linear::interpolate(x(1), p0(1), p2(1), tmp0, tmp1)
// }

// }  // namespace bilinear
// }  // namespace ip
// }  // namespace hm3
