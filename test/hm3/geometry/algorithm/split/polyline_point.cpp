#include <hm3/geometry/algorithm/split/polyline_point.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd>  //
void test_polyline_point_split() {
  using p_t  = geometry::point<Nd>;
  using pl_t = geometry::small_polyline<Nd, 4>;

  auto pM = p_t::constant(-1.);
  auto p0 = p_t::constant(0.);
  auto p1 = p_t::constant(1.);
  auto p2 = p_t::constant(2.);
  auto p3 = p_t::constant(3.);
  auto p4 = p_t::constant(4.);
  auto p5 = p_t::constant(1.5);

  auto pl = pl_t({p0, p1, p2, p3, p4});

  using r_t = decltype(geometry::split(pl, p0));

  auto r0 = geometry::split(pl, pM);
  CHECK(r0 == r_t{monostate{}});
  auto r1 = geometry::split(pl, p0);
  CHECK(r1 == r_t{pl});
  auto r2 = geometry::split(pl, p1);
  CHECK(r2 == r_t{hm3::make_pair(pl_t({p0, p1}), pl_t({p1, p2, p3, p4}))});
  auto r3 = geometry::split(pl, p2);
  CHECK(r3 == r_t{hm3::make_pair(pl_t({p0, p1, p2}), pl_t({p2, p3, p4}))});
  auto r4 = geometry::split(pl, p3);
  CHECK(r4 == r_t{hm3::make_pair(pl_t({p0, p1, p2, p3}), pl_t({p3, p4}))});
  auto r5 = geometry::split(pl, p4);
  CHECK(r5 == r_t{pl_t({p0, p1, p2, p3, p4})});
  auto r6 = geometry::split(pl, p5);
  CHECK(r6 == r_t{hm3::make_pair(pl_t({p0, p1, p5}), pl_t({p5, p2, p3, p4}))});
}

int main() {
  test_polyline_point_split<1>();
  test_polyline_point_split<2>();
  test_polyline_point_split<3>();

  return test::result();
}
