#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Ad>  //
void test_ray_segment_intersection() {
  using p_t = point<Ad>;
  using s_t = segment<Ad>;
  using v_t = vec<Ad>;
  using r_t = ray<Ad>;

  auto o0 = p_t::constant(1.);
  auto d0 = v_t(v_t::constant(1.).normalized());

  auto r = r_t(o0, d0);

  auto so0 = p_t::constant(-1.);
  auto so1 = p_t::constant(0.);
  auto so2 = p_t::constant(1.);
  auto so3 = p_t::constant(2.);
  auto so4 = p_t::constant(3.);

  // colinear segments:
  auto s0 = s_t(so0, so1);
  auto s1 = s_t(so1, so2);
  auto s2 = s_t(so1, so3);
  auto s3 = s_t(so2, so3);
  auto s4 = s_t(so3, so4);

  CHECK(!intersection.test(r, s0));
  CHECK(intersection.test(r, s1));
  CHECK(intersection.test(r, s2));
  CHECK(intersection.test(r, s3));
  CHECK(intersection.test(r, s4));
  {  // inverting the direction of the segments has no effect:
    CHECK(!intersection.test(r, direction.invert(s0)));
    CHECK(intersection.test(r, direction.invert(s1)));
    CHECK(intersection.test(r, direction.invert(s2)));
    CHECK(intersection.test(r, direction.invert(s3)));
    CHECK(intersection.test(r, direction.invert(s4)));
  }

  {  // inverting the direction of the ray changes the result
    CHECK(intersection.test(direction.invert(r), s0));
    CHECK(intersection.test(direction.invert(r), s1));
    CHECK(intersection.test(direction.invert(r), s2));
    CHECK(intersection.test(direction.invert(r), s3));
    CHECK(!intersection.test(direction.invert(r), s4));
  }
  {  // inverting the direction of the ray changes the result
    CHECK(intersection.test(direction.invert(r), direction.invert(s0)));
    CHECK(intersection.test(direction.invert(r), direction.invert(s1)));
    CHECK(intersection.test(direction.invert(r), direction.invert(s2)));
    CHECK(intersection.test(direction.invert(r), direction.invert(s3)));
    CHECK(!intersection.test(direction.invert(r), direction.invert(s4)));
  }

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(Same<T, s_t>{}) { CHECK(false); }
     else if
       constexpr(Same<T, p_t>{}) { CHECK(false); }
     else if
       constexpr(Same<T, monostate>{}) { CHECK(true); }
     else {
       static_assert(always_false<T>{}, "non-exhaustive variant");
     }
   },
   intersection(r, s0));

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(Same<T, s_t>{}) {
         // fmt::print("result: {}\n", i);
         CHECK(false);
       }
     else if
       constexpr(Same<T, p_t>{}) { CHECK(i == so2); }
     else if
       constexpr(Same<T, monostate>{}) { CHECK(false); }
     else {
       static_assert(always_false<T>{}, "non-exhaustive variant");
     }
   },
   intersection(r, s1));

  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(Same<T, s_t>{}) { CHECK(i == s3); }
     else if
       constexpr(Same<T, p_t>{}) { CHECK(false); }
     else if
       constexpr(Same<T, monostate>{}) { CHECK(false); }
     else {
       static_assert(always_false<T>{}, "non-exhaustive variant");
     }
   },
   intersection(r, s2));
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(Same<T, s_t>{}) { CHECK(i == s3); }
     else if
       constexpr(Same<T, p_t>{}) { CHECK(false); }
     else if
       constexpr(Same<T, monostate>{}) { CHECK(false); }
     else {
       static_assert(always_false<T>{}, "non-exhaustive variant");
     }
   },
   intersection(r, s3));
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(Same<T, s_t>{}) { CHECK(approx(i, s4)); }
     else if
       constexpr(Same<T, p_t>{}) { CHECK(false); }
     else if
       constexpr(Same<T, monostate>{}) { CHECK(false); }
     else {
       static_assert(always_false<T>{}, "non-exhaustive variant");
     }
   },
   intersection(r, s4));
}

int main() {
  test_ray_segment_intersection<1>();
  test_ray_segment_intersection<2>();
  test_ray_segment_intersection<3>();

  return test::result();
}
