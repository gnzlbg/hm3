#include <hm3/geometry/algorithm/split/polygon_polyline.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  using p_t   = geometry::point<2>;
  using plg_t = geometry::small_polygon<2, 10>;
  using pll_t = geometry::small_polyline<2, 10>;
  using a_t   = geometry::aabb<2>;

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

  pll_t pl0 = {p0, p1, p2};
  pll_t pl1 = {p3, p4, p5};

  vector<plg_t> pgs;
  pgs.push_back(a);

  auto plls = {pl0, pl1};
  geometry::split.range(pgs, plls);
  CHECK(pgs.size() == 3_u);

  auto r0 = plg_t{p6, p_t{0., 0.}, p_t{1., 0.}, p7, p1};
  auto r1 = plg_t{p8, p6, p1, p7, p9, p4};

  // this won't work for whatever reason
  // CHECK(pgs[0] == r0);
  // CHECK(pgs[1] == r1);
  CHECK(pgs[0].size() == r0.size());
  CHECK(pgs[1].size() == r1.size());
  for (auto&& p : view::zip(vertices(pgs[0]), vertices(r0))) {
    CHECK(geometry::approx(p.first, p.second));
  }
  for (auto&& p : view::zip(vertices(pgs[1]), vertices(r1))) {
    CHECK(geometry::approx(p.first, p.second));
  }

  auto r2 = plg_t{p9, p_t{1., 1.}, p_t{0., 1.}, p8, p4};
  CHECK(pgs[2] == r2);

  return test::result();
}
