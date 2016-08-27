#include <hm3/geometry/algorithm/intersection/polygon_segment.hpp>
#include <hm3/geometry/io/vtk.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  using sp_t = geometry::small_polygon<2, 10>;
  using p_t  = geometry::point<2>;
  using s_t  = geometry::segment<2>;

  auto p0 = p_t{0.0, 0.0};
  auto p1 = p_t{1.0, 0.0};
  auto p2 = p_t{1.0, 1.0};
  auto p3 = p_t{0.0, 1.0};

  sp_t p({p0, p1, p2, p3});

  // intersects a corner point (segments points are outside):
  s_t s0(p_t{-0.5, 0.5}, p_t{0.5, -0.5});
  // intersects consecutive faces (segment endpoints lie on the faces):
  s_t s1(p_t{0.0, 0.25}, p_t{0.25, 0.0});
  // intersect two consecutive faces (segment endpoints outside the polygon):
  s_t s2(p_t{0.25, 1.25}, p_t{1.25, 0.75});
  // intersects opposite polygon corners:
  s_t s3(p_t{-1., 2.}, p_t{2., -1.});
  s_t s4(p_t{-0.5, 0.5}, p_t{1.5, 0.5});     // intersects opposite faces
  s_t s5(p_t{-0.5, 0.75}, p_t{0.25, 0.75});  // intersects one face
  s_t s6(p_t{-0.5, -0.5}, p_t{1.25, -0.5});  // completely outside
  s_t s7(p_t{1.0, -0.5}, p_t{1.0, 1.5});     // intersects whole polygon face
  s_t s8(p_t{0.25, 0.25}, p_t{.25, .75});    // completely inside
  // intersects corner, one segment endpoint is part of the corner:
  s_t s9(p_t{1.0, 1.0}, p_t{1.5, 1.0});
  // intersects corner, all segment is part of the polygon face:
  s_t s10(p_t{0.5, 1.0}, p_t{1.0, 1.0});
  // intersects corner, one point in the face one point outside
  s_t s11(p_t{0.5, 1.0}, p_t{1.5, 1.0});

  // intersects corner, all segment is part of the polygon face:
  s_t s12(p_t{1.0, 1.0}, p_t{0.5, 1.0});
  // intersects corner, one point in the face one point outside
  s_t s13(p_t{1.5, 1.0}, p_t{0.5, 1.0});

  s_t s14(p_t{0.5, 0.5}, p_t{1.5, 0.5});

  auto ss = {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14};

  using geometry::intersection;
  using i_t = decltype(intersection(p, s0));
  CHECK(intersection(p, s0) == i_t{p0});
  CHECK(intersection(p, s1) == i_t{s1});
  CHECK(intersection(p, s2) == i_t{s_t(p_t{0.75, 1.0}, p_t{1.0, 0.875})});
  CHECK(intersection(p, s3) == i_t{s_t(p_t{0.0, 1.0}, p_t{1.0, 0.0})});
  CHECK(intersection(p, s4) == i_t{s_t(p_t{0.0, .5}, p_t{1.0, 0.5})});
  CHECK(intersection(p, s5) == i_t{s_t(p_t{0.0, .75}, p_t{0.25, 0.75})});
  CHECK(intersection(p, s6) == i_t{});
  CHECK(intersection(p, s7) == i_t{s_t(p_t{1.0, 0.0}, p_t{1.0, 1.0})});
  CHECK(intersection(p, s8) == i_t{s8});

  CHECK(intersection(p, s9) == i_t{p_t{1.0, 1.0}});
  CHECK(intersection(p, s10) == i_t{s10});
  CHECK(intersection(p, s11) == i_t{s10});
  CHECK(intersection(p, s12) == i_t{s12});
  CHECK(intersection(p, s13) == i_t{s12});
  CHECK(intersection(p, s14) == i_t{s_t(p_t{.5, .5}, p_t{1.0, .5})});

  {  // polygon with colinear vertices
    auto p4 = p_t{0.5, 0.0};

    sp_t pc({p0, p4, p1, p2, p3});
    s_t s(p0, p1);
    CHECK(intersection(pc, s) == i_t{s});
    using geometry::direction;
    CHECK(intersection(pc, direction.invert(s)) == i_t{direction.invert(s)});
  }

#ifdef HM3_ENABLE_VTK
  geometry::io::vtk<2> o{};
  o.push_cell(p);
  o.push_range(ss);
  o.write("polygon_segment_intersection");
#endif  // HM3_ENABLE_VTK

  return test::result();
}
