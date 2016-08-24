#include <hm3/geometry/algorithm/intersection/ray_point.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd>
void test_ray_point_intersection() {
  using p_t = geometry::point<Nd>;
  using v_t = geometry::vec<Nd>;
  using r_t = geometry::ray<Nd>;

  auto o0 = p_t::constant(1.);
  auto d0 = v_t(v_t::constant(1.).normalized());

  auto r = r_t(o0, d0);

  CHECK(geometry::intersection.test(r, o0));

  for (int i = -10; i < 1; ++i) {
    auto p = p_t::constant(num_t(i));
    CHECK(!geometry::intersection.test(r, p));
    visit(
     [&p](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, monostate>()) { CHECK(true); }
       else if
         constexpr(Same<T, p_t>{}) { CHECK(false); }
       else {
         static_assert(fail<T>{}, "non-exhaustive variant");
       }
     },
     geometry::intersection(r, p));
  }

  for (int i = 1; i < 10; ++i) {
    auto p = p_t::constant(num_t(i));
    CHECK(geometry::intersection.test(r, p));
    visit(
     [&p](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, monostate>()) { CHECK(true); }
       else if
         constexpr(Same<T, p_t>{}) { CHECK(v == p); }
       else {
         static_assert(fail<T>{}, "non-exhaustive variant");
       }
     },
     geometry::intersection(r, p));
  }

  if (Nd > 1) {
    for (int i = -10; i < 10; ++i) {
      auto p = p_t::constant(num_t(i));
      p(1)   = 0.;
      CHECK(!geometry::intersection.test(r, p));
      visit(
       [&p](auto&& v) {
         using T = uncvref_t<decltype(v)>;
         if
           constexpr(Same<T, monostate>()) { CHECK(true); }
         else if
           constexpr(Same<T, p_t>{}) { CHECK(false); }
         else {
           static_assert(fail<T>{}, "non-exhaustive variant");
         }
       },
       geometry::intersection(r, p));
    }
  }
}

int main() {
  test_ray_point_intersection<1>();
  test_ray_point_intersection<2>();
  test_ray_point_intersection<3>();

  return test::result();
}
