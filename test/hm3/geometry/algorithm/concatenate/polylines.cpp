#include <hm3/geometry/algorithm/concatenate.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  using p_t  = point<2>;
  using pl_t = polyline<2>;

  auto p0  = p_t{0., 0.};
  auto p1  = p_t{1., 1.};
  auto p2  = p_t{2., 1.};
  auto p3  = p_t{3., 2.};
  auto p4  = p_t{4., 1.};
  auto p5  = p_t{1., 2.};
  auto p6  = p_t{2., 2.};
  auto p7  = p_t{2., 3.};
  auto p8  = p_t{0., 3.};
  auto p9  = p_t{5., 5.};
  auto p10 = p_t{6., 6.};
  auto p11 = p_t{7., 7.};

  auto vx0 = {p0, p1};
  pl_t pl0(make_open_segment_range(vx0));

  auto vx1 = {p2, p1};
  pl_t pl1(make_open_segment_range(vx1));

  auto vx2 = {p2, p3};
  pl_t pl2(make_open_segment_range(vx2));

  auto vx3 = {p4, p3};
  pl_t pl3(make_open_segment_range(vx3));

  auto vx4 = {p5, p6};
  pl_t pl4(make_open_segment_range(vx4));

  auto vx5 = {p7, p6};
  pl_t pl5(make_open_segment_range(vx5));

  auto vx6 = {p7, p8};
  pl_t pl6(make_open_segment_range(vx6));

  auto vx7 = {p9, p10, p11};
  pl_t pl7(make_open_segment_range(vx7));

  auto pl_rng = {pl2, pl7, pl0, pl4, pl1, pl6, pl3, pl5};

  auto r = concatenate.range(pl_rng);

  auto vxs0 = {p0, p1, p2, p3, p4};
  pl_t s0(make_open_segment_range(vxs0));
  auto vxs1 = {p5, p6, p7, p8};
  pl_t s1(make_open_segment_range(vxs1));

  CHECK(r[0] == s0);
  CHECK(r[1] == pl7);
  CHECK(r[2] == s1);

  return test::result();
}
