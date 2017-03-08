#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/ray.hpp>

#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  using p_t   = point<2>;
  using plg_t = polygon<2>;
  using pll_t = polyline<2>;
  using a_t   = aabb<2>;

  a_t a(p_t{0., 0.}, p_t{1., 1.});

  p_t p0 = {-.1, .2};
  p_t p1 = {.5, .25};
  p_t p2 = {1.1, .2};

  p_t p3 = {-.1, .8};
  p_t p4 = {.5, .75};
  p_t p5 = {1.1, .8};

  p_t p6 = {0, 0.2083333333333334};
  p_t p7 = {1, 0.2083333333333333};
  p_t p8 = {0, 0.7916666666666667};
  p_t p9 = {1, 0.7916666666666667};

  auto pl0_vxs = {p0, p1, p2};
  auto pl1_vxs = {p3, p4, p5};

  pll_t pl0(make_open_segment_range(pl0_vxs));
  pll_t pl1(make_open_segment_range(pl1_vxs));

  auto plls = {pl0, pl1};
  auto pgs  = split.against_range(plg_t(edges(a)), plls);
  CHECK(pgs.size() == 3_u);

  auto r0_vxs = {p6, p_t{0., 0.}, p_t{1., 0.}, p7, p1};
  auto r0     = plg_t(make_segment_range(r0_vxs));
  auto r1_vxs = {p8, p6, p1, p7, p9, p4};
  auto r1     = plg_t(make_segment_range(r1_vxs));
  auto r2_vxs = {p9, p_t{1., 1.}, p_t{0., 1.}, p8, p4};
  auto r2     = plg_t(make_segment_range(r2_vxs));

  CHECK(pgs[0] == r0);
  CHECK(pgs[1] == r2);
  CHECK(pgs[2] == r1);

  return test::result();
}
