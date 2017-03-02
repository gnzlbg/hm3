#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/plane.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Ad>
void test_plane() {
  using p_t  = point<Ad>;
  using v_t  = vec<Ad>;
  using pl_t = plane<Ad>;

  static_assert(AmbientDimension<pl_t>{}, "");
  static_assert(AmbientDimension<pl_t, Ad>{}, "");

  static_assert(ElementDimension<pl_t>{}, "");
  static_assert(ElementDimension<pl_t, Ad>{}, "");
  static_assert(ElementDimension<pl_t, Ad, Ad - 1>{}, "");

  static_assert(GeometryObject<pl_t>{}, "");
  static_assert(GeometryObject<pl_t, Ad>{}, "");
  static_assert(GeometryObject<pl_t, Ad, Ad - 1>{}, "");

  static_assert(Plane<pl_t>{});
  static_assert(Plane<pl_t, Ad>{});

  // test cartesian planes through the origin
  for (suint_t d = 0; d < Ad; ++d) {
    auto e_0 = v_t::unit(d);
    auto o   = p_t::constant(0.);

    // plane at the origin with normal pointing in positive d direction
    auto pl0 = pl_t(e_0, o);

    CHECK(o == o);
    CHECK(!(o != o));

    CHECK(parallel(pl0, pl0));

    CHECK(pl0.normal() == e_0);
    CHECK(pl0.signed_distance(o) == 0.);
    CHECK(pl0.abs_distance(o) == 0.);
    CHECK(pl0.projection(o) == o);
    CHECK(pl0.distance_to_origin() == 0.);

    // check distance and signed distance
    CHECK(distance(pl0, o) == 0.);
    auto p0 = p_t::constant(1.);
    CHECK(distance(pl0, p0) == 1.);
    CHECK(pl0.signed_distance(p0) == 1.);
    auto p1 = p_t::constant(2.);
    CHECK(distance(pl0, p1) == 2.);
    CHECK(pl0.signed_distance(p1) == 2.);
    auto p2 = p_t::constant(-2.);
    CHECK(distance(pl0, p2) == 2.);
    CHECK(pl0.signed_distance(p2) == -2.);

    // check projection
    auto p0p = p0;
    p0p(d)   = 0.;
    CHECK(pl0.projection(p0) == p0p);
    auto p1p = p1;
    p1p(d)   = 0.;
    CHECK(pl0.projection(p1) == p1p);
    auto p2p = p2;
    p2p(d)   = 0.;
    CHECK(pl0.projection(p2) == p2p);
  }
}

int main() {
  test_plane<1>();
  test_plane<2>();
  test_plane<3>();

  return test::result();
}
