#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/line.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Ad>
void test_line_line_intersection() {
  using p_t = point<Ad>;
  using v_t = vec<Ad>;
  using l_t = line<Ad>;

  auto o0 = p_t::constant(0.);
  auto d0 = v_t(v_t::constant(1.).normalized());
  auto o1 = p_t::constant(1.);

  // coincidental_lines lines:
  auto l0 = l_t(o0, d0);
  auto l1 = l_t(o1, d0);
  CHECK(coincidental_lines(l0, l1));
  CHECK(intersection.test(l0, l1));

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;

     if constexpr (Same<T, l_t>{}) {
       CHECK(i == l0);
     } else if constexpr (Same<T, p_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(false);
     } else {
       static_assert(always_false<T>{}, "non-exhaustive visitor");
     }
   },
   intersection(l0, l1));

  if (Ad == 1) { return; }

  // parallel but not coincidental_lines (no intersection):
  auto o2 = o0;
  o2(0)   = 1.;
  auto l2 = l_t(o2, d0);
  CHECK(parallel(l0, l2));
  CHECK(!coincidental_lines(l0, l2));
  CHECK(!intersection.test(l0, l2));

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;

     if constexpr (Same<T, l_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, p_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(true);
     } else {
       static_assert(always_false<T>{}, "non-exhaustive visitor");
     }
   },
   intersection(l0, l2));

  // intersection at l0's origin
  auto d1 = v_t::unit(0);
  auto l3 = l_t(o2, d1);
  CHECK(!parallel(l0, l3));
  CHECK(!coincidental_lines(l0, l3));
  CHECK(intersection.test(l0, l3));
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;

     if constexpr (Same<T, l_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, p_t>{}) {
       CHECK(i == o0);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(false);
     } else {
       static_assert(always_false<T>{}, "non-exhaustive visitor");
     }
   },
   intersection(l0, l3));

  // intersection at l1's origin
  auto l4 = l_t(o1, d1);
  CHECK(!parallel(l0, l4));
  CHECK(!coincidental_lines(l0, l4));
  CHECK(intersection.test(l0, l4));
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;

     if constexpr (Same<T, l_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, p_t>{}) {
       CHECK(i == o1);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(false);
     } else {
       static_assert(always_false<T>{}, "non-exhaustive visitor");
     }
   },
   intersection(l0, l4));

  // if 3d skew lines => no intersection
  if (Ad == 2) { return; }

  // intersection at some point
  auto o5 = o1;
  o5(2)   = 0.5;
  auto d5 = d1;
  d5(1)   = 1.0;
  auto l5 = l_t(o5, v_t(d5().normalized()));
  CHECK(!parallel(l0, l5));
  CHECK(!coincidental_lines(l0, l5));
  CHECK(intersection.test(l0, l5));
  auto i05 = p_t::constant(0.5);
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;

     if constexpr (Same<T, l_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, p_t>{}) {
       CHECK(i == i05);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(false);
     } else {
       static_assert(always_false<T>{}, "non-exhaustive visitor");
     }
   },
   intersection(l0, l5));

  auto o6 = o5;
  o6(0)   = 0.5;
  auto l6 = l_t(o6, d1);
  CHECK(!parallel(l0, l5));
  CHECK(!coincidental_lines(l0, l5));
  CHECK(!intersection.test(l0, l6));
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;

     if constexpr (Same<T, l_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, p_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(true);
     } else {
       static_assert(always_false<T>{}, "non-exhaustive visitor");
     }
   },
   intersection(l0, l6));
}

int main() {
  test_line_line_intersection<1>();
  test_line_line_intersection<2>();
  test_line_line_intersection<3>();

  return test::result();
}
