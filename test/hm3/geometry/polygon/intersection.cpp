#include <hm3/geometry/aabb.hpp>
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon.hpp>
#include <hm3/geometry/sd/combinators.hpp>
#include <hm3/geometry/sd/edge.hpp>

#include <hm3/utility/test.hpp>

using namespace hm3;

using p2d    = geometry::point<2>;
using v2d    = geometry::vector<2>;
using quad2d = geometry::polygon<2, 4>;
using tri2d  = geometry::polygon<2, 3>;
using box2d  = geometry::box<2>;
using aabb2d = geometry::aabb<2>;

template <uint_t MaxNp> using poly2d = geometry::polygon<2, MaxNp>;

using edge2d = geometry::sd::fixed_edge<2>;

static_assert(geometry::SignedDistance<edge2d, 2>{}, "");

template <typename Shape, typename SDF, typename RShape>
void test_intersect(Shape&& s, SDF&& sdf, RShape&& inside, RShape&& outside,
                    RShape&& surface) {
  // check cut:
  auto r = geometry::intersect(s, sdf);
  CHECK(r.inside == inside);
  CHECK(r.outside == outside);
  CHECK(r.surface == surface);

  if (!r.inside.empty()) { CHECK(counter_clock_wise(r.inside)); }
  if (!r.outside.empty()) { CHECK(counter_clock_wise(r.outside)); }

  // check consistency of the cut of the inverse signed distance function:
  // inside of the inverse equals outside, and vice-verse, while surface remains
  // the same
  auto&& isdf = geometry::sd::adapt(geometry::sd::op_inverse, sdf);
  auto ir     = geometry::intersect(s, isdf);
  CHECK(ir.inside == outside);
  CHECK(ir.outside == inside);
  CHECK(ir.surface == surface);

  if (!ir.inside.empty()) { CHECK(counter_clock_wise(ir.inside)); }
  if (!ir.outside.empty()) { CHECK(counter_clock_wise(ir.outside)); }
}

edge2d make_edge(p2d p, num_t nx, num_t ny) {
  v2d n;
  n(0) = nx;
  n(1) = ny;
  return edge2d(p, n);
}

edge2d make_edge(num_t x_c, num_t nx, num_t ny) {
  return make_edge(p2d::constant(x_c), nx, ny);
}

