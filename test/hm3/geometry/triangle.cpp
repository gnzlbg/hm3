#include <hm3/geometry/polygon/segments.hpp>
#include <hm3/geometry/triangle.hpp>
#include <hm3/utility/test.hpp>

int main() {
  using namespace hm3;
  using namespace geometry;
  using geometry::empty;
  {  // 2D triangle
    static constexpr dim_t nd = 2;

    using p_t  = point<nd>;
    using b_t  = box<nd>;
    using ab_t = aabb<nd>;
    using t_t  = triangle<nd>;
    using s_t  = segment<nd>;

    static_assert(Dimensional<t_t>{}, "");
    static_assert(Regular<t_t>{}, "");
    static_assert(Polygon<t_t>{}, "");
    static_assert(PolygonD<t_t, nd>{}, "");

    p_t ps0[3] = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}};
    p_t ps1[3] = {{1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}};
    p_t ps2[3] = {{0.0, 0.0}, {0.5, 0.0}, {0.5, 1.0}};

    // constructors:
    t_t t0(ps0);
    t_t t1(ps1);
    t_t t2(ps2);
    {
      t_t t3;
      t3 = t0;
      CHECK(t3 == t0);
    }

    // relational operators:
    CHECK(t0 == t0);
    CHECK(!(t0 != t0));
    CHECK(t0 != t1);
    CHECK(!(t0 == t1));
    CHECK(t0 != t2);
    static_assert(!empty(t_t{}), "");

    {  // vertices
      CHECK(size(vertices(t0)) == 3_su);
      {
        suint_t vx = 0;
        for (auto&& v : vertices(t0)) {
          CHECK(v == ps0[vx]);
          CHECK(v == vertex(t0, vx));
          ++vx;
        }
        CHECK(vx == 3_su);
      }

      CHECK(size(vertex_indices(t0)) == 3_su);
      {
        suint_t vx = 0;
        for (auto&& i : vertex_indices(t0)) {
          CHECK(i == vx);
          ++vx;
        }
        CHECK(vx == 3_su);
      }
    }

    {  // point order (2D-only):
      CHECK(counter_clock_wise(t0));
      CHECK(counter_clock_wise(t1));
      CHECK(counter_clock_wise(t2));
      CHECK(!clock_wise(t0));
      CHECK(!clock_wise(t1));
      CHECK(!clock_wise(t2));

      p_t ps3[3] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};
      t_t t3(ps3);
      CHECK(!counter_clock_wise(t3));
      CHECK(clock_wise(t3));
    }

    {  // area
      CHECK(area(t0) == 0.5);
      CHECK(area(t1) == 0.5);
      CHECK(area(t2) == 0.25);
    }

    {  // centroid
      p_t c0{2. / 3., 1. / 3.};
      p_t c1{1. / 3., 2. / 3.};
      p_t c2{1. / 3., 1. / 3.};
      CHECK(centroid(t0) == c0);
      CHECK(centroid(t1) == c1);
      CHECK(centroid(t2) == c2);
    }

    {  // aabb
      ab_t a0{p_t{0.0, 0.0}, p_t{1.0, 1.0}};
      ab_t a1{p_t{0.0, 0.0}, p_t{1.0, 1.0}};
      ab_t a2{p_t{0.0, 0.0}, p_t{0.5, 1.0}};

      CHECK(axis_aligned_bounding_box(t0) == a0);
      CHECK(axis_aligned_bounding_box(t1) == a1);
      CHECK(axis_aligned_bounding_box(t2) == a2);
    }

    {  // square bounding box
      b_t b0{p_t{0.0, 0.0}, p_t{1.0, 1.0}};
      b_t b1{p_t{0.0, 0.0}, p_t{1.0, 1.0}};
      b_t b2{p_t{0.25, 0.5}, 1.0};
      CHECK(square_bounding_box(t0) == b0);
      CHECK(square_bounding_box(t1) == b1);
      CHECK(square_bounding_box(t2) == b2);
    }

    {  // segments
      s_t s0[]
       = {s_t{ps0[0], ps0[1]}, s_t{ps0[1], ps0[2]}, s_t{ps0[2], ps0[0]}};
      s_t s1[]
       = {s_t{ps1[0], ps1[1]}, s_t{ps1[1], ps1[2]}, s_t{ps1[2], ps1[0]}};
      s_t s2[]
       = {s_t{ps2[0], ps2[1]}, s_t{ps2[1], ps2[2]}, s_t{ps2[2], ps2[0]}};

      CHECK(equal(segments(t0), s0));
      CHECK(equal(segments(t1), s1));
      CHECK(equal(segments(t2), s2));
    }
  }
  {  // 3D triangle
    static constexpr dim_t nd = 3;

    using p_t  = point<nd>;
    using b_t  = box<nd>;
    using ab_t = aabb<nd>;
    using t_t  = triangle<nd>;
    using s_t  = segment<nd>;

    static_assert(Dimensional<t_t>{}, "");
    static_assert(Regular<t_t>{}, "");
    static_assert(Polygon<t_t>{}, "");
    static_assert(PolygonD<t_t, nd>{}, "");

    p_t ps0[3] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}};
    p_t ps1[3] = {{1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0}};
    p_t ps2[3] = {{0.0, 0.0, 0.0}, {1.0, 0.0, -1.0}, {1.0, 1.0, 1.0}};

    // constructors:
    t_t t0(ps0);
    t_t t1(ps1);
    t_t t2(ps2);
    {
      t_t t3;
      t3 = t0;
      CHECK(t3 == t0);
    }

    // relational operators:
    CHECK(t0 == t0);
    CHECK(!(t0 != t0));
    CHECK(t0 != t1);
    CHECK(!(t0 == t1));
    CHECK(t0 != t2);
    static_assert(!empty(t_t{}), "");

    {  // vertices
      CHECK(size(vertices(t0)) == 3_su);
      {
        suint_t vx = 0;
        for (auto&& v : vertices(t0)) {
          CHECK(v == ps0[vx]);
          CHECK(v == vertex(t0, vx));
          ++vx;
        }
        CHECK(vx == 3_su);
      }

      CHECK(size(vertex_indices(t0)) == 3_su);
      {
        suint_t vx = 0;
        for (auto&& i : vertex_indices(t0)) {
          CHECK(i == vx);
          ++vx;
        }
        CHECK(vx == 3_su);
      }
    }

    {  // area
      CHECK(area(t0) == 0.5);
      CHECK(area(t1) == 0.5);
      CHECK(area(t2) == std::sqrt(3. / 2.));
    }

    {  // normal (ONLY 3D)

      // check the normals of triangles in a plane

      // in yz-plane:
      p_t ps3[3] = {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 1.0, 1.0}};
      p_t ps4[3] = {{0.0, 0.0, 0.0}, {0.0, 1.0, 1.0}, {0.0, 1.0, 0.0}};
      t_t t3(ps3);
      t_t t4(ps4);
      p_t n3{1.0, 0.0, 0.0};
      p_t n4{-1.0, 0.0, 0.0};

      // in xy-plane:
      p_t ps5[3] = {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 1.0, 0.0}};
      p_t ps6[3] = {{0.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}};
      t_t t5(ps5);
      t_t t6(ps6);
      p_t n5{0.0, 0.0, -1.0};
      p_t n6{0.0, 0.0, 1.0};

      // in xz-plane:
      p_t ps7[3] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 1.0}};
      p_t ps8[3] = {{0.0, 0.0, 0.0}, {1.0, 0.0, 1.0}, {1.0, 0.0, 0.0}};
      t_t t7(ps7);
      t_t t8(ps8);
      p_t n7{0.0, -1.0, 0.0};
      p_t n8{0.0, 1.0, 0.0};

      auto n3i = normal(t3);
      auto n4i = normal(t4);
      auto n5i = normal(t5);
      auto n6i = normal(t6);
      auto n7i = normal(t7);
      auto n8i = normal(t8);

      CHECK(n3i().norm() == 1.0);
      CHECK(n4i().norm() == 1.0);
      CHECK(n5i().norm() == 1.0);
      CHECK(n6i().norm() == 1.0);
      CHECK(n7i().norm() == 1.0);
      CHECK(n8i().norm() == 1.0);

      CHECK(n3i == n3);
      CHECK(n4i == n4);
      CHECK(n5i == n5);
      CHECK(n6i == n6);
      CHECK(n7i == n7);
      CHECK(n8i == n8);

      {  // check input triangles
        p_t n0{0.0, 0.0, 1.0};
        p_t n1{0.0, 0.0, 1.0};
        p_t n2{1. / std::sqrt(6.), -std::sqrt(2. / 3.), 1. / std::sqrt(6.)};

        auto n0i = normal(t0);
        auto n1i = normal(t1);
        auto n2i = normal(t2);
        CHECK(n0i().norm() == 1.0);
        CHECK(n1i().norm() == 1.0);
        CHECK(n2i().norm() == 1.0);

        CHECK(n0i == n0);
        CHECK(n1i == n1);
        CHECK(n2i == n2);
      }
    }

    {  // centroid
      p_t c0{2. / 3., 1. / 3., 0.};
      p_t c1{1. / 3., 2. / 3., 0.};
      p_t c2{2. / 3., 1. / 3., 0.};
      CHECK(centroid(t0) == c0);
      CHECK(centroid(t1) == c1);
      CHECK(centroid(t2) == c2);
    }

    {  // aabb
      ab_t a0{p_t{0.0, 0.0, -math::eps}, p_t{1.0, 1.0, +math::eps}};
      ab_t a1{p_t{0.0, 0.0, -math::eps}, p_t{1.0, 1.0, +math::eps}};
      ab_t a2{p_t{0.0, 0.0, -1.0}, p_t{1.0, 1.0, 1.0}};

      CHECK(axis_aligned_bounding_box(t0) == a0);
      CHECK(axis_aligned_bounding_box(t1) == a1);
      CHECK(axis_aligned_bounding_box(t2) == a2);
    }

    {  // square bounding box
      b_t b0{p_t{0.5, 0.5, 0.0}, 1.};
      b_t b1{p_t{0.5, 0.5, 0.0}, 1.};
      b_t b2{p_t{0.5, 0.5, 0.0}, 2.0};
      CHECK(square_bounding_box(t0) == b0);
      CHECK(square_bounding_box(t1) == b1);
      CHECK(square_bounding_box(t2) == b2);
    }

    {  // segments
      s_t s0[]
       = {s_t{ps0[0], ps0[1]}, s_t{ps0[1], ps0[2]}, s_t{ps0[2], ps0[0]}};
      s_t s1[]
       = {s_t{ps1[0], ps1[1]}, s_t{ps1[1], ps1[2]}, s_t{ps1[2], ps1[0]}};
      s_t s2[]
       = {s_t{ps2[0], ps2[1]}, s_t{ps2[1], ps2[2]}, s_t{ps2[2], ps2[0]}};

      CHECK(equal(segments(t0), s0));
      CHECK(equal(segments(t1), s1));
      CHECK(equal(segments(t2), s2));
    }
  }

  return test::result();
}
