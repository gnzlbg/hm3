#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd>  //
void test_point_point_intersection() {
  using p_t = geometry::point<Nd>;

  auto a = p_t::constant(0.);
  auto b = p_t::constant(0.);
  auto c = p_t::constant(1.);

  CHECK(geometry::intersection.test(a, b));
  CHECK(!geometry::intersection.test(a, c));

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if constexpr (Same<T, p_t>{}) {
       CHECK(i == a);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(false);
     } else {
       static_assert(always_false<T>{}, "forgot to handle a case");
     }
   },
   geometry::intersection(a, b));

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if constexpr (Same<T, p_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(true);
     }

     else {
       static_assert(always_false<T>{}, "forgot to handle a case");
     }
   },
   geometry::intersection(a, c));
}

int main() {
  test_point_point_intersection<1>();
  test_point_point_intersection<2>();
  test_point_point_intersection<3>();

  return test::result();
}
