#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Nd>  //
void test_polyline_point_split() {
  using p_t  = point<Nd>;
  using vp_t = std::vector<p_t>;
  using pl_t = polyline<Nd>;

  auto pM = p_t::constant(-1.);
  auto p0 = p_t::constant(0.);
  auto p1 = p_t::constant(1.);
  auto p2 = p_t::constant(2.);
  auto p3 = p_t::constant(3.);
  auto p4 = p_t::constant(4.);
  auto p5 = p_t::constant(1.25);
  auto p6 = p_t::constant(1.5);
  auto p7 = p_t::constant(1.75);
  auto p8 = p_t::constant(3.75);

  auto pP = p_t::constant(5.);

  auto vxs0 = {p0, p1};
  auto pl0  = pl_t(make_open_segment_range(vxs0));
  auto vxs1 = {p0, p1, p2, p3, p4};
  auto pl1  = pl_t(make_open_segment_range(vxs1));

  auto check = [&](auto r, auto... args) {
    auto vxs = {args...};
    auto pl_ = pl_t(make_open_segment_range(vxs));
    CHECK(r == pl_);
  };

  {  // split == original polyline
    // zero points
    auto rZ = split.against_range(pl0, vp_t{});
    CHECK(rZ.size() == 1_u);
    CHECK(rZ[0] == pl0);
    // points outside polyline
    auto r0 = split.against_range(pl0, vp_t{pP, pM});
    CHECK(r0.size() == 1_u);
    CHECK(r0[0] == pl0);
    auto r1 = split.against_range(pl0, vp_t{pM, pP});
    CHECK(r1.size() == 1_u);
    CHECK(r1[0] == pl0);
    // points at the polyline boundaries
    auto r2 = split.against_range(pl0, vp_t{pM, pP, p0});
    CHECK(r2.size() == 1_u);
    CHECK(r2[0] == pl0);
    auto r3 = split.against_range(pl0, vp_t{p0});
    CHECK(r3.size() == 1_u);
    CHECK(r3[0] == pl0);
    auto r4 = split.against_range(pl0, vp_t{pM, pP, p1});
    CHECK(r4.size() == 1_u);
    CHECK(r4[0] == pl0);
    auto r5 = split.against_range(pl0, vp_t{p1});
    CHECK(r5.size() == 1_u);
    CHECK(r5[0] == pl0);
    auto r6 = split.against_range(pl0, vp_t{pM, pP, p0, p1});
    CHECK(r6.size() == 1_u);
    CHECK(r6[0] == pl0);
  }

  {  // split == original polyline
    // zero points
    auto rZ = split.against_range(pl1, vp_t{});
    CHECK(rZ.size() == 1_u);
    CHECK(rZ[0] == pl1);
    // points outside polyline
    auto r0 = split.against_range(pl1, vp_t{pP, pM});
    CHECK(r0.size() == 1_u);
    CHECK(r0[0] == pl1);
    auto r1 = split.against_range(pl1, vp_t{pM, pP});
    CHECK(r1.size() == 1_u);
    CHECK(r1[0] == pl1);
    // points at the polyline boundaries
    auto r2 = split.against_range(pl1, vp_t{pM, pP, p0});
    CHECK(r2.size() == 1_u);
    CHECK(r2[0] == pl1);
    auto r3 = split.against_range(pl1, vp_t{p0});
    CHECK(r3.size() == 1_u);
    CHECK(r3[0] == pl1);
    auto r4 = split.against_range(pl1, vp_t{pM, pP, p4});
    CHECK(r4.size() == 1_u);
    CHECK(r4[0] == pl1);
    auto r5 = split.against_range(pl1, vp_t{p4});
    CHECK(r5.size() == 1_u);
    CHECK(r5[0] == pl1);
    auto r6 = split.against_range(pl1, vp_t{pM, pP, p0, p4});
    CHECK(r6.size() == 1_u);
    CHECK(r6[0] == pl1);
  }

  {
    auto rz = split.against_range(pl1, vp_t{pM, pP, p5, p6, p7, p8});
    CHECK(rz.size() == 5_u);
    check(rz[0], p0, p1, p5);
    check(rz[1], p5, p6);
    check(rz[2], p6, p7);
    check(rz[3], p7, p2, p3, p8);
    check(rz[4], p8, p4);
  }

  {
    auto rz = split.against_range(pl1, vp_t{p1, p5, p6, p7, p2});
    CHECK(rz.size() == 6_u);
    check(rz[0], p0, p1);
    check(rz[1], p1, p5);
    check(rz[2], p5, p6);
    check(rz[3], p6, p7);
    check(rz[4], p7, p2);
    check(rz[5], p2, p3, p4);
  }
}

int main() {
  test_polyline_point_split<1>();
  test_polyline_point_split<2>();
  test_polyline_point_split<3>();

  return test::result();
}
