#include <hm3/geometry/algorithm/concatenate.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  using p_t  = point<1>;
  using pl_t = polyline<1>;
  using s_t  = segment<1>;

  auto p0 = p_t{0.};
  auto p1 = p_t{1.};
  auto p2 = p_t{2.};
  auto p3 = p_t{3.};

  pl_t pl{view::concat(view::single(s_t(p0, p3)))};

  {  // concatenate two points (into a segment):
    auto r = concatenate(p0, p1);
    CHECK(r);
    CHECK(r.value() == s_t(p0, p1));
    // concatenate a segment and a point into a polyline:
    auto r2 = concatenate(r.value(), p2);
    CHECK(r2);
    CHECK(r2.value() == pl_t{view::single(s_t(p0, p2))});
    // concatenate a polyline and a point (into a polyline):
    auto r3 = concatenate(r2.value(), p3);
    CHECK(r3);
    // the polyline has only one edge because the points are colinear and
    // "mergeable" (i.e. have no incompatible data):
    CHECK(r3.value() == pl);
  }

  {  // concatenate a sequence of points
    p_t ps[] = {p0, p1, p2, p3};
    auto r   = concatenate.range(ps);
    CHECK(r.size() == 1_u);
    CHECK(r[0] == pl);
  }

  return test::result();
}
