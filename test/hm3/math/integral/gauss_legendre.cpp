#include <hm3/math/integral/gauss_legendre.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  {
    static constexpr dim_t nd = 1;
    using p_t                 = geometry::point<nd>;
    using b_t                 = geometry::box<nd>;

    b_t box(p_t{2.5}, 1.);  // [2, 3]

    auto f = [](p_t x) { return 1 * x(0) - 2; };

    num_t r = math::integral_gauss_legendre(f, box);
    CHECK(r == 0.5);

    auto g = [](p_t x) { return x(0); };

    num_t s = math::integral_gauss_legendre(g, box);
    CHECK(s == (0.5 * 1. * 1 + 2. * 1.));
  }

  {
    static constexpr dim_t nd = 2;
    using p_t                 = geometry::point<nd>;
    using b_t                 = geometry::box<nd>;

    b_t box(p_t{2.5, 2.5}, 1.);  // [2, 3]

    auto f = [](p_t x) { return 1 * x(0) - 2; };

    num_t r = math::integral_gauss_legendre(f, box);
    CHECK(r == 0.5 * 1.);

    auto g = [](p_t x) { return x(0); };

    num_t s = math::integral_gauss_legendre(g, box);
    CHECK(s == (0.5 * 1. * 1 + 2. * 1.) * 1);

    auto h = [](p_t x) { return x(1); };

    num_t t = math::integral_gauss_legendre(h, box);
    CHECK(t == (0.5 * 1. * 1 + 2. * 1.) * 1);
  }

  return test::result();
}
