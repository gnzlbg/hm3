#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>
#include <hm3/geometry/io/vtk.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

using p_t    = geometry::point<2>;
using poly_t = geometry::bounded_polygon<2, 10>;

int main() {
  // tri:
  p_t p0{.5, .5};
  p_t p1{1.5, .5};
  p_t p2{1.0, 1.5};

  // points inside tri:
  p_t p3{.55, .55};
  p_t p4{1.45, .55};
  p_t p5{1.0, 1.45};

  // points outside tri (close to sides):
  p_t p6{1., .45};
  p_t p7{.55, 1.};
  p_t p8{1.3, 1.};

  // points outside tri (close to corner):
  p_t p9{0.95, 1.5};
  p_t p10{1.05, 1.5};

  // points outside tri (parallel to edges):
  p_t p11{0.3, .5};

  auto pis = {p3, p4, p5};
  auto pos = {p6, p7, p8, p9, p10, p11};

  poly_t tri({p0, p1, p2});

  // points inside
  for (auto&& pi : pis) {
    fmt::print("pi: {}\n", pi);
    CHECK(geometry::intersection.test(tri, pi));
    auto r = geometry::intersection(tri, pi);
    visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, p_t>{}) { CHECK(v == pi); }
       else {
         CHECK(false);
       }
     },
     r);
  }

  for (auto&& po : pos) {
    fmt::print("po: {}\n", po);
    CHECK(!geometry::intersection.test(tri, po));
    auto r = geometry::intersection(tri, po);
    visit(
     [](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, monostate>{}) { CHECK(true); }
       else {
         CHECK(false);
       }
     },
     r);
  }

#ifdef HM3_ENABLE_VTK
  geometry::io::vtk<2> o{};
  o.push_cell(tri);
  o.push_range(pis);
  o.push_range(pos);
  o.write("polygon_point_intersection");
#endif  // HM3_ENABLE_VTK

  return 0;
}
