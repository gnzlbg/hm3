#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/io/vtk.hpp>
#include <hm3/geometry/primitives.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  using pg_t = polygon<2>;
  using pl_t = polyline<2>;

  using p_t = point<2>;

  auto pg0    = p_t{0.0, 0.0};
  auto pg1    = p_t{1.0, 0.0};
  auto pg2    = p_t{1.0, 1.0};
  auto pg3    = p_t{0.0, 1.0};
  auto pg_vxs = {pg0, pg1, pg2, pg3};
  pg_t pg(make_segment_range(pg_vxs));

  {  // horizontal across two edges
    auto p0     = p_t{-0.5, 0.5};
    auto p1     = p_t{1.5, 0.5};
    auto pl_vxs = {p0, p1};
    pl_t pl(make_open_segment_range(pl_vxs));

    auto p0r     = p_t{0., 0.5};
    auto p1r     = p_t{1., 0.5};
    auto plr_vxs = {p0r, p1r};
    pl_t plr(make_open_segment_range(plr_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 0_u);
    CHECK(ipls.size() == 1_u);

    CHECK(ipls[0] == plr);
  }

  {  // horizontal 2, intersected with two edges
    auto p0     = p_t{0.0, 0.5};
    auto p1     = p_t{1.0, 0.5};
    auto pl_vxs = {p0, p1};
    pl_t pl(make_open_segment_range(pl_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 0_u);
    CHECK(ipls.size() == 1_u);

    CHECK(ipls[0] == pl);
  }

  {  // horizontal 2, intersected with one full edge
    auto p0     = p_t{0.0, 0.0};
    auto p1     = p_t{1.0, 0.0};
    auto pl_vxs = {p0, p1};
    pl_t pl(make_open_segment_range(pl_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 0_u);
    CHECK(ipls.size() == 1_u);

    CHECK(ipls[0] == pl);
  }

  {  // horizontal 2, intersected with two full edges
    auto p0     = p_t{0.0, 0.0};
    auto p1     = p_t{1.0, 0.0};
    auto p2     = p_t{1.0, 1.0};
    auto pl_vxs = {p0, p1, p2};
    pl_t pl(make_open_segment_range(pl_vxs));
    auto plr0_vxs = {p0, p1};
    pl_t plr0(make_open_segment_range(plr0_vxs));
    auto plr1_vxs = {p1, p2};
    pl_t plr1(make_open_segment_range(plr1_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 0_u);
    CHECK(ipls.size() == 2_u);

    CHECK(ipls[0] == plr0);
    CHECK(ipls[1] == plr1);
  }

  {  // horizontal 2, intersected with one part of an edge
    auto p0     = p_t{0.2, 0.0};
    auto p1     = p_t{0.8, 0.0};
    auto pl_vxs = {p0, p1};
    pl_t pl(make_open_segment_range(pl_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 0_u);
    CHECK(ipls.size() == 1_u);

    CHECK(ipls[0] == pl);
  }

  {  // horizontal: inside the polygon
    auto p0     = p_t{0.2, 0.5};
    auto p1     = p_t{0.8, 0.5};
    auto pl_vxs = {p0, p1};
    pl_t pl(make_open_segment_range(pl_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 0_u);
    CHECK(ipls.size() == 1_u);

    CHECK(ipls[0] == pl);
  }

  {  // horizontal: outside the polygon
    auto p0     = p_t{0.2, 1.5};
    auto p1     = p_t{0.8, 1.5};
    auto pl_vxs = {p0, p1};
    pl_t pl(make_open_segment_range(pl_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 0_u);
    CHECK(ipls.size() == 0_u);
  }

  {  // intersected with one corner
    auto p0     = p_t{-1.0, 0.0};
    auto p1     = p_t{0.0, 0.0};
    auto pl_vxs = {p0, p1};
    pl_t pl(make_open_segment_range(pl_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 1_u);
    CHECK(ipls.size() == 0_u);

    CHECK(ips[0] == pg0);
  }

  {  // intersected with two corners
    auto p0     = p_t{-0.5, 0.5};
    auto p1     = p_t{0.5, -0.5};
    auto p2     = p_t{1.5, 0.5};
    auto pl_vxs = {p0, p1, p2};
    pl_t pl(make_open_segment_range(pl_vxs));

    auto[ips, ipls] = intersection(pg, pl);

    CHECK(ips.size() == 2_u);
    CHECK(ipls.size() == 0_u);

    CHECK(ips[0] == pg0);
    CHECK(ips[1] == pg1);
  }

  return test::result();
}
