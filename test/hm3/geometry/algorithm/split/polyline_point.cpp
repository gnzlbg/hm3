#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Ad>  //
void test_polyline_point_split() {
  using p_t  = point<Ad>;
  using pl_t = polyline<Ad>;

  auto pM = p_t::constant(-1.);
  auto p0 = p_t::constant(0.);
  auto p1 = p_t::constant(1.);
  auto p2 = p_t::constant(2.);
  auto p3 = p_t::constant(3.);
  auto p4 = p_t::constant(4.);
  auto p5 = p_t::constant(1.5);

  auto vxs = {p0, p1, p2, p3, p4};
  auto pl  = pl_t(make_open_segment_range(vxs));

  auto vxs0 = {p0, p1};
  auto vxs1 = {p1, p2, p3, p4};
  auto vxs2 = {p0, p1, p2};
  auto vxs3 = {p2, p3, p4};
  auto vxs4 = {p0, p1, p2, p3};
  auto vxs5 = {p3, p4};
  auto vxs6 = {p0, p1, p2, p3, p4};
  auto vxs7 = {p0, p1, p5};
  auto vxs8 = {p5, p2, p3, p4};

  pl_t pl0(make_open_segment_range(vxs0));
  pl_t pl1(make_open_segment_range(vxs1));
  pl_t pl2(make_open_segment_range(vxs2));
  pl_t pl3(make_open_segment_range(vxs3));
  pl_t pl4(make_open_segment_range(vxs4));
  pl_t pl5(make_open_segment_range(vxs5));
  pl_t pl6(make_open_segment_range(vxs6));
  pl_t pl7(make_open_segment_range(vxs7));
  pl_t pl8(make_open_segment_range(vxs8));

  {
    auto r = split(pl, pM);
    CHECK(r.size() == 0);
  }

  auto check = [&pl](auto p, auto... args) {
    auto r = split(pl, p);
    auto s = {args...};
    CHECK(r.size() == s.size());
    test::check_equal(r, s);
  };

  check(p0, pl);
  check(p1, pl0, pl1);
  check(p2, pl2, pl3);
  check(p3, pl4, pl5);
  check(p4, pl6);
  check(p5, pl7, pl8);
}

int main() {
  test_polyline_point_split<1>();
  test_polyline_point_split<2>();
  test_polyline_point_split<3>();

  return test::result();
}