int main() {
  // Test a box cut by a plane passing by its center aligned with the box
  // axis (4 possibilities): normal pointing left, right, top, and bottom

  {  // box intersected with left pointing plane at box center
    // (inverse test covers right pointing plane)
    auto s   = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    auto sdf = make_edge(0.5, 1.0, 0.0);

    poly2d<5> inside  = {p2d{.5, 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{.5, 1.}};
    poly2d<5> outside = {p2d{0., 0.}, p2d{.5, 0.}, p2d{.5, 1.}, p2d{0., 1.}};
    poly2d<5> surface = {p2d{.5, 0.}, p2d{.5, 1.}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with top pointing plane at box center
    // (inverse test covers bottom pointing plane)
    auto s   = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    auto sdf = make_edge(0.5, 0.0, 1.0);

    poly2d<5> inside  = {p2d{1., .5}, p2d{1., 1.}, p2d{0., 1.}, p2d{0., .5}};
    poly2d<5> outside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., .5}, p2d{0., .5}};
    poly2d<5> surface = {p2d{1., .5}, p2d{0., .5}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  // Test a box cut by a 45 degree plane passing by its center (4
  // possibilities): top right (TR), bottom left (BL), top left (TL), bottom
  // right (BR)

  {  // box intersected with 45deg plane at box center pointing TR
    // (inverse test covers pointing BL)
    auto s   = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    auto sdf = make_edge(0.5, 0.5, 0.5);

    poly2d<5> inside  = {p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside = {p2d{0., 0.}, p2d{1., 0.}, p2d{0., 1.}};
    poly2d<5> surface = {p2d{1., 0.}, p2d{0., 1.}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with 45deg plane at box center pointing TL
    // (inverse test covers pointing BR)
    auto s   = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    auto sdf = make_edge(0.5, 0.5, -0.5);

    poly2d<5> inside  = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}};
    poly2d<5> outside = {p2d{0., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> surface = {p2d{0., 0.}, p2d{1., 1.}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  /// Test a box cut by a 45 degree plane displaced by a delta along a 45
  /// degree diagonal: two possibilities per corner are possible, the plane
  /// normal can point to the corner or away from it, resulting in a total of 8
  /// cases. The corners are BL BR TL and TR, and the normals are into the
  /// corner C or away from it A:

  {  // box intersected with 45deg plane at distance detla = 0.25 from box
     // center pointing into the BL corner: CBL (inverse test covers ABL)
    auto s   = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    auto sdf = make_edge(0.25, -0.5, -0.5);

    poly2d<5> inside = {p2d{0., 0.}, p2d{.5, 0.}, p2d{0., .5}};
    poly2d<5> outside
     = {p2d{0.5, 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}, p2d{0., .5}};
    poly2d<5> surface = {p2d{.5, 0.}, p2d{.0, .5}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with 45deg plane at distance detla = 0.25 from box
     // center pointing into the BR corner: CBR (inverse test covers ABR)
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{0.75, 0.25}};
    auto sdf = make_edge(ep, 0.5, -0.5);

    poly2d<5> inside = {p2d{.5, 0.}, p2d{1., 0.}, p2d{1., .5}};
    poly2d<5> outside
     = {p2d{0., 0.}, p2d{.5, 0.}, p2d{1., .5}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> surface = {p2d{.5, 0.}, p2d{1., .5}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with 45deg plane at distance detla = 0.25 from box
     // center pointing into the TR corner: CTR (inverse test covers ATR)
    auto s   = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    auto sdf = make_edge(0.75, 0.5, 0.5);

    poly2d<5> inside = {p2d{1., .5}, p2d{1., 1.}, p2d{.5, 1.}};
    poly2d<5> outside
     = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., .5}, p2d{.5, 1.}, p2d{0., 1.}};
    poly2d<5> surface = {p2d{1., .5}, p2d{.5, 1.}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with 45deg plane at distance detla = 0.25 from box
     // center pointing into the TL corner: CTL (inverse test covers ATL)
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{0.25, 0.75}};
    auto sdf = make_edge(ep, -0.5, 0.5);

    poly2d<5> inside = {p2d{.5, 1.}, p2d{0., 1.}, p2d{0., .5}};
    poly2d<5> outside
     = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{.5, 1.}, p2d{0., .5}};
    poly2d<5> surface = {p2d{.5, 1.}, p2d{0., .5}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  // Test a box intersected with cut planes aligned with box sides (4
  // cases)
  {  // box intersected with a plane aligned with the left axis: L
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{0.0, 0.5}};
    auto sdf = make_edge(ep, 1., 0.);

    poly2d<5> inside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside;
    poly2d<5> surface = {p2d{0., 0.}, p2d{0., 1.}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with a plane aligned with the left axis: R
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{1.0, 0.5}};
    auto sdf = make_edge(ep, -1., 0.);

    poly2d<5> inside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside;
    poly2d<5> surface = {p2d{1., 0.}, p2d{1., 1.}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with a plane aligned with the bottom axis: B
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{0.5, 0.}};
    auto sdf = make_edge(ep, 0., 1.);

    poly2d<5> inside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside;
    poly2d<5> surface = {p2d{0., 0.}, p2d{1., 0.}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with a plane aligned with the bottom axis: B
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{0.5, 1.}};
    auto sdf = make_edge(ep, 0., -1.);

    poly2d<5> inside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside;
    poly2d<5> surface = {p2d{1., 1.}, p2d{0., 1.}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  // Test box intersected with cut plane at the corners that doesn't cut the
  // box, 4 cases (one for each corner): BL, BR, TR, TL
  {  // box intersected with non-cutting plane at the corner: BL
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{0., 0.}};
    auto sdf = make_edge(ep, 0.5, 0.5);

    poly2d<5> inside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside;
    poly2d<5> surface = {p2d{0.0, 0.0}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with non-cutting plane at the corner: BR
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{1., 0.}};
    auto sdf = make_edge(ep, -0.5, 0.5);

    poly2d<5> inside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside;
    poly2d<5> surface = {p2d{1.0, 0.0}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with non-cutting plane at the corner: TR
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{1., 1.}};
    auto sdf = make_edge(ep, -0.5, -0.5);

    poly2d<5> inside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside;
    poly2d<5> surface = {p2d{1.0, 1.0}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  {  // box intersected with non-cutting plane at the corner: TL
    auto s = poly2d<5>(box2d(p2d::constant(0.5), 1.0));
    p2d ep{{0., 1.}};
    auto sdf = make_edge(ep, 0.5, -0.5);

    poly2d<5> inside = {p2d{0., 0.}, p2d{1., 0.}, p2d{1., 1.}, p2d{0., 1.}};
    poly2d<5> outside;
    poly2d<5> surface = {p2d{0.0, 1.0}};
    test_intersect(s, sdf, inside, outside, surface);
  }

  return test::result();
}
