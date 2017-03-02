#include <hm3/geometry/io/vtk.hpp>
#include <hm3/geometry/primitives.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  using p_t    = point<3>;
  using f_t    = polygon<3>;
  using h_t    = polyhedron;
  using aabb_t = aabb<3>;
  using box_t  = box<3>;

  static_assert(AmbientDimension<h_t>{});
  static_assert(AmbientDimension<h_t, 3>{});

  static_assert(ElementDimension<h_t>{});
  static_assert(ElementDimension<h_t, 3>{});

  static_assert(GeometryObject<h_t>{});
  static_assert(GeometryObject<h_t, 3>{});
  static_assert(GeometryObject<h_t, 3, 3>{});
  static_assert(Polyhedron<h_t>{});
  static_assert(Polyhedron<h_t, 3>{});

  // Tetra:
  p_t vxs[4]
   = {p_t{0., 0., 0.}, p_t{1., 0., 0.}, p_t{0., 1., 0.}, p_t{0., 0., 1.}};

  auto vxs0 = {vxs[0], vxs[1], vxs[2]};
  auto vxs1 = {vxs[0], vxs[1], vxs[3]};
  auto vxs2 = {vxs[0], vxs[2], vxs[3]};
  auto vxs3 = {vxs[1], vxs[2], vxs[3]};

  f_t f0(make_segment_range(vxs0));
  f_t f1(make_segment_range(vxs1));
  f_t f2(make_segment_range(vxs2));
  f_t f3(make_segment_range(vxs3));

  f_t fs[4] = {f0, f1, f2, f3};

  h_t tet(fs);

  // Hexas:
  aabb_t a(p_t{1.5, 1.5, 1.5}, p_t{2., 2.5 ,3.});
  box_t b(p_t{5.,5.,5.}, 1.5);

  h_t ha(faces(a));
  h_t hb(faces(b));

#ifdef HM3_ENABLE_VTK
  geometry::io::vtk<3> o{};
  o.push_cell(ha);
  o.push_cell(tet);
  o.push_cell(hb);
  o.write("polyhedron_primitive");
#endif  // HM3_ENABLE_VTK

  return test::result();
}
