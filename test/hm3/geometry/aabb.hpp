#pragma once
#include <hm3/geometry/polygon/concept.hpp>

namespace hm3 {
using namespace geometry;

template <typename A>  //
void test_unit_aabb() {
  static constexpr dim_t nd = A::dimension();
  using p_t                 = point<nd>;
  using aabb_t              = A;

  static_assert(Dimensional<aabb_t>{}, "");
  static_assert(AABB<aabb_t>{}, "");
  static_assert(
   (nd == 3 && !Polygon<aabb_t>{}) || (nd < 3 && Polygon<aabb_t>{}), "");
  static_assert((nd == 3 && !PolygonD<aabb_t, nd>{}) || PolygonD<aabb_t, nd>{},
                "");

  auto pz = p_t::constant(0.);
  auto po = p_t::constant(1.);
  auto pm = p_t::constant(.5);

  // constructors:
  aabb_t a;
  aabb_t b(pz, po);
  aabb_t c = aabb_t::at(pm, po);
  aabb_t e = geometry::unit(aabb_t{});
  CHECK(b == c);
  CHECK(e == c);

  CHECK(!(b != c));
  CHECK(b != a);
  a = b;
  CHECK(a == c);

  // x_min/x_max:
  CHECK(x_min(b) == pz);
  CHECK(x_max(b) == po);

  // length:
  for (dim_t d = 0; d < nd; ++d) { CHECK(length(b, d) == 1.); }
  CHECK(lengths(b) == po);

  // bounds:
  CHECK(bounds(b).min == pz);
  CHECK(bounds(b).max == po);

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
  CHECK(contains(b, p_t::constant(0.1)));
  CHECK(contains(b, p_t::constant(0.9)));
  CHECK(!contains(b, p_t::constant(-0.1)));
  CHECK(!contains(b, p_t::constant(1.1)));

  // volume:
  CHECK(volume(b) == 1.);

  // is box
  CHECK(is_box(a));
  CHECK(!geometry::empty(a));
}

template <template <dim_t> class A>  //
void test_unit_aabbs() {
  test_unit_aabb<A<1>>();
  test_unit_aabb<A<2>>();
  test_unit_aabb<A<3>>();
}

}  // namespace hm3
