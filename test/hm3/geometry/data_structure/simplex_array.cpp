#include <hm3/geometry/data_structure/simplex_array.hpp>
#include <hm3/utility/test.hpp>

int main() {
  using namespace hm3;
  using namespace geometry;

  {  // 2D
    static constexpr dim_t nd = 2;

    using mesh_t = simplex_array<nd>;
    using p_t    = point<nd>;
    using s_t    = simplex<nd>;

    p_t ps[] = {
     // Triangle Bottom Right
     p_t{0.214, -1.76},   // 0
     p_t{0.744, -1.469},  // 1
     p_t{0.422, -1.278},  // 2
     // Triangle Middle
     p_t{-0.189, -1.324},  // 3
     p_t{0.157, -1.037},   // 4
     p_t{-0.264, -0.748},  // 5
     // Square Top Right
     p_t{0.302, -0.262},  // 6
     p_t{0.731, -0.237},  // 7
     p_t{0.747, 0.750},   // 8
     p_t{0.274, 0.737},   // 9
     // Polygon Top Left
     p_t{-0.977, -0.198},  // 10
     p_t{-0.747, 0.193},   // 11
     p_t{-0.905, 0.593},   // 12
     p_t{-1.305, 0.658},   // 13
     p_t{-1.791, 0.722},   // 14
     p_t{-1.694, 0.180},   // 15
     p_t{-1.369, -0.493}   // 16
    };

    s_t ss[]{
     // Triangle Bottom Right
     s_t{ps[0], ps[1]},  // 0
     s_t{ps[1], ps[2]},  // 1
     s_t{ps[2], ps[0]},  // 2
     // Triangle Middle
     s_t{ps[3], ps[4]},  // 3
     s_t{ps[4], ps[5]},  // 4
     s_t{ps[5], ps[3]},  // 5
     // Square Top Right
     s_t{ps[6], ps[7]},  // 6
     s_t{ps[7], ps[8]},  // 7
     s_t{ps[8], ps[9]},  // 8
     s_t{ps[9], ps[6]},  // 9
     // Polygon Top Left
     s_t{ps[10], ps[11]},  // 10
     s_t{ps[11], ps[12]},  // 11
     s_t{ps[12], ps[13]},  // 12
     s_t{ps[13], ps[14]},  // 13
     s_t{ps[14], ps[15]},  // 14
     s_t{ps[15], ps[16]},  // 15
     s_t{ps[16], ps[10]}   // 16
    };

    // default constructor
    mesh_t m;

    // construct from simplex range
    m = mesh_t(ss);

    CHECK(m.face_size() == 17);
    CHECK(m.vertex_size() == 17);
  }

  {  // 3D
    static constexpr dim_t nd = 3;

    using mesh_t = simplex_array<nd>;
    using p_t    = point<nd>;
    using s_t    = simplex<nd>;

    mesh_t m;
  }

  return test::result();
}
