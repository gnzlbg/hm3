#include <fstream>
#include <hm3/geometry/primitive/polygon.hpp>

#include <hm3/geometry/data_structure/bvh.hpp>
#include <hm3/geometry/data_structure/bvh/vtk.hpp>
#include <hm3/geometry/io/stl.hpp>

#include <hm3/utility/test.hpp>

namespace hm3 {
namespace geometry {

inline bool set_normal(triangle<3>&, int, double) { return true; }

inline float get_normal(triangle<3> const& t, int i) {
  if (i >= 0 and i < 3) { return normal(t)(i); }
  throw "error";
}

inline bool set_vertex(triangle<3>& t, int vx, int d, double v) {
  if (vx >= 0 and vx < 3 and d >= 0 and d < 3) {
    vertex(t, vx)(d) = v;
    return true;
  }
  return false;
}

inline float get_vertex(triangle<3> const& t, int vx, int d) {
  if (vx >= 0 and vx < 3 and d >= 0 and d < 3) { return vertex(t, vx)(d); }
  throw "error";
}
}
}

using namespace hm3;
using namespace geometry;

template <typename T>  //
struct mesh {
  using triangle_type = T;
  std::vector<T> elements;
  std::string name;
  T& simplex(uint_t i) { return elements[i]; }
  T const& simplex(uint_t i) const { return elements[i]; }

  auto face_ids() const { return view::ints(std::size_t{0}, elements.size()); }

  std::size_t size() const { return elements.size(); }
};

template <typename T> inline bool set_mesh_name(mesh<T>& m, std::string name) {
  m.name = std::move(name);
  return true;
}

template <typename T> inline bool push_triangle(mesh<T>& m, T t) {
  m.elements.push_back(t);
  return true;
}

template <typename T> inline bool reserve(mesh<T>& m, std::uint32_t n) {
  std::cout << "reserving memory for " << n << " triangles" << std::endl;
  m.elements.reserve(n);
  return true;
}

int main() {
  HM3_INITIALIZE_TIMERS();

  {  // 3D
    using point_t = point<3>;
    using tri_t   = triangle<3>;
    point_t p0    = {0., 0., 0.};
    point_t p1    = {1., 0., 0.};
    point_t p2    = {0., 1., 0.};
    point_t p3    = {1., 1., 0.};
    point_t p4    = {0.5, 0.5, 1.};

    point_t tris[][3] = {{p0, p1, p2}, {p1, p3, p2}, {p0, p1, p4},
                         {p1, p3, p4}, {p3, p2, p4}, {p0, p4, p2}};

    mesh<tri_t> m;
    for (auto&& vs : tris) { m.elements.push_back(tri_t(vs)); }

    CHECK(m.size() > 0_u);

    bounding_volume_hierarchy<3> b;
    b.rebuild(m);

#ifdef HM3_ENABLE_VTK
    bvh::vtk::serialize(b, "bvh_output0");
#endif
  }

  {  // bunny
    using tri_t = triangle<3>;
    mesh<tri_t> m;
    string file_name = "bunny2.stl";
    std::ifstream ifs(file_name);
    if (!ifs.is_open()) {
      HM3_FATAL_ERROR("file \"{}\" not found!", file_name);
    }

    using is_it = std::istreambuf_iterator<char>;

    boost::spirit::multi_pass<is_it> first
     = boost::spirit::make_default_multi_pass(is_it(ifs));
    boost::spirit::multi_pass<is_it> last
     = boost::spirit::make_default_multi_pass(is_it{});

    auto success = stl::parse_mesh(first, last, m, stl::ascii);
    CHECK(success);
    CHECK(m.size() > 0_u);

    std::cout << "no triangles in mesh: " << m.elements.size() << std::endl;

    bounding_volume_hierarchy<3> bvh_;

    bvh_.rebuild(m);

#ifdef HM3_ENABLE_VTK
    bvh::vtk::serialize(bvh_, "bvh_output1");
#endif
  }

  return test::result();
}
