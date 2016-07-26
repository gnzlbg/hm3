#include <hm3/geometry/aabb.hpp>
#include <hm3/utility/test.hpp>
// clang-format off
#include "aabb.hpp"
// clang-format on

using namespace hm3;

int main() {
  test_unit_aabbs<aabb>();

  {  // aabb non box
    static constexpr dim_t nd = 2;

    using p_t = point<nd>;
    p_t pmin{0., 0.};
    p_t pmax{1.0, .5};
    aabb<nd> a(pmin, pmax);

    // x_min/x_max:
    CHECK(x_min(a) == pmin);
    CHECK(x_max(a) == pmax);

    // length:
    CHECK(length(a, 0) == 1.0);
    CHECK(length(a, 1) == 0.5);
    CHECK(lengths(a) == p_t{1.0, 0.5});

    // bounds:
    CHECK(bounds(a).min == pmin);
    CHECK(bounds(a).max == pmax);

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
    CHECK(!geometry::empty(a));
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
      CHECK(!intersects.query(a, b));
      CHECK(!intersects.query(b, a));

      // Case B1:
      CHECK(intersects.query(c, b));
      // Case B2:
      CHECK(intersects.query(b, c));

      // Case C1:
      CHECK(intersects.query(a, a));
      CHECK(intersects.query(b, b));
      CHECK(intersects.query(a, d));
      CHECK(intersects.query(d, a));
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
      CHECK(!intersects.query(a, b));
      CHECK(!intersects.query(b, a));
      CHECK(!intersects.query(e, b));
      CHECK(!intersects.query(e, c));

      // Case B1:
      CHECK(intersects.query(c, b));
      // Case B2:
      CHECK(intersects.query(b, c));

      // Case C1: volume
      CHECK(intersects.query(a, a));
      CHECK(intersects.query(b, b));
      CHECK(intersects.query(a, d));
      CHECK(intersects.query(d, a));

      // Case C2: face
      CHECK(intersects.query(a, e));
      CHECK(intersects.query(e, a));

      // Case C3: edge (point)
      CHECK(intersects.query(a, f));
      CHECK(intersects.query(f, a));
      CHECK(intersects.query(b, f));
      CHECK(intersects.query(f, b));
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
      CHECK(!intersects.query(a, b));
      CHECK(!intersects.query(b, a));
      CHECK(!intersects.query(e, b));
      CHECK(!intersects.query(e, c));

      // Case B1:
      CHECK(intersects.query(c, b));
      // Case B2:
      CHECK(intersects.query(b, c));

      // Case C1: volume
      CHECK(intersects.query(a, a));
      CHECK(intersects.query(b, b));
      CHECK(intersects.query(a, d));
      CHECK(intersects.query(d, a));

      // Case C2: face
      CHECK(intersects.query(a, e));
      CHECK(intersects.query(e, a));

      // Case C3: edge (line)
      CHECK(intersects.query(a, g));
      CHECK(intersects.query(g, a));

      // Case C4: corner (point)
      CHECK(intersects.query(a, f));
      CHECK(intersects.query(f, a));
      CHECK(intersects.query(b, f));
      CHECK(intersects.query(f, b));
    }
  }

  return test::result();
}
