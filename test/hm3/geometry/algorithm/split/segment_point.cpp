#include <hm3/geometry/algorithm/split/segment_point.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd>  //
void test_segment_point_split() {
  using p_t = geometry::point<Nd>;
  using s_t = geometry::segment<Nd>;

  auto p0 = p_t::constant(0.);
  auto p1 = p_t::constant(1.);
  auto p2 = p_t::constant(2.);
  auto p3 = p_t::constant(3.);
  auto p4 = p_t::constant(1.5);

  auto s  = s_t(p1, p2);
  auto r1 = s_t(p1, p4);
  auto r2 = s_t(p4, p2);

  auto s0 = geometry::split(s, p0);
  auto s1 = geometry::split(s, p1);
  auto s2 = geometry::split(s, p2);
  auto s3 = geometry::split(s, p3);
  auto s4 = geometry::split(s, p4);

  using r_t = decltype(geometry::split(s, p0));

  CHECK(s0 == r_t{monostate{}});
  CHECK(s1 == r_t{s});
  CHECK(s2 == r_t{s});
  CHECK(s3 == r_t{monostate{}});
  CHECK(s4 == r_t{make_pair(r1, r2)});
}

int main() {
  test_segment_point_split<1>();
  test_segment_point_split<2>();
  test_segment_point_split<3>();

  return test::result();
}
