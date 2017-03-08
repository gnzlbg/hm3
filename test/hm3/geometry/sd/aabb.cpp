#include <hm3/geometry/sd.hpp>
#include <hm3/geometry/sd/functions/aabb.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  {  // 2D
    static constexpr dim_t ad = 2;
    using p_t                 = point<2>;
    using oa_t                = sd::aabb_at_origin<ad>;

    p_t o = {0., 0.};

    {
      oa_t unit_aabb;

      p_t i0 = p_t::constant(-0.25);
      p_t i1 = p_t::constant(0.25);
      p_t i2 = {0.25, -0.25};
      p_t i3 = {-0.25, 0.25};

      // points inside
      CHECK(unit_aabb(o) == -0.5);
      CHECK(unit_aabb(i0) == -0.25);
      CHECK(unit_aabb(i1) == -0.25);
      CHECK(unit_aabb(i2) == -0.25);
      CHECK(unit_aabb(i3) == -0.25);

      // point boundary: corners
      p_t c0 = p_t{-0.5, -0.5};
      p_t c1 = p_t{0.5, -0.5};
      p_t c2 = p_t{0.5, 0.5};
      p_t c3 = p_t{-0.5, 0.5};

      CHECK(unit_aabb(c0) == 0.0);
      CHECK(unit_aabb(c1) == 0.0);
      CHECK(unit_aabb(c2) == 0.0);
      CHECK(unit_aabb(c3) == 0.0);

      // point boundary: mid-planes
      p_t m0 = p_t{-0.5, 0.};
      p_t m1 = p_t{0.0, -0.5};
      p_t m2 = p_t{0.5, 0.};
      p_t m3 = p_t{0., 0.5};

      CHECK(unit_aabb(m0) == 0.0);
      CHECK(unit_aabb(m1) == 0.0);
      CHECK(unit_aabb(m2) == 0.0);
      CHECK(unit_aabb(m3) == 0.0);

      // points outside: corner regions
      p_t co0 = p_t{-1, -1};
      p_t co1 = p_t{1, -1};
      p_t co2 = p_t{1, 1};
      p_t co3 = p_t{-1, 1};

      CHECK(unit_aabb(co0) == 0.5 * std::sqrt(2.));
      CHECK(unit_aabb(co1) == 0.5 * std::sqrt(2.));
      CHECK(unit_aabb(co2) == 0.5 * std::sqrt(2.));
      CHECK(unit_aabb(co3) == 0.5 * std::sqrt(2.));

      // points outside: mid planes
      p_t mo0 = p_t{-1, 0};
      p_t mo1 = p_t{0, -1};
      p_t mo2 = p_t{1, 0};
      p_t mo3 = p_t{0, 1};

      CHECK(unit_aabb(mo0) == 0.5);
      CHECK(unit_aabb(mo1) == 0.5);
      CHECK(unit_aabb(mo2) == 0.5);
      CHECK(unit_aabb(mo3) == 0.5);
    }
  }

  // TODO: test 1D
  // TODO: test 3D
  // TODO: test rotations

  return test::result();
}
