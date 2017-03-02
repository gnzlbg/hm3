/// \file
///
/// Geometry line tests.
#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/primitive/line.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template struct hm3::geometry::line_primitive::line<1>;
template struct hm3::geometry::line_primitive::line<2>;
template struct hm3::geometry::line_primitive::line<3>;

// TODO: test project line into Cartesian plane

template <dim_t Ad>
void basic_line_test() {
  static constexpr dim_t ad = Ad;
  using l_t                 = line<ad>;
  using p_t                 = point<ad>;
  using v_t                 = vec<ad>;

  static_assert(AmbientDimension<l_t>{}, "");
  static_assert(AmbientDimension<l_t, Ad>{}, "");

  static_assert(ElementDimension<l_t>{}, "");
  static_assert(ElementDimension<l_t, Ad>{}, "");
  static_assert(ElementDimension<l_t, Ad, 1>{}, "");

  static_assert(GeometryObject<l_t>{}, "");
  static_assert(GeometryObject<l_t, Ad>{}, "");
  static_assert(GeometryObject<l_t, Ad, 1>{}, "");

  static_assert(Line<l_t>{}, "");
  static_assert(Line<l_t, Ad>{}, "");

  auto o0 = p_t::constant(0.);
  auto o1 = p_t::constant(1.);
  auto o2 = p_t::constant(-1.);

  v_t d0  = v_t::c(1.).normalized();
  auto d1 = v_t::unit(0);

  auto l0 = l_t(o0, d0);
  auto l1 = l_t(o1, d0);

  auto l2 = l_t(o2, d1);
  auto l3 = l_t(o0, d1);

  auto l0_ = l_t(o0, o1);

  {  // assignment
    l_t l = l0;
    CHECK(l == l0);
  }

  // relational
  CHECK(l0 == l0);
  CHECK(!(l0 != l0));
  CHECK(l0 != l1);
  // direction
  CHECK(direction(l0) == d0);
  CHECK(direction(l1) == d0);

  // origin
  CHECK(l0.origin() == o0);
  CHECK(l1.origin() == o1);

  // parallel
  CHECK(parallel(l0, l1));
  CHECK(parallel(l2, l3));
  if (ad != 1) { CHECK(!parallel(l0, l2)); }

  // colinear / coincidental / approximately equal:
  CHECK(approx(l0, l1));
  if (ad != 1) {
    CHECK(!approx(l2, l3));
    CHECK(!approx(l0, l2));
  }

  // parameter
  auto check_parameter = [&](auto&& l, auto&& o, num_t val) {
    auto p = line_intersection_parameter(l, o);
    CHECK(p);
    if (p) { CHECK(p.value() == val); }
  };
  check_parameter(l0, o0, 0.);
  check_parameter(l0, o1, std::sqrt(ad));
  check_parameter(l1, o0, -std::sqrt(ad));
  check_parameter(l1, o1, 0.);

  /// TODO: check that for a point not in the line the parameter is empty
}

