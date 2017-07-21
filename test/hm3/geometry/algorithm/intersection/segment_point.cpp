#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Ad>  //
void test_segment_point_intersection() {
  using p_t = geometry::point<Ad>;
  using s_t = geometry::segment<Ad>;

  auto x0 = p_t::constant(0.);
  auto x1 = p_t::constant(2.);

  auto s = s_t(x0, x1);

  auto m = p_t::constant(0.5);

  auto o0 = p_t::constant(-1.);
  auto o1 = p_t::constant(3.);

  auto o2 = p_t::constant(0.5);
  o2(0)   = 3;

  CHECK(geometry::intersection.test(s, x0));
  CHECK(geometry::intersection.test(s, x1));
  CHECK(geometry::intersection.test(s, m));
  CHECK(!geometry::intersection.test(s, o0));
  CHECK(!geometry::intersection.test(s, o1));
  CHECK(!geometry::intersection.test(s, o2));

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if constexpr (Same<T, p_t>{}) {
       CHECK(i == x0);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(false);
     } else {
       HM3_STATIC_ASSERT_EXHAUSTIVE_VISITOR(T);
     }
   },
   geometry::intersection(s, x0));

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if constexpr (Same<T, p_t>{}) {
       CHECK(false);
     } else if constexpr (Same<T, monostate>{}) {
       CHECK(true);
     }

     else {
       HM3_STATIC_ASSERT_EXHAUSTIVE_VISITOR(T);
     }
   },
   geometry::intersection(s, o0));
}

int main() {
  test_segment_point_intersection<1>();
  test_segment_point_intersection<2>();
  test_segment_point_intersection<3>();

  return test::result();
}
