#include <fstream>
#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/data_structure/bvh.hpp>
#include <hm3/geometry/data_structure/bvh/vtk.hpp>
#include <hm3/geometry/io/stl.hpp>
#include <hm3/geometry/primitives.hpp>

#include <hm3/utility/test.hpp>

template struct hm3::geometry::bvh::bvh<1>;
template struct hm3::geometry::bvh::bvh<2>;
template struct hm3::geometry::bvh::bvh<3>;

using namespace hm3;
using namespace geometry;

template <typename T>
struct mesh {
  using triangle_type = T;
  std::vector<T> elements;
  std::string name;
  T& simplex(uint_t i) { return elements[i]; }
  T const& simplex(uint_t i) const { return elements[i]; }

  auto face_ids() const { return view::ints(std::size_t{0}, elements.size()); }

  std::size_t size() const { return elements.size(); }

  void reserve(std::size_t n) {
    std::cout << "reserving memory for " << n << " triangles" << std::endl;
    elements.reserve(n);
  }
  void push_back(::hm3::geometry::io::stl::triangle t) {
    elements.push_back(T(make_segment_range(
     t.vertices() | view::transform([](auto&& v) {
       return point<3>{{num_t{v[0]}, num_t{v[1]}, num_t{v[2]}}};
     }))));
  }
};

int main() {
  HM3_INITIALIZE_TIMERS();

  {  // 3D single tetrahedron
    using point_t = point<3>;
    using tri_t   = triangle<3>;
    point_t p0    = {0., 0., 0.};
    point_t p1    = {1., 0., 0.};
    point_t p2    = {0., 1., 0.};
    point_t p3    = {1., 1., 0.};
    point_t p4    = {0.5, 0.5, 1.};  // "top" point, others are part of the base

    point_t tris[][3] = {
     {p0, p1, p2},  // 0 (bottom)
     {p1, p3, p2},  // 1 (bottom)
     {p0, p1, p4},  // 2 (side)
     {p1, p3, p4},  // 3 (side)
     {p3, p2, p4},  // 4 (side)
     {p0, p4, p2}   // 5 (side)
    };

    mesh<tri_t> m;
    for (auto&& vs : tris) {
      m.elements.push_back(tri_t(make_segment_range(vs)));
    }

    CHECK(m.size() > 0_u);

    bounding_volume_hierarchy<3> b;
    b.rebuild(m);

    // bounding box around point 4 (top)
    aabb<3> target(point_t{0.25, 0.25, 0.75}, point_t{0.75, 0.75, 1.25});
    auto nodes = b.aabb_intersection(target);
    test::check_equal(
     nodes | view::transform([&](auto&& n) { return *(b.simplex(n)); }),
     {5_u, 2_u, 4_u, 3_u});

#ifdef HM3_ENABLE_VTK
    bvh::vtk::serialize(b, "bvh_output0");
#endif
  }

  {  // bunny ascii
    using tri_t = triangle<3>;
    mesh<tri_t> m;

    namespace stl = hm3::geometry::io::stl;
    stl::read(stl::ascii_format,
              HM3_TEST_RESOURCES "hm3/geometry/io/bunny_ascii.stl", m,
              mpi::comm::world());

    CHECK(m.size() > 0_u);

    ascii_fmt::out("no triangles in mesh: {}\n", m.elements.size());

    bounding_volume_hierarchy<3> bvh_;

    bvh_.rebuild(m);

    mesh<tri_t> mb;

    namespace stl = hm3::geometry::io::stl;
    stl::read(stl::binary_format,
              HM3_TEST_RESOURCES "hm3/geometry/io/bunny_binary.stl", mb,
              mpi::comm::world());

    CHECK(mb.size() > 0_u);
    ascii_fmt::out("no triangles in mesh: {}\n", mb.elements.size());

    bounding_volume_hierarchy<3> bvh_b;

    bvh_b.rebuild(mb);

// CHECK((bvh_ == bvh_b));

#ifdef HM3_ENABLE_VTK
#ifdef HM3_RELEASE_BUILD
    bvh::vtk::serialize(bvh_, "bvh_bunny_ascii");
    bvh::vtk::serialize(bvh_, "bvh_bunny_binary");
#endif
#endif
  }

  return test::result();
}
