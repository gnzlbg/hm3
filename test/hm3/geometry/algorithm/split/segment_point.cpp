#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitives.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Nd>  //
void test_segment_point_split() {
  using p_t = point<Nd>;
  using s_t = segment<Nd>;

  auto p0 = p_t::constant(0.);
  auto p1 = p_t::constant(1.);
  auto p2 = p_t::constant(2.);
  auto p3 = p_t::constant(3.);
  auto p4 = p_t::constant(1.5);

  auto s  = s_t(p1, p2);
  auto r1 = s_t(p1, p4);
  auto r2 = s_t(p4, p2);

  auto s0 = split(s, p0);
  auto s1 = split(s, p1);
  auto s2 = split(s, p2);
  auto s3 = split(s, p3);
  auto s4 = split(s, p4);

  CHECK(s0.empty());
  CHECK(s1.size() == 1);
  CHECK(s1[0] == s);
  CHECK(s2.size() == 1);
  CHECK(s2[0] == s);
  CHECK(s3.empty());
  CHECK(s4.size() == 2);
  CHECK(s4[0] == r1);
  CHECK(s4[1] == r2);
}

int main() {
  test_segment_point_split<1>();
  test_segment_point_split<2>();
  test_segment_point_split<3>();

  return test::result();
}