int main() {
  basic_line_test<1>();
  basic_line_test<2>();
  basic_line_test<3>();
  {  // minimum distance between lines 1D
    static constexpr dim_t ad = 1;
    using l_t                 = geometry::line<ad>;
    using p_t                 = geometry::point<ad>;
    using v_t                 = geometry::vec<ad>;

    p_t o0 = p_t::constant(0.);
    p_t o1 = p_t::constant(1.);

    v_t d0  = v_t::unit(0);
    auto l0 = l_t(o0, d0);
    auto l1 = l_t(o1, d0);

    CHECK(geometry::distance(l0, o0) == 0.);
    CHECK(geometry::distance(o0, l0) == 0.);
    CHECK(geometry::distance(l0, l0) == 0.);
    CHECK(geometry::distance(l0, l1) == 0.);
  }

  {  // minimum distance between lines 2D
    static constexpr dim_t ad = 2;
    using l_t                 = geometry::line<ad>;
    using p_t                 = geometry::point<ad>;
    using v_t                 = geometry::vec<ad>;

    p_t o0 = p_t::constant(0.);

    v_t d0  = v_t::unit(0);
    auto l0 = l_t(o0, d0);

    // coincidental
    p_t o1  = o0;
    o1(0)   = 1.0;
    auto l1 = l_t(o1, d0);
    CHECK(geometry::distance(l0, l0) == 0.);
    CHECK(geometry::distance(l0, l1) == 0.);

    // parallel
    p_t o2  = p_t::constant(1.);
    auto l2 = l_t(o2, d0);
    CHECK(geometry::distance(l0, l2) == 1.);

    {  // minimum distance between point and line:
      p_t o3 = p_t::constant(2.);
      CHECK(geometry::distance(l0, o2) == 1.);
      CHECK(geometry::distance(l0, o3) == 2.);
    }

    {  // normal of a 2D line
      v_t n{0.0, 1.0};
      CHECK(geometry::normal(l0) == n);
    }

    // intersecting
    auto d1 = v_t::unit(1);
    auto l3 = l_t(o2, d1);
    CHECK(geometry::distance(l0, l3) == 0.);
  }

  {  // minimum distance between lines 3D
    static constexpr dim_t ad = 3;
    using l_t                 = geometry::line<ad>;
    using p_t                 = geometry::point<ad>;
    using v_t                 = geometry::vec<ad>;

    p_t o0 = p_t::constant(0.);

    v_t d0  = v_t::unit(0);
    auto l0 = l_t(o0, d0);

    // coincidental
    p_t o1  = o0;
    o1(0)   = 1.0;
    auto l1 = l_t(o1, d0);
    CHECK(geometry::distance(l0, l0) == 0.);
    CHECK(geometry::distance(l0, l1) == 0.);

    // parallel
    p_t o2  = p_t::constant(1.);
    auto l2 = l_t(o2, d0);
    CHECK(geometry::distance(l0, l2) == std::sqrt(2.));

    // intersecting
    v_t d1  = v_t::constant(1).normalized();
    auto l3 = l_t(o2, d1);
    CHECK(geometry::distance(l0, l3) == 0.);

    // skew
    auto o4 = o2;
    o4(1)   = 3.;
    auto d2 = v_t::unit(2);
    auto l4 = l_t(o4, d2);
    CHECK(geometry::distance(l0, l4) == 3.);
  }
  {  // Relative position of point w.r.t. line (2D)
    static constexpr dim_t ad = 2;
    using l_t                 = geometry::line<ad>;
    using p_t                 = geometry::point<ad>;

    p_t p0{0., 0.};
    p_t p1{1., 1.};
    p_t p2{0., 1.};
    p_t p3{1., 0.};
    p_t p4{2., 2.};
    p_t p5{2., 3.};
    p_t p6{3., 2.};

    auto l0 = l_t(p0, p1);
    auto l1 = l_t(p1, p0);

    using geometry::relative_position_t;
    using geometry::relative_position;

    CHECK(relative_position(p0, l0) == relative_position_t::intersected);
    CHECK(relative_position(p1, l0) == relative_position_t::intersected);
    CHECK(relative_position(p4, l0) == relative_position_t::intersected);
    CHECK(relative_position(p2, l0) == relative_position_t::inside);
    CHECK(relative_position(p3, l0) == relative_position_t::outside);
    CHECK(relative_position(p5, l0) == relative_position_t::inside);
    CHECK(relative_position(p6, l0) == relative_position_t::outside);

    CHECK(relative_position(p0, l1) == relative_position_t::intersected);
    CHECK(relative_position(p1, l1) == relative_position_t::intersected);
    CHECK(relative_position(p4, l1) == relative_position_t::intersected);
    CHECK(relative_position(p2, l1) == relative_position_t::outside);
    CHECK(relative_position(p3, l1) == relative_position_t::inside);
    CHECK(relative_position(p5, l1) == relative_position_t::outside);
    CHECK(relative_position(p6, l1) == relative_position_t::inside);
  }
  return test::result();
}
