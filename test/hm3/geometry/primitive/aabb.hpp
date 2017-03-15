#pragma once
#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;       // NOLINT(google-global-names-in-headers)
using namespace geometry;  // NOLINT(google-global-names-in-headers)

template <typename A>  //
void test_unit_aabb() {
  static constexpr dim_t ad = ambient_dimension_v<A>;
  using p_t                 = associated::point_t<A>;
  using v_t                 = associated::vector_t<A>;
  using aabb_t              = A;

  static_assert(AmbientDimension<aabb_t>{}, "");
  static_assert(AmbientDimension<aabb_t, ad>{}, "");

  static_assert(ElementDimension<aabb_t>{}, "");
  static_assert(ElementDimension<aabb_t, ad>{}, "");
  static_assert(ElementDimension<aabb_t, ad, ad>{}, "");

  static_assert(GeometryObject<aabb_t>{}, "");
  static_assert(GeometryObject<aabb_t, ad>{}, "");
  static_assert(GeometryObject<aabb_t, ad, ad>{}, "");

  if
    constexpr(ad == 1) {
      static_assert(VertexAccess<aabb_t>{});
      static_assert(Edge<aabb_t>{});
      static_assert(EdgeAccess<aabb_t>{});
      static_assert(Segment<aabb_t>{});
      static_assert(Segment<aabb_t, ad>{});
    }
  else if
    constexpr(ad == 2) {
      static_assert(VertexAccess<aabb_t>{});
      static_assert(EdgeAccess<aabb_t>{});
      static_assert(Polygon<aabb_t>{});
      static_assert(Polygon<aabb_t, ad>{});
    }
  else if
    constexpr(ad == 3) {
      static_assert(VertexAccess<aabb_t>{});
      static_assert(EdgeAccess<aabb_t>{});
      static_assert(FaceAccess<aabb_t>{});
      static_assert(Polyhedron<aabb_t>{});
      static_assert(Polyhedron<aabb_t, ad>{});
    }

  auto pz = p_t::constant(0.);
  auto po = p_t::constant(1.);
  auto vo = v_t::constant(1.);
  auto pm = p_t::constant(.5);

  // constructors:
  aabb_t a{};
  aabb_t b(pz, po);
  aabb_t c = aabb_t(pm, vo);
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
  for (dim_t d = 0; d < ad; ++d) { CHECK(bounding_length(b, d) == 1.); }
  CHECK(bounding_length.max(b) == 1.);
  CHECK(bounding_length.all(b) == vo);

  // size:
  CHECK(vertex_size(a) == math::ipow(dim_t{2}, ad));

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
  CHECK(!intersection.test(b, p_t::constant(-0.0000001)));
  CHECK(!intersection.test(b, p_t::constant(+1.0000001)));

  // volume:
  CHECK(volume(b) == 1.);

  // area
  num_t area_should = 2. * ad;
  CHECK(integral.boundary(b) == area_should);

  // is box
  CHECK(is_box(a));

  // bounding volume
  CHECK(bounding_volume.aabb(a) == aabb<ad>::unit());
  CHECK(bounding_volume.box(a) == box<ad>::unit());
}

template <template <dim_t> class A>  //
void test_unit_aabbs() {
  test_unit_aabb<A<1>>();
  test_unit_aabb<A<2>>();
  test_unit_aabb<A<3>>();
}
