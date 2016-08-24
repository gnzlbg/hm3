#include <hm3/geometry/primitive/line.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd> void basic_line_test() {
  using namespace geometry;

  static constexpr dim_t nd = Nd;
  using l_t                 = line<nd>;
  using p_t                 = point<nd>;
  using v_t                 = vec<nd>;

  static_assert(Dimensional<l_t>{}, "");
  static_assert(!Primitive<l_t>{}, "");

  auto o0 = p_t::constant(0.);
  auto o1 = p_t::constant(1.);
  auto o2 = p_t::constant(-1.);

  v_t d0  = v_t::c(1.).normalized();
  auto d1 = v_t::unit(0);

  auto l0 = l_t(o0, d0);
  auto l1 = l_t(o1, d0);

  auto l2 = l_t(o2, d1);
  auto l3 = l_t(o0, d1);

  auto l0_ = l_t::through(o0, o1);

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
  if (nd != 1) { CHECK(!parallel(l0, l2)); }

  // colinear
  CHECK(coincidental(l0, l1));
  if (nd != 1) {
    CHECK(!coincidental(l2, l3));
    CHECK(!coincidental(l0, l2));
  }

  // parameter
  auto check_parameter = [&](auto&& l, auto&& o, num_t val) {
    auto p = parameter(l, o);
    CHECK(p);
    if (p) { CHECK(p.value() == val); }
  };
  check_parameter(l0, o0, 0.);
  check_parameter(l0, o1, std::sqrt(nd));
  check_parameter(l1, o0, -std::sqrt(nd));
  check_parameter(l1, o1, 0.);
}

int main() {
  basic_line_test<1>();
  basic_line_test<2>();
  basic_line_test<3>();

  {  // minimum distance between lines 1D
    static constexpr dim_t nd = 1;
    using l_t                 = geometry::line<nd>;
    using p_t                 = geometry::point<nd>;
    using v_t                 = geometry::vec<nd>;

    p_t o0 = p_t::constant(0.);
    p_t o1 = p_t::constant(1.);

    v_t d0  = v_t::unit(0);
    auto l0 = l_t(o0, d0);
    auto l1 = l_t(o1, d0);

    CHECK(geometry::distance.minimum(l0, l0) == 0.);
    CHECK(geometry::distance.minimum(l0, l1) == 0.);
  }

  {  // minimum distance between lines 2D
    static constexpr dim_t nd = 2;
    using l_t                 = geometry::line<nd>;
    using p_t                 = geometry::point<nd>;
    using v_t                 = geometry::vec<nd>;

    p_t o0 = p_t::constant(0.);

    v_t d0  = v_t::unit(0);
    auto l0 = l_t(o0, d0);

    // coincidental
    p_t o1  = o0;
    o1(0)   = 1.0;
    auto l1 = l_t(o1, d0);
    CHECK(geometry::distance.minimum(l0, l0) == 0.);
    CHECK(geometry::distance.minimum(l0, l1) == 0.);

    // parallel
    p_t o2  = p_t::constant(1.);
    auto l2 = l_t(o2, d0);
    CHECK(geometry::distance.minimum(l0, l2) == 1.);

    // intersecting
    auto d1 = v_t::unit(1);
    auto l3 = l_t(o2, d1);
    CHECK(geometry::distance.minimum(l0, l3) == 0.);
  }

  {  // minimum distance between lines 3D
    static constexpr dim_t nd = 3;
    using l_t                 = geometry::line<nd>;
    using p_t                 = geometry::point<nd>;
    using v_t                 = geometry::vec<nd>;

    p_t o0 = p_t::constant(0.);

    v_t d0  = v_t::unit(0);
    auto l0 = l_t(o0, d0);

    // coincidental
    p_t o1  = o0;
    o1(0)   = 1.0;
    auto l1 = l_t(o1, d0);
    CHECK(geometry::distance.minimum(l0, l0) == 0.);
    CHECK(geometry::distance.minimum(l0, l1) == 0.);

    // parallel
    p_t o2  = p_t::constant(1.);
    auto l2 = l_t(o2, d0);
    CHECK(geometry::distance.minimum(l0, l2) == std::sqrt(2.));

    // intersecting
    v_t d1  = v_t::constant(1).normalized();
    auto l3 = l_t(o2, d1);
    CHECK(geometry::distance.minimum(l0, l3) == 0.);

    // skew
    auto o4 = o2;
    o4(1)   = 3.;
    auto d2 = v_t::unit(2);
    auto l4 = l_t(o4, d2);
    CHECK(geometry::distance.minimum(l0, l4) == 3.);
  }

  {  // Side of point w.r.t. line (2D)
    static constexpr dim_t nd = 2;
    using l_t                 = geometry::line<nd>;
    using p_t                 = geometry::point<nd>;
    using v_t                 = geometry::vec<nd>;

    p_t p0{0., 0.};
    p_t p1{1., 1.};
    p_t p2{0., 1.};
    p_t p3{1., 0.};
    p_t p4{2., 2.};
    p_t p5{2., 3.};
    p_t p6{3., 2.};

    auto l0 = l_t::through(p0, p1);
    auto l1 = l_t::through(p1, p0);

    using geometry::side_t;
    using geometry::side;

    CHECK(side(l0, p0) == side_t::neither);
    CHECK(side(l0, p1) == side_t::neither);
    CHECK(side(l0, p4) == side_t::neither);
    CHECK(side(l0, p2) == side_t::pos);
    CHECK(side(l0, p3) == side_t::neg);
    CHECK(side(l0, p5) == side_t::pos);
    CHECK(side(l0, p6) == side_t::neg);

    CHECK(side(l1, p0) == side_t::neither);
    CHECK(side(l1, p1) == side_t::neither);
    CHECK(side(l1, p4) == side_t::neither);
    CHECK(side(l1, p2) == side_t::neg);
    CHECK(side(l1, p3) == side_t::pos);
    CHECK(side(l1, p5) == side_t::neg);
    CHECK(side(l1, p6) == side_t::pos);
  }

  return test::result();
}
