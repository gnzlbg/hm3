#include <hm3/geometry/algorithm/concatenate.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  using p_t  = point<2>;
  using pl_t = polyline<2>;
  using s_t  = segment<2>;

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

  s_t s0 = {p0, p1};
  s_t s1 = {p2, p1};
  s_t s2 = {p2, p3};
  s_t s3 = {p4, p3};
  s_t s4 = {p5, p6};
  s_t s5 = {p7, p6};
  s_t s6 = {p7, p8};
  s_t s7 = {p9, p10};
  s_t s8 = {p10, p11};

  auto s_rng = {s2, s7, s0, s4, s1, s6, s3, s5, s8};

  auto r = concatenate.range(s_rng);

  auto vxs0 = {p0, p1, p2, p3, p4};
  pl_t rs0(make_open_segment_range(vxs0));
  auto vxs1 = {p5, p6, p7, p8};
  pl_t rs1(make_open_segment_range(vxs1));
  auto vxs2 = {p9, p10, p11};
  pl_t rs2(make_open_segment_range(vxs2));

  CHECK(r[0] == rs0);
  CHECK(r[1] == rs2);
  CHECK(r[2] == rs1);

  return test::result();
}
