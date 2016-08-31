#include <hm3/geometry/algorithm/sd_intersection/triangle.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/sd/combinators.hpp>
#include <hm3/geometry/sd/edge.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

void test_2d_triangle_intersection() {
  static constexpr dim_t nd = 2;

  using p_t  = geometry::point<nd>;
  using s_t  = geometry::segment<nd>;
  using pl_t = geometry::small_polyline<nd, 3>;
  using t_t  = geometry::triangle<nd>;
  using e_t  = geometry::sd::fixed_edge<nd>;
  using geometry::sd_intersection;

  t_t t0({{{p_t{0.0, 0.0}, p_t{1.0, 0.0}, p_t{0.0, 1.0}}}});

  using r_t = decltype(sd_intersection(t0, e_t{}));

  {  // case 0: no zero vertices (0)
    e_t e({0.5, 0.5}, {-1., 0.});
    CHECK(sd_intersection(t0, e) == r_t{s_t(p_t{0.5, 0.0}, p_t{0.5, 0.5})});
  }

  {  // case 0: no zero vertices (1)
    e_t e({0.5, 0.5}, {0., -1.});
    CHECK(sd_intersection(t0, e) == r_t{s_t(p_t{0.5, 0.5}, p_t{0.0, 0.5})});
  }

  {  // case 1: one zero vertex (0)
    e_t e({1.0, 0.5}, {-1., 0.});
    CHECK(sd_intersection(t0, e) == r_t{p_t{1.0, 0.0}});
  }

  {  // case 1: one zero vertex (1)
    e_t e({1.0, 0.0}, {-0.5, -1.0});
    CHECK(sd_intersection(t0, e) == r_t{s_t(p_t{1.0, 0.0}, p_t{0.0, 0.5})});
  }

  {  // case 2: two zero vertices (0)
    e_t e({0.0, 0.0}, {0.0, 1.0});
    CHECK(sd_intersection(t0, e) == r_t{s_t(p_t{0.0, 0.0}, p_t{1.0, 0.0})});
  }

  {  // case 3: three zero vertices (0): corner like sdf
    e_t e0({0.0, 0.0}, {0.0, 1.0});
    e_t e1({0.0, 0.0}, {1.0, 0.0});
    auto corner_sdf = geometry::sd::neg_union(e0, e1);
    CHECK(sd_intersection(t0, corner_sdf)
          == r_t{pl_t({p_t{0.0, 1.0}, p_t{0.0, 0.0}, p_t{1.0, 0.0}})});
  }

  {  // case 3: three zero vertices (1): triangle-like sdf
    e_t e0({0.0, 0.0}, {0.0, 1.0});
    e_t e1({0.0, 0.0}, {1.0, 0.0});
    e_t e2({1.0, 0.0}, {-1.0, -1.0});
    auto tri_sdf = geometry::sd::neg_union(e0, e1, e2);
    CHECK(sd_intersection(t0, tri_sdf) == r_t{t0});
  }

  {  // case 3: three zero vertices (2): split like stl
    e_t e0({0.0, 0.0}, {+1., 0.});
    e_t e1({1.0, 0.0}, {-1., 0.});
    auto split_sdf = geometry::sd::neg_union(e0, e1);
    CHECK(sd_intersection(t0, split_sdf)
          == r_t{make_pair(p_t{1.0, 0.}, s_t(p_t{0.0, 1.0}, p_t{0.0, 0.0}))});
  }
}

int main() {
  test_2d_triangle_intersection();
  return test::result();
}
