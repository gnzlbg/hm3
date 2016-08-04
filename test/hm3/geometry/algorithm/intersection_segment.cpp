#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  {  // 1D segment intersection
    using p_t = geometry::point<1>;
    using s_t = geometry::segment<1>;

    p_t p0 = {0.};
    p_t p1 = {1.};
    p_t p2 = {0.5};
    p_t p3 = {2.};
    p_t p4 = {3.};

    // s0 - s1 intersect over volume (one sided)
    // s1 - s2 intersect over a face (point)
    // s0 - s2 don't intersect
    // s3 - s4 intersect over a volume (two-sided)

    auto s0 = s_t::through(p0, p1);
    auto s1 = s_t::through(p2, p3);
    auto s2 = s_t::through(p3, p4);
    auto s3 = s_t::through(p0, p3);
    auto s4 = s_t::through(p2, p1);

    using geometry::intersection;

    CHECK(intersection.test(s0, s1));
    CHECK(intersection.test(s1, s0));
    CHECK(intersection.test(s1, s2));
    CHECK(intersection.test(s2, s1));
    CHECK(intersection.test(s3, s4));
    CHECK(!intersection.test(s0, s2));
    CHECK(!intersection.test(s2, s0));
    /*
    // volume intersection result:
    {
      auto inside_r   = s_t::through(p2, p1);
      auto outside_r0 = s_t::through(p0, p2);
      auto outside_r1 = s_t::through(p1, p3);
      auto boundary_0 = p2;
      auto boundary_1 = p1;

      using geometry::intersection;

      auto ir = intersection(s0, s1);

      CHECK(ir.inside.size() == 1);
      CHECK(ir.inside[0].domain == inside_r);
      CHECK(ir.inside[0].ids.size() == 2);
      CHECK(ir.inside[0].ids[0] == 0);
      CHECK(ir.inside[0].ids[1] == 1);

      CHECK(ir.outside.size() == 2);

      CHECK(ir.outside[0].domain == outside_r0);
      CHECK(ir.outside[0].ids.size() == 1);
      CHECK(ir.outside[0].ids[0] == 0);

      CHECK(ir.outside[1].domain == outside_r1);
      CHECK(ir.outside[1].ids.size() == 1);
      CHECK(ir.outside[1].ids[0] == 1);

      CHECK(ir.boundary.size() == 2);

      CHECK(ir.boundary[0].domain == boundary_0);
      CHECK(ir.boundary[0].ids.size() == 2);
      CHECK(ir.boundary[0].ids[0] == 0);
      CHECK(ir.boundary[0].ids[1] == 1);

      CHECK(ir.boundary[1].domain == boundary_0);
      CHECK(ir.boundary[1].ids.size() == 2);
      CHECK(ir.boundary[1].ids[0] == 0);
      CHECK(ir.boundary[1].ids[1] == 1);
    }

    // face intersection result
    {
      auto ir = intersection(s1, s2);
      CHECK(ir.boundary.size() == 1);
      CHECK(ir.boundary.domain == p3);
    }

    // empty intersection result
    {
      auto ir = intersection(s0, s2);
      CHECK(!ir);
    }
    */
  }

  {  // 2D segment intersection
    using p_t = geometry::point<2>;
    using s_t = geometry::segment<2>;

    p_t p0 = {0., 0.};
    p_t p1 = {1., 1.};
    p_t p2 = {2., 2.};
    p_t p3 = {3., 3.};
    p_t p4 = {4., 4.};
    p_t p5 = {5., 5.};

    p_t p6 = {3., 2.};
    p_t p7 = {4., 3.};

    p_t p8 = {0., 2.};
    p_t p9 = {2., 0.};

    p_t p10 = {2., 0.};
    p_t p11 = {0., 2.};

    // s0 - s1 intersect over volume (two-sided)
    // s2 - s3 intersect over volume (one-sided)
    // s0 - s4 don't intersect (but in same line)
    // s0 - s5 don't intersect (but parallel)
    // s0 - s6 intersect over point (middle)
    // s7 - s8 intersect over point (side)

    auto s0 = s_t::through(p0, p3);
    auto s1 = s_t::through(p1, p2);
    auto s2 = s_t::through(p0, p2);

    auto s3  = s_t::through(p1, p3);
    auto s4  = s_t::through(p4, p5);
    auto s4_ = s_t::through(p5, p4);
    auto s5  = s_t::through(p6, p7);
    auto s6  = s_t::through(p8, p9);

    auto s7 = s_t::through(p0, p1);

    using geometry::intersection;

    CHECK(intersection.test(s0, s1));
    CHECK(intersection.test(s2, s3));
    CHECK(!intersection.test(s0, s4));
    CHECK(!intersection.test(s0, s4_));
    CHECK(!intersection.test(s0, s5));
    CHECK(intersection.test(s0, s6));
    CHECK(intersection.test(s7, s6));
  }

  {  // 3D segment intersection (TODO)
  }

  return test::result();
}
