#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/line.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Ad>
void test_line_point_intersection() {
  using p_t = geometry::point<Ad>;
  using v_t = geometry::vec<Ad>;
  using l_t = geometry::line<Ad>;

  auto o0 = p_t::constant(0.);
  auto d0 = v_t(v_t::constant(1.).normalized());

  auto l = l_t(o0, d0);

  CHECK(geometry::intersection.test(l, o0));

  for (int i = -10; i < 10; ++i) {
    auto p = p_t::constant(num_t(i));
    CHECK(geometry::intersection.test(l, p));
    visit(
     [&p](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if constexpr (Same<T, monostate>()) {
         CHECK(false);
       } else if constexpr (Same<T, p_t>{}) {
         CHECK(v == p);
       } else {
         static_assert(always_false<T>{}, "non-exhaustive variant");
       }
     },
     geometry::intersection(l, p));
  }

  if (Ad > 1) {
    for (int i = 1; i < 10; ++i) {
      auto p = p_t::constant(num_t(i));
      p(1)   = 0.;
      CHECK(!geometry::intersection.test(l, p));
      visit(
       [](auto&& v) {
         using T = uncvref_t<decltype(v)>;
         if constexpr (Same<T, monostate>()) {
           CHECK(true);
         } else if constexpr (Same<T, p_t>{}) {
           CHECK(false);
         } else {
           static_assert(always_false<T>{}, "non-exhaustive variant");
         }
       },
       geometry::intersection(l, p));
    }
  }
}

int main() {
  test_line_point_intersection<1>();
  test_line_point_intersection<2>();
  test_line_point_intersection<3>();

  return test::result();
}
