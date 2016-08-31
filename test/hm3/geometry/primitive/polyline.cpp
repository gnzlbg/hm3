#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd>
void test_polyline() {
  using p_t  = geometry::point<Nd>;
  using v_t  = geometry::vec<Nd>;
  using pl_t = geometry::small_polyline<Nd, 4>;
  using s_t  = geometry::segment<Nd>;
  using a_t  = geometry::aabb<Nd>;
  using b_t  = geometry::box<Nd>;

  static_assert(geometry::Polyline<pl_t>{}, "");
  static_assert(geometry::Polyline<pl_t, Nd>{}, "");

  auto p0 = p_t::constant(0.);
  auto p1 = p_t::constant(1.);
  auto p2 = p_t::constant(2.);
  auto p3 = p_t::constant(3.);

  auto vxs = {p0, p1, p2, p3};

  pl_t pl(vxs);

  s_t s0(p0, p1);
  s_t s1(p1, p2);
  s_t s2(p2, p3);

  auto ss = {s0, s1, s2};

  CHECK(geometry::vertex_size(pl) == 4_u);
  CHECK(geometry::face_size(pl) == 3_u);

  test::check_equal(vertices(pl), vxs);
  test::check_equal(faces(pl), ss);

  auto ls = v_t::constant(3.);
  CHECK(geometry::bounding_length(pl) == 3.);
  CHECK(geometry::bounding_length.all(pl) == ls);
  CHECK(geometry::bounding_length.max(pl) == 3.);

  a_t aabb(p0, p3);
  CHECK(geometry::bounding_volume.aabb(pl) == aabb);

  b_t box(p0, p3);
  CHECK(geometry::bounding_volume.box(pl) == box);

  auto xc = p_t::constant(1.5);
  CHECK(geometry::approx(geometry::centroid(pl), xc));

  auto p4 = p_t::constant(3.);
  auto p5 = p_t::constant(4.);
  auto p6 = p_t::constant(5.);
  auto p7 = p_t::constant(6.);

  auto vxs2 = {p4, p5, p6, p7};
  pl_t pl2(vxs2);

  auto u  = geometry::set_union(pl, pl2);
  auto u2 = geometry::set_union(pl2, pl);

  CHECK(u);
  CHECK(u2);

  auto pl_union_should = pl_t({p0, p1, p2, p3, p5, p6, p7});

  CHECK(u == pl_union_should);
  CHECK(u2 == pl_union_should);

  pl_t pl3({p5, p6, p7});
  auto u3 = geometry::set_union(pl, pl3);
  auto u4 = geometry::set_union(pl3, pl);

  CHECK(!u3);
  CHECK(!u4);

  auto u5 = geometry::merge(geometry::direction.invert(pl), pl2);
  auto u6 = geometry::merge(geometry::direction.invert(pl2), pl);

  CHECK(u5);
  CHECK(u6);
}

int main() {
  test_polyline<1>();
  test_polyline<2>();
  test_polyline<3>();

  {  // 2D polyline sides
    using p_t  = geometry::point<2>;
    using pl_t = geometry::small_polyline<2, 6>;
    p_t p0{1., 1.};
    p_t p1{2., 2.};
    p_t p2{3., 1.5};
    p_t p3{3.5, 1.75};
    p_t p4{4., 1.5};
    p_t p5{5., 3.};

    auto ps = {p0, p1, p2, p3, p4, p5};

    pl_t pl(ps);

    p_t o0{1., 2.};
    p_t o1{1.5, 0.5};
    p_t o2{2.5, 1.5};
    p_t o3{3., 2.};
    p_t o4{3., 1.6};
    p_t o5{3.5, 1.8};
    p_t o6{3.5, 1.6};
    p_t o7{4.5, 2.6};
    p_t o8{5., 2.4};

    using geometry::side;
    using geometry::side_t;

    CHECK(side(pl, p0) == side_t::neither);
    CHECK(side(pl, p1) == side_t::neither);
    CHECK(side(pl, p2) == side_t::neither);
    CHECK(side(pl, p3) == side_t::neither);
    CHECK(side(pl, p4) == side_t::neither);
    CHECK(side(pl, p5) == side_t::neither);

    CHECK(side(pl, o0) == side_t::pos);
    CHECK(side(pl, o1) == side_t::neg);
    CHECK(side(pl, o2) == side_t::neg);
    CHECK(side(pl, o3) == side_t::pos);
    CHECK(side(pl, o4) == side_t::pos);
    CHECK(side(pl, o5) == side_t::pos);
    CHECK(side(pl, o6) == side_t::neg);
    CHECK(side(pl, o7) == side_t::pos);
    CHECK(side(pl, o8) == side_t::neg);
  }

  return test::result();
}
