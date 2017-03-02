#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Ad>
void test_polyline() {
  using p_t  = point<Ad>;
  using v_t  = vec<Ad>;
  using pl_t = polyline<Ad>;
  using s_t  = segment<Ad>;
  using a_t  = aabb<Ad>;
  using b_t  = box<Ad>;

  static_assert(AmbientDimension<pl_t>{}, "");
  static_assert(AmbientDimension<pl_t, Ad>{}, "");

  static_assert(ElementDimension<pl_t>{}, "");
  static_assert(ElementDimension<pl_t, Ad>{}, "");
  static_assert(ElementDimension<pl_t, Ad, 1>{}, "");

  static_assert(GeometryObject<pl_t>{}, "");
  static_assert(GeometryObject<pl_t, Ad>{}, "");
  static_assert(GeometryObject<pl_t, Ad, 1>{}, "");

  static_assert(Polyline<pl_t>{}, "");
  static_assert(Polyline<pl_t, Ad>{}, "");
  static_assert(MutablePolyline<pl_t>{}, "");
  static_assert(MutablePolyline<pl_t, Ad>{}, "");

  auto p0 = p_t::constant(0.);
  auto p1 = p_t::constant(1.);
  auto p2 = p_t::constant(2.);
  auto p3 = p_t::constant(3.);

  auto vxs = {p0, p1, p2, p3};

  s_t s0(p0, p1);
  s_t s1(p1, p2);
  s_t s2(p2, p3);

  auto es = {s0, s1, s2};
  pl_t pl(es);

  {  // construct polyline from segment
    auto es2 = {s0};
    auto pl2 = pl_t(es2);
    CHECK(vertex(pl2, 0) == vertex(s0, 0));
    CHECK(vertex(pl2, 1) == vertex(s0, 1));
  }

  auto ss = {s0, s1, s2};

  CHECK(vertex_size(pl) == 4_u);
  CHECK(edge_size(pl) == 3_u);

  test::check_equal(vertices(pl), vxs);
  test::check_equal(edges(pl), ss);

  auto ls = v_t::constant(3.);
  CHECK(bounding_length(pl, 0) == 3.);
  CHECK(bounding_length.all(pl) == ls);
  CHECK(bounding_length.max(pl) == 3.);

  a_t aabb(p0, p3);
  CHECK(bounding_volume.aabb(pl) == aabb);

  b_t box(p0, p3);
  CHECK(bounding_volume.box(pl) == box);

  auto xc = p_t::constant(1.5);
  CHECK(approx(centroid(pl), xc));

  auto p4 = p_t::constant(3.);
  auto p5 = p_t::constant(4.);
  auto p6 = p_t::constant(5.);
  auto p7 = p_t::constant(6.);

  auto es2 = {s_t{p4, p5}, s_t{p5, p6}, s_t{p6, p7}};
  pl_t pl2(es2);

  auto u  = concatenate(pl, pl2);
  auto u2 = concatenate(pl2, pl);

  CHECK(u);
  CHECK(u2);
  auto ess = {s_t{p0, p1}, s_t{p1, p2}, s_t{p2, p3},
              s_t{p3, p5}, s_t{p5, p6}, s_t{p6, p7}};
  auto pl_union_should = pl_t(ess);

  CHECK(u.value() == pl_union_should);
  CHECK(u2.value() == pl_union_should);

  auto ess2 = {s_t{p5, p6}, s_t{p6, p7}};
  pl_t pl3(ess2);
  auto u3 = concatenate(pl, pl3);
  auto u4 = concatenate(pl3, pl);

  CHECK(!u3);
  CHECK(!u4);

  {  // pl, p2
    auto u_fail = concatenate(direction.invert(pl), pl2);
    CHECK(!u_fail);
    auto u5 = concatenate.direction_independent(direction.invert(pl), pl2);
    CHECK(u5);
    CHECK(u5.value() == direction.invert(pl_union_should));
  }

  {  // pl2, pl
    auto u_fail = concatenate(direction.invert(pl2), pl);
    CHECK(!u_fail);
    auto u6 = concatenate.direction_independent(direction.invert(pl2), pl);
    CHECK(u6);
    CHECK(u6.value() == direction.invert(pl_union_should));
  }
}

