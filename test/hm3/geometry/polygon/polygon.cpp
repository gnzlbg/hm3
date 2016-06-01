#include <hm3/geometry/aabb.hpp>
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon.hpp>

#include <hm3/utility/test.hpp>

using namespace hm3;

using p2d    = geometry::point<2>;
using v2d    = geometry::vector<2>;
using quad2d = geometry::polygon<2, 4>;
using tri2d  = geometry::polygon<2, 3>;
using box2d  = geometry::box<2>;
using aabb2d = geometry::aabb<2>;

// check concepts:
static_assert(geometry::PolygonD<quad2d, 2>{}, "");
static_assert(geometry::PolygonD<tri2d, 2>{}, "");
static_assert(geometry::PolygonD<box2d, 2>{}, "");
static_assert(geometry::PolygonD<aabb2d, 2>{}, "");

template <typename Points, typename Point>
void test_unit_box(Points&& ps, num_t sa,
                   Point&& x_c) {  // constructors and equality:
  quad2d a;
  quad2d b;
  num_t A = std::abs(sa);

  // check equality
  CHECK(a == b);
  CHECK(!(a != b));

  for (auto&& p : ps) { a.push_back(p); }
  hm3::fmt::print("a: {}\n", a);
  CHECK(a != b);
  b = a;
  CHECK(a == b);

  // check vertices
  CHECK(size(vertex_indices(a)) == 4_u);
  CHECK(equal(vertex_indices(a), {0, 1, 2, 3}));
  CHECK(equal(vertices(a), ps));

  // construct from box

  box2d sq(x_c, 1.0);
  quad2d c(sq);
  CHECK(a == c);
  CHECK(equal(vertices(a), vertices(c)));

  // construct from aabb
  auto rect = aabb2d::at(x_c, v2d::constant(1.0));
  quad2d d(rect);
  CHECK(a == d);
  CHECK(equal(vertices(a), vertices(d)));

  // signed area:
  CHECK(signed_area(a) == sa);
  CHECK(signed_area(b) == sa);
  CHECK(signed_area(c) == sa);
  CHECK(signed_area(d) == sa);

  // area:
  CHECK(area(a) == A);
  CHECK(area(b) == A);
  CHECK(area(c) == A);
  CHECK(area(d) == A);

  /// ccw:
  CHECK(counter_clock_wise(a));
  CHECK(counter_clock_wise(b));
  CHECK(counter_clock_wise(c));
  CHECK(counter_clock_wise(d));

  /// cw:
  CHECK(!clock_wise(a));
  CHECK(!clock_wise(b));
  CHECK(!clock_wise(c));
  CHECK(!clock_wise(d));

  // centroid:
  CHECK(centroid(a) == x_c);
  CHECK(centroid(b) == x_c);
  CHECK(centroid(c) == x_c);
  CHECK(centroid(d) == x_c);
}

int main() {
  auto b123 = geometry::unit(geometry::box<2>{});
  {  // test unit boxs: 4 points of a box in ccw order
    p2d ps[]  = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};
    p2d ps2[] = {{-0.5, -0.5}, {0.5, -0.5}, {0.5, 0.5}, {-0.5, 0.5}};
    p2d ps3[] = {{-1.0, -1.0}, {0.0, -1.0}, {0.0, 0.0}, {-1.0, 0.0}};

    test_unit_box(ps, 1.0, p2d{{0.5, 0.5}});
    test_unit_box(ps2, 1.0, p2d{{0.0, 0.0}});
    test_unit_box(ps3, 1.0, p2d{{-0.5, -0.5}});
  }

  {
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
      ccw_tri0_area        = 0.5 * 1 * 1;
      ccw_tri0_centroid(0) = 1. / 3.;
      ccw_tri0_centroid(1) = 1. / 3.;
    }
    CHECK(counter_clock_wise(ccw_tri0));
    CHECK(!clock_wise(ccw_tri0));
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
    CHECK(!counter_clock_wise(cw_tri0));
    CHECK(clock_wise(cw_tri0));
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
      ccw_quad0_area        = 1.;
      ccw_quad0_centroid(0) = .5;
      ccw_quad0_centroid(1) = .5;
    }
    CHECK(counter_clock_wise(ccw_quad0));
    CHECK(!clock_wise(ccw_quad0));
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
      ccw_quad1_area        = 1.5;
      ccw_quad1_centroid(0) = (0.5 * 1. + (1. + 1. / 3.) * 0.5) / (1 + 0.5);
      ccw_quad1_centroid(1) = (0.5 * 1 + (1. - 1. / 3.) * 0.5) / (1 + 0.5);
    }
    CHECK(counter_clock_wise(ccw_quad1));
    CHECK(!clock_wise(ccw_quad1));
    CHECK(signed_area(ccw_quad1) == ccw_quad1_area);
    CHECK(area(ccw_quad1) == ccw_quad1_area);
    CHECK(centroid(ccw_quad1) == ccw_quad1_centroid);

    {  // box ccw vertices
      box2d s(p2d::constant(0.), p2d::constant(1.));
      auto ccw_vertices = vertices(s);

      p2d ccw_vertices_should[] = {{0., 0.}, {1., 0.}, {1., 1.}, {0., 1.}};
      for (suint_t i = 0; i != 4; ++i) {
        CHECK(ccw_vertices[i] == ccw_vertices_should[i]);
      }
    }
  }

  return test::result();
}
