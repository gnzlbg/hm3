#pragma once
#include <hm3/geometry/algorithm/intersection/aabb_point.hpp>
#include <hm3/geometry/algorithm/intersection/box_point.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <typename A>  //
void test_unit_aabb() {
  static constexpr dim_t nd = A::dimension();
  using p_t                 = point<nd>;
  using v_t                 = vec<nd>;
  using aabb_t              = A;

  static_assert(Dimensional<aabb_t>{}, "");
  static_assert(SemiRegular<aabb_t>{}, "");
  static_assert(Regular<aabb_t>{}, "");

  static_assert(Primitive<aabb_t>{}, "");
  static_assert(Volume<aabb_t>{}, "");

  static_assert((nd == 1 and Segment<aabb_t>{}) or nd != 1, "");

  static_assert((nd == 2 and Polygon<aabb_t>{}) or !Polygon<aabb_t>{}, "");
  static_assert((nd == 2 and Polygon<aabb_t, 2>{}) or !Polygon<aabb_t, 2>{},
                "");

  auto pz = p_t::constant(0.);
  auto po = p_t::constant(1.);
  auto vo = v_t::constant(1.);
  auto pm = p_t::constant(.5);

  // constructors:
  aabb_t a;
  aabb_t b(pz, po);
  aabb_t c = aabb_t::at(pm, vo);
  aabb_t e = aabb_t::unit();
  CHECK(b == c);
  CHECK(e == c);

  CHECK(!(b != c));
  CHECK(b != a);
  a = b;
  CHECK(a == c);

  // x_min/x_max:
  CHECK(x_min(b) == pz);
  CHECK(x_max(b) == po);

  // Bounding length:
  for (dim_t d = 0; d < nd; ++d) { CHECK(bounding_length(b, d) == 1.); }
  CHECK(bounding_length.max(b) == 1.);
  CHECK(bounding_length.all(b) == v_t(po));

  // size:
  CHECK(vertex_size(a) == math::ipow(dim_t{2}, nd));

  // vertices:
  auto vxs = vertices(a);
  for (suint_t i = 0; i < vertex_size(a); ++i) {
    CHECK(vertex(a, i) >= pz);
    CHECK(vertex(a, i) <= po);
    CHECK(vertex(a, i) == vxs[i]);
  }

  // centroid:
  CHECK(centroid(b) == pm);

  // contains:
  CHECK(intersection.test(b, p_t::constant(0.1)));
  CHECK(intersection.test(b, p_t::constant(0.9)));
  CHECK(intersection.test(b, p_t::constant(0.0)));
  CHECK(intersection.test(b, p_t::constant(1.0)));

  CHECK(!intersection.test(b, p_t::constant(-0.1)));
  CHECK(!intersection.test(b, p_t::constant(1.1)));
  CHECK(!intersection.test(b, p_t::constant(-0.00000000000001)));
  CHECK(!intersection.test(b, p_t::constant(+1.00000000000001)));

  // volume:
  CHECK(volume(b) == 1.);

  // area
  num_t area_should = nd == 1 ? math::eps : 2. * nd;
  CHECK(surface_area(b) == area_should);
  CHECK(perimeter(b) == area_should);

  // is box
  CHECK(is_box(a));

  // bounding volume
  CHECK(bounding_volume.aabb(a) == aabb<nd>::unit());
  CHECK(bounding_volume.box(a) == box<nd>::unit());
}

template <template <dim_t> class A>  //
void test_unit_aabbs() {
  test_unit_aabb<A<1>>();
  test_unit_aabb<A<2>>();
  test_unit_aabb<A<3>>();
}