int main() {
  test_polyline<1>();
  test_polyline<2>();
  test_polyline<3>();

  {  // 2D polyline sides
    using p_t  = point<2>;
    using s_t  = segment<2>;
    using pl_t = polyline<2>;
    p_t p0{1., 1.};
    p_t p1{2., 2.};
    p_t p2{3., 1.5};
    p_t p3{3.5, 1.75};
    p_t p4{4., 1.5};
    p_t p5{5., 3.};

    auto es = {s_t{p0, p1}, s_t{p1, p2}, s_t{p2, p3}, s_t{p3, p4}, s_t{p4, p5}};

    pl_t pl(es);

    p_t o0{1., 2.};
    p_t o1{1.5, 0.5};
    p_t o2{2.5, 1.5};
    p_t o3{3., 2.};
    p_t o4{3., 1.6};
    p_t o5{3.5, 1.8};
    p_t o6{3.5, 1.6};
    p_t o7{4.5, 2.6};
    p_t o8{5., 2.4};

    CHECK(relative_position(p0, pl) == relative_position_t::intersected);
    CHECK(relative_position(p1, pl) == relative_position_t::intersected);
    CHECK(relative_position(p2, pl) == relative_position_t::intersected);
    CHECK(relative_position(p3, pl) == relative_position_t::intersected);
    CHECK(relative_position(p4, pl) == relative_position_t::intersected);
    CHECK(relative_position(p5, pl) == relative_position_t::intersected);

    CHECK(relative_position(o0, pl) == relative_position_t::inside);
    CHECK(relative_position(o1, pl) == relative_position_t::outside);
    CHECK(relative_position(o2, pl) == relative_position_t::outside);
    CHECK(relative_position(o3, pl) == relative_position_t::inside);
    CHECK(relative_position(o4, pl) == relative_position_t::inside);
    CHECK(relative_position(o5, pl) == relative_position_t::inside);
    CHECK(relative_position(o6, pl) == relative_position_t::outside);
    CHECK(relative_position(o7, pl) == relative_position_t::inside);
    CHECK(relative_position(o8, pl) == relative_position_t::outside);
  }

  {  // 2D simplify
    using p_t  = point<2>;
    using s_t  = segment<2>;
    using pl_t = polyline<2>;

    p_t p0{0., 0.};
    p_t p1{1., 1.};
    p_t p2{2., 2.};
    p_t p3{3., 2.};
    p_t p4{3., 3.};
    p_t p5{4., 4.};
    p_t p6{5., 5.};
    p_t p7{6., 5.};
    p_t p8{7., 5.};

    auto oes0 = {s_t{p0, p1}, s_t{p1, p2}, s_t{p2, p3}, s_t{p3, p4},
                 s_t{p4, p5}, s_t{p5, p6}, s_t{p6, p7}, s_t{p7, p8}};

    num_t len = 4. * (1. + std::sqrt(2.));

    auto ses0
     = {s_t{p0, p2}, s_t{p2, p3}, s_t{p3, p4}, s_t{p4, p6}, s_t{p6, p8}};

    auto pl_o0 = pl_t(oes0);
    auto pl_s0 = pl_t(ses0);

    auto r0 = simplify(pl_o0);
    CHECK(r0 == pl_s0);

    CHECK(integral.path(pl_o0) == len);
    CHECK(integral.path(r0) == len);
    CHECK(length(pl_o0) == len);

    {  // once at the beginning
      auto oes  = {s_t{p0, p1}, s_t{p1, p2}, s_t{p2, p3}};
      auto ses  = {s_t{p0, p2}, s_t{p2, p3}};
      auto pl_o = pl_t(oes);
      auto pl_s = pl_t(ses);

      auto r = simplify(pl_o);
      CHECK(r == pl_s);
    }

    {  // once at the end
      auto oes  = {s_t{p4, p6}, s_t{p6, p7}, s_t{p7, p8}};
      auto ses  = {s_t{p4, p6}, s_t{p6, p8}};
      auto pl_o = pl_t(oes);
      auto pl_s = pl_t(ses);

      auto r = simplify(pl_o);
      CHECK(r == pl_s);
    }

    {  // only once
      auto oes  = {s_t{p0, p1}, s_t{p1, p2}};
      auto ses  = {s_t{p0, p2}};
      auto pl_o = pl_t(oes);
      auto pl_s = pl_t(ses);

      auto r = simplify(pl_o);
      CHECK(r == pl_s);
    }
    {  // only once
      auto oes  = {s_t{p0, p1}, s_t{p1, p2}};
      auto ses  = {s_t{p0, p2}};
      auto pl_o = pl_t(oes);
      auto pl_s = pl_t(ses);

      auto r = simplify(pl_o);
      CHECK(r == pl_s);
    }

    {  // nothing to simplify
      auto oes  = {s_t{p4, p6}};
      auto ses  = {s_t{p4, p6}};
      auto pl_o = pl_t(oes);
      auto pl_s = pl_t(ses);

      auto r = simplify(pl_o);
      CHECK(r == pl_s);
    }
  }

  return test::result();
}
