#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitives.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Ad>
void test_ray_point_intersection() {
  using p_t = point<Ad>;
  using v_t = vec<Ad>;
  using r_t = ray<Ad>;

  auto o0 = p_t::constant(1.);
  auto d0 = v_t(v_t::constant(1.).normalized());

  auto r = r_t(o0, d0);

  CHECK(intersection.test(r, o0));

  for (int i = -10; i < 1; ++i) {
    auto p = p_t::constant(num_t(i));
    CHECK(!intersection.test(r, p));
    visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, monostate>()) { CHECK(true); }
       else if
         constexpr(Same<T, p_t>{}) { CHECK(false); }
       else {
         static_assert(always_false<T>{}, "non-exhaustive variant");
       }
     },
     intersection(r, p));
  }

  for (int i = 1; i < 10; ++i) {
    auto p = p_t::constant(num_t(i));
    CHECK(intersection.test(r, p));
    visit(
     [&p](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, monostate>()) { CHECK(true); }
       else if
         constexpr(Same<T, p_t>{}) { CHECK(v == p); }
       else {
         static_assert(always_false<T>{}, "non-exhaustive variant");
       }
     },
     intersection(r, p));
  }

  if (Ad > 1) {
    for (int i = -10; i < 10; ++i) {
      auto p = p_t::constant(num_t(i));
      p(1)   = 0.;
      CHECK(!intersection.test(r, p));
      visit(
       [&](auto&& v) {
         using T = uncvref_t<decltype(v)>;
         if
           constexpr(Same<T, monostate>()) { CHECK(true); }
         else if
           constexpr(Same<T, p_t>{}) { CHECK(false); }
         else {
           static_assert(always_false<T>{}, "non-exhaustive variant");
         }
       },
       intersection(r, p));
    }
  }
}

int main() {
  test_ray_point_intersection<1>();
  test_ray_point_intersection<2>();
  test_ray_point_intersection<3>();

  return test::result();
}
