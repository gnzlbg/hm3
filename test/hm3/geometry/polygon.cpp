#include <hm3/utility/test.hpp>
#include <hm3/geometry/polygon.hpp>
#include <hm3/geometry/line.hpp>
#include <hm3/geometry/intersect.hpp>
#include <hm3/geometry/sd.hpp>

using namespace hm3;
using namespace geometry;

using tri2d  = polygon<2, 3>;
using quad2d = polygon<2, 4>;
using p2d    = point<2>;

int main() {
  tri2d ccw_tri0;
  num_t ccw_tri0_area;
  p2d ccw_tri0_centroid;
  {
    p2d x0({1.0, 0.0});
    p2d x1({0.0, 1.0});
    p2d x2({0.0, 0.0});
    ccw_tri0.push_back(x0);
    ccw_tri0.push_back(x1);
    ccw_tri0.push_back(x2);
    ccw_tri0_area = 0.5 * 1 * 1;
    ccw_tri0_centroid(0) = 1. / 3.;
    ccw_tri0_centroid(1) = 1. / 3.;
  }
  CHECK(is_sorted_counter_clock_wise(ccw_tri0));
  CHECK(!is_sorted_clock_wise(ccw_tri0));
  CHECK(signed_area(ccw_tri0) == ccw_tri0_area);
  CHECK(area(ccw_tri0) == ccw_tri0_area);
  CHECK(centroid(ccw_tri0) == ccw_tri0_centroid);

  tri2d cw_tri0;
  num_t cw_tri0_area;
  {
    p2d x0({0.0, 0.0});
    p2d x1({0.0, 1.0});
    p2d x2({1.0, 0.0});
    cw_tri0.push_back(x0);
    cw_tri0.push_back(x1);
    cw_tri0.push_back(x2);
    cw_tri0_area = 0.5 * 1 * 1;
  }
  CHECK(!is_sorted_counter_clock_wise(cw_tri0));
  CHECK(is_sorted_clock_wise(cw_tri0));
  CHECK(signed_area(cw_tri0) == -cw_tri0_area);
  CHECK(area(cw_tri0) == cw_tri0_area);

  quad2d ccw_quad0;
  num_t ccw_quad0_area;
  p2d ccw_quad0_centroid;
  {
    p2d x0({0.0, 0.0});
    p2d x1({1.0, 0.0});
    p2d x2({1.0, 1.0});
    p2d x3({0.0, 1.0});
    ccw_quad0.push_back(x0);
    ccw_quad0.push_back(x1);
    ccw_quad0.push_back(x2);
    ccw_quad0.push_back(x3);
    ccw_quad0_area = 1.;
    ccw_quad0_centroid(0) = .5;
    ccw_quad0_centroid(1) = .5;
  }
  CHECK(is_sorted_counter_clock_wise(ccw_quad0));
  CHECK(!is_sorted_clock_wise(ccw_quad0));
  CHECK(signed_area(ccw_quad0) == ccw_quad0_area);
  CHECK(area(ccw_quad0) == ccw_quad0_area);
  CHECK(centroid(ccw_quad0) == ccw_quad0_centroid);

  quad2d ccw_quad1;
  num_t ccw_quad1_area;
  p2d ccw_quad1_centroid;
  {
    p2d x0({0.0, 0.0});
    p2d x1({1.0, 0.0});
    p2d x2({2.0, 1.0});
    p2d x3({0.0, 1.0});
    ccw_quad1.push_back(x0);
    ccw_quad1.push_back(x1);
    ccw_quad1.push_back(x2);
    ccw_quad1.push_back(x3);
    ccw_quad1_area = 1.5;
    ccw_quad1_centroid(0) = (0.5 * 1. + (1. + 1. / 3.) * 0.5) / (1 + 0.5);
    ccw_quad1_centroid(1) = (0.5 * 1 + (1. - 1. / 3.) * 0.5) / (1 + 0.5);
  }
  CHECK(is_sorted_counter_clock_wise(ccw_quad1));
  CHECK(!is_sorted_clock_wise(ccw_quad1));
  CHECK(signed_area(ccw_quad1) == ccw_quad1_area);
  CHECK(area(ccw_quad1) == ccw_quad1_area);
  CHECK(centroid(ccw_quad1) == ccw_quad1_centroid);

  {  // square ccw corners
    square<2> s(point<2>::constant(0.), point<2>::constant(1.));
    auto ccw_corners = corners(s);

    std::array<p2d, 4> ccw_corners_should
     = {{p2d{{0., 0.}}, p2d{{1., 0.}}, p2d{{1., 1.}}, p2d{{0., 1.}}}};

    for (suint_t i = 0; i != 4; ++i) {
      CHECK(ccw_corners[i] == ccw_corners_should[i]);
    }
  }

  // line tests
  {
    auto l0 = line<2>::through(point<2>::constant(0.), point<2>::constant(1.));
    num_t l      = std::sqrt(2.);
    point<2> p_c = point<2>::constant(0.5);
    CHECK(dimension(l0) == 2_u);
    CHECK(length(l0) == l);
    CHECK(center(l0) == point<2>::constant(0.5));
    CHECK(direction(l0) == point<2>::constant(1.0 / l));
    point<2> n;
    n(0) = -1. / l;
    n(1) = 1. / l;
    CHECK(normal(l0) == n);

    std::array<num_t, 2> vs{{0., 1.}};
    CHECK(interpolate(l0, vs, l / 2.) == 0.5);
    CHECK(distance_to_value(l0, vs, 0.5) == l / 2.);
    CHECK(point_with_value(l0, vs, 0.5) == p_c);
    std::array<num_t, 2> vs2{{-1., 1.}};
    CHECK(interpolate(l0, vs2, l / 2.) == 0.0);
    CHECK(distance_to_value(l0, vs2, 0.0) == l / 2.);
    CHECK(point_with_value(l0, vs2, 0.0) == p_c);
  }

  {  // intersect tests
    square<2> s0(point<2>::constant(0.5), 1.0);
    point<2> n0;
    n0(0) = 1.0;
    n0(1) = 0.0;
    sd::edge<2> e0(point<2>::constant(0.5), n0);
    auto r0 = intersect(s0, e0);
    {  // check shape points
      polygon<2, 4> p0;
      {
        p0.push_back(p2d{{.5, 0.}});
        p0.push_back(p2d{{1., 0.}});
        p0.push_back(p2d{{1., 1.}});
        p0.push_back(p2d{{0.5, 1.}});
      }
      polygon<2, 4> p1;
      {
        p1.push_back(p2d{{.0, .0}});
        p1.push_back(p2d{{.5, 0.}});
        p1.push_back(p2d{{.5, 1.}});
        p1.push_back(p2d{{0., 1.}});
      }
      CHECK(std::get<0>(r0) == p0);
      CHECK(std::get<1>(r0) == p1);
    }

    point<2> n1;
    n1(0) = -1.0;
    n1(1) = 0.0;
    sd::edge<2> e1(point<2>::constant(0.5), n1);
    auto r1 = intersect(s0, e1);
    CHECK(std::get<0>(r0) == std::get<1>(r1));
    CHECK(std::get<1>(r0) == std::get<0>(r1));

    point<2> n2;
    n2(0) = 0.0;
    n2(1) = 1.0;
    sd::edge<2> e2(point<2>::constant(0.5), n2);
    auto r2 = intersect(s0, e2);
    {  // check shape points
      polygon<2, 4> p0;
      {
        p0.push_back(p2d{{1., .5}});
        p0.push_back(p2d{{1., 1.}});
        p0.push_back(p2d{{0., 1.}});
        p0.push_back(p2d{{0., .5}});
      }
      polygon<2, 4> p1;
      {
        p1.push_back(p2d{{.0, .0}});
        p1.push_back(p2d{{1., 0.}});
        p1.push_back(p2d{{1., .5}});
        p1.push_back(p2d{{0., .5}});
      }
      CHECK(std::get<0>(r2) == p0);
      CHECK(std::get<1>(r2) == p1);
    }

    point<2> n3;
    n3(0) = 0.0;
    n3(1) = -1.0;
    sd::edge<2> e3(point<2>::constant(0.5), n3);
    auto r3 = intersect(s0, e3);

    CHECK(std::get<0>(r2) == std::get<1>(r3));
    CHECK(std::get<1>(r2) == std::get<0>(r3));

    point<2> n4;
    n4(0) = 1.0;
    n4(1) = 1.0;
    sd::edge<2> e4(point<2>::constant(0.5), n4);
    auto r4 = intersect(s0, e4);
    {  // check shape points
      polygon<2, 4> p0;
      {
        p0.push_back(p2d{{1., 0.}});
        p0.push_back(p2d{{1., 1.}});
        p0.push_back(p2d{{0., 1.}});
      }
      polygon<2, 4> p1;
      {
        p1.push_back(p2d{{.0, .0}});
        p1.push_back(p2d{{1., 0.}});
        p1.push_back(p2d{{0., 1.}});
      }
      CHECK(std::get<0>(r4) == p0);
      CHECK(std::get<1>(r4) == p1);
    }

    point<2> n5;
    n5(0) = -1.0;
    n5(1) = -1.0;
    sd::edge<2> e5(point<2>::constant(0.5), n5);
    auto r5 = intersect(s0, e5);

    CHECK(std::get<0>(r4) == std::get<1>(r5));
    CHECK(std::get<1>(r4) == std::get<0>(r5));
  }
  return 0;
}
