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

  return test::result();
}
