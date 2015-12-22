#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon.hpp>
#include <hm3/geometry/rectangle.hpp>
#include <hm3/geometry/sd/edge.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/geometry/vector.hpp>

#include <hm3/utility/test.hpp>

using namespace hm3;

using p2d         = geometry::point<2>;
using v2d         = geometry::vector<2>;
using quad2d      = geometry::polygon<2, 4>;
using tri2d       = geometry::polygon<2, 3>;
using square2d    = geometry::square<2>;
using rectangle2d = geometry::rectangle<2>;

template <uint_t MaxNp> using poly2d = geometry::polygon<2, MaxNp>;

using edge2d = geometry::sd::fixed_edge<2>;

auto p = geometry::concepts::signed_distance().requires_(
 edge2d{}, std::integral_constant<uint_t, 2>{});
static_assert(geometry::SignedDistance<edge2d, 2>{}, "");

template <typename Shape, typename SDF, typename RShape>
void test_intersect(Shape&& s, SDF&& sdf, RShape&& inside, RShape&& outside,
                    RShape&& surface) {
  auto r = geometry::intersect(s, sdf);
  CHECK(r.inside == inside);
  CHECK(r.outside == outside);
  CHECK(r.surface == surface);
}

edge2d make_edge(num_t x_c, num_t nx, num_t ny) {
  v2d n;
  n(0) = nx;
  n(1) = ny;
  return edge2d(p2d::constant(x_c), n);
}

int main() {
  {
    auto s   = poly2d<5>(square2d(p2d::constant(0.5), 1.0));
    auto sdf = make_edge(0.5, 1.0, 0.0);

    poly2d<5> inside  = {p2d{.5, 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{.5, 1.}};
    poly2d<5> outside = {p2d{0., 0.}, p2d{.5, 0.}, p2d{.5, 1.}, p2d{.0, 1.}};
    poly2d<5> surface = {p2d{.5, 0.}, p2d{.5, .1}};
    test_intersect(s, sdf, inside, outside, surface);
  }
#ifdef FIXED
  {  // intersect tests
    square<2> s0(point<2>::constant(0.5), 1.0);
    vector<2> n0;
    n0(0) = 1.0;
    n0(1) = 0.0;
    sd::edge<2> e0(point<2>::constant(0.5), n0);
    auto r0 = intersect(s0, e0);
    {  // check shape points
      polygon<2, 5> p0;
      {
        p0.push_back(p2d{{.5, 0.}});
        p0.push_back(p2d{{1., 0.}});
        p0.push_back(p2d{{1., 1.}});
        p0.push_back(p2d{{0.5, 1.}});
      }
      polygon<2, 5> p1;
      {
        p1.push_back(p2d{{.0, .0}});
        p1.push_back(p2d{{.5, 0.}});
        p1.push_back(p2d{{.5, 1.}});
        p1.push_back(p2d{{0., 1.}});
      }
      CHECK(std::get<0>(r0) == p0);
      CHECK(std::get<1>(r0) == p1);
    }

    vector<2> n1;
    n1(0) = -1.0;
    n1(1) = 0.0;
    sd::edge<2> e1(point<2>::constant(0.5), n1);
    auto r1 = intersect(s0, e1);
    CHECK(std::get<0>(r0) == std::get<1>(r1));
    CHECK(std::get<1>(r0) == std::get<0>(r1));

    vector<2> n2;
    n2(0) = 0.0;
    n2(1) = 1.0;
    sd::edge<2> e2(point<2>::constant(0.5), n2);
    auto r2 = intersect(s0, e2);
    {  // check shape points
      polygon<2, 5> p0;
      {
        p0.push_back(p2d{{1., .5}});
        p0.push_back(p2d{{1., 1.}});
        p0.push_back(p2d{{0., 1.}});
        p0.push_back(p2d{{0., .5}});
      }
      polygon<2, 5> p1;
      {
        p1.push_back(p2d{{.0, .0}});
        p1.push_back(p2d{{1., 0.}});
        p1.push_back(p2d{{1., .5}});
        p1.push_back(p2d{{0., .5}});
      }
      CHECK(std::get<0>(r2) == p0);
      CHECK(std::get<1>(r2) == p1);
    }

    vector<2> n3;
    n3(0) = 0.0;
    n3(1) = -1.0;
    sd::edge<2> e3(point<2>::constant(0.5), n3);
    auto r3 = intersect(s0, e3);

    CHECK(std::get<0>(r2) == std::get<1>(r3));
    CHECK(std::get<1>(r2) == std::get<0>(r3));

    vector<2> n4;
    n4(0) = 1.0;
    n4(1) = 1.0;
    sd::edge<2> e4(point<2>::constant(0.5), n4);
    auto r4 = intersect(s0, e4);
    {  // check shape points
      polygon<2, 5> p0;
      {
        p0.push_back(p2d{{1., 0.}});
        p0.push_back(p2d{{1., 1.}});
        p0.push_back(p2d{{0., 1.}});
      }
      polygon<2, 5> p1;
      {
        p1.push_back(p2d{{.0, .0}});
        p1.push_back(p2d{{1., 0.}});
        p1.push_back(p2d{{0., 1.}});
      }
      CHECK(std::get<0>(r4) == p0);
      CHECK(std::get<1>(r4) == p1);
    }

    vector<2> n5;
    n5(0) = -1.0;
    n5(1) = -1.0;
    sd::edge<2> e5(point<2>::constant(0.5), n5);
    auto r5 = intersect(s0, e5);

    CHECK(std::get<0>(r4) == std::get<1>(r5));
    CHECK(std::get<1>(r4) == std::get<0>(r5));
  }
#endif
  return test::result();
}
