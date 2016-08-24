#include "aabb.hpp"
#include <hm3/geometry/algorithm/intersection/aabb.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/utility/test.hpp>

int main() {
  test_unit_aabbs<aabb>();

  {  // aabb non box
    static constexpr dim_t nd = 2;

    using p_t = point<nd>;
    using v_t = vec<nd>;
    p_t pmin{0., 0.};
    p_t pmax{1.0, .5};
    aabb<nd> a(pmin, pmax);

    // x_min/x_max:
    CHECK(x_min(a) == pmin);
    CHECK(x_max(a) == pmax);

    // length:
    CHECK(bounding_length(a, 0) == 1.0);
    CHECK(bounding_length(a, 1) == 0.5);
    CHECK(bounding_length.all(a) == v_t{1.0, 0.5});

    // size:
    CHECK(vertex_size(a) == math::ipow(dim_t{2}, nd));

    // vertices:
    auto vxs = vertices(a);
    CHECK(vxs[0] == p_t{0.0, 0.0});
    CHECK(vxs[1] == p_t{1.0, 0.0});
    CHECK(vxs[2] == p_t{1.0, 0.5});
    CHECK(vxs[3] == p_t{0.0, 0.5});

    // centroid:
    CHECK(centroid(a) == p_t{0.5, 0.25});

    // contains:
    CHECK(contains(a, p_t{0.1, 0.1}));
    CHECK(contains(a, p_t{0.1, 0.4}));
    CHECK(contains(a, p_t{0.9, 0.1}));
    CHECK(contains(a, p_t{0.9, 0.4}));
    CHECK(!contains(a, p_t{-0.1, -0.1}));
    CHECK(!contains(a, p_t{-0.1, 0.6}));
    CHECK(!contains(a, p_t{1.1, -0.1}));
    CHECK(!contains(a, p_t{1.1, 0.6}));

    // volume:
    CHECK(volume(a) == 0.5);

    // is box
    CHECK(!is_box(a));
  }

  {  // intersection tests

    {  // 1D
      static constexpr dim_t nd = 1;

      using p_t = point<nd>;
      using a_t = aabb<nd>;

      a_t a{p_t::constant(-1.), p_t::constant(0.)};
      a_t b{p_t::constant(1.), p_t::constant(2.)};
      a_t c{p_t::constant(1.25), p_t::constant(1.75)};
      a_t d{p_t::constant(-.5), p_t::constant(.5)};

      // Case A:
      CHECK(intersection.test(a, b));
      CHECK(intersection.test(b, a));

      // Case B1:
      CHECK(!intersection.test(c, b));
      // Case B2:
      CHECK(!intersection.test(b, c));

      // Case C1:
      CHECK(!intersection.test(a, a));
      CHECK(!intersection.test(b, b));
      CHECK(!intersection.test(a, d));
      CHECK(!intersection.test(d, a));
    }

    {  // 2D:
      static constexpr dim_t nd = 2;

      using p_t = point<nd>;
      using a_t = aabb<nd>;

      a_t a{p_t::constant(-1.), p_t::constant(0.)};
      a_t b{p_t::constant(1.), p_t::constant(2.)};
      a_t c{p_t::constant(1.25), p_t::constant(1.75)};
      a_t d{p_t::constant(-.5), p_t::constant(.5)};
      a_t e{p_t{0., -1.}, p_t{1., 0.}};
      a_t f{p_t::constant(0.), p_t::constant(1.)};

      // Case A:
      CHECK(intersection.test(a, b));
      CHECK(intersection.test(b, a));
      CHECK(intersection.test(e, b));
      CHECK(intersection.test(e, c));

      // Case B1:
      CHECK(!intersection.test(c, b));
      // Case B2:
      CHECK(!intersection.test(b, c));

      // Case C1: volume
      CHECK(!intersection.test(a, a));
      CHECK(!intersection.test(b, b));
      CHECK(!intersection.test(a, d));
      CHECK(!intersection.test(d, a));

      // Case C2: face
      CHECK(!intersection.test(a, e));
      CHECK(!intersection.test(e, a));

      // Case C3: edge (point)
      CHECK(!intersection.test(a, f));
      CHECK(!intersection.test(f, a));
      CHECK(!intersection.test(b, f));
      CHECK(!intersection.test(f, b));
    }

    {  // 3D:
      static constexpr dim_t nd = 3;

      using p_t = point<nd>;
      using a_t = aabb<nd>;

      a_t a{p_t::constant(-1.), p_t::constant(0.)};
      a_t b{p_t::constant(1.), p_t::constant(2.)};
      a_t c{p_t::constant(1.25), p_t::constant(1.75)};
      a_t d{p_t::constant(-.5), p_t::constant(.5)};
      a_t e{p_t{0., -1., -1.}, p_t{1., 0., 0.}};
      a_t f{p_t::constant(0.), p_t::constant(1.)};
      a_t g{p_t{0., 0., -1}, p_t{1., 0., 0.}};

      // Case A:
      CHECK(intersection.test(a, b));
      CHECK(intersection.test(b, a));
      CHECK(intersection.test(e, b));
      CHECK(intersection.test(e, c));

      // Case B1:
      CHECK(!intersection.test(c, b));
      // Case B2:
      CHECK(!intersection.test(b, c));

      // Case C1: volume
      CHECK(!intersection.test(a, a));
      CHECK(!intersection.test(b, b));
      CHECK(!intersection.test(a, d));
      CHECK(!intersection.test(d, a));

      // Case C2: face
      CHECK(!intersection.test(a, e));
      CHECK(!intersection.test(e, a));

      // Case C3: edge (line)
      CHECK(!intersection.test(a, g));
      CHECK(!intersection.test(g, a));

      // Case C4: corner (point)
      CHECK(!intersection.test(a, f));
      CHECK(!intersection.test(f, a));
      CHECK(!intersection.test(b, f));
      CHECK(!intersection.test(f, b));
    }
  }

  return test::result();
}
