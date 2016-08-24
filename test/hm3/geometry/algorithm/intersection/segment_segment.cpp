#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <typename S, typename R, dim_t Nd = uncvref_t<S>::dimension()>
void test_intersection(S const& s0, S const& s1, R&& r) {
  using p_t = geometry::point<Nd>;
  using s_t = geometry::segment<Nd>;
  using geometry::intersection;
  using geometry::direction;
  auto s0i = direction.invert(s0);
  auto s1i = direction.invert(s1);

  R ir;
  // invert the result:
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     if
       constexpr(Same<T, s_t>{}) { ir = direction.invert(i); }
     else if
       constexpr(Same<T, p_t>{}) { ir = i; }
     else if
       constexpr(Same<T, monostate>{}) { ir = i; }
     else {
       static_assert(fail<T>{}, "non-exhaustive variant");
     }
   },
   r);

  // intersection.test
  visit(
   [&](auto&& i) {
     using T = uncvref_t<decltype(i)>;
     bool expected;
     if
       constexpr(Same<T, monostate>{}) { expected = false; }
     else if
       constexpr(Same<T, p_t>{} or Same<T, s_t>{}) { expected = true; }
     else {
       static_assert(fail<T>{}, "non-exhaustive variant");
     }

     CHECK(intersection.test(s0, s1) == expected);
     CHECK(intersection.test(s0i, s1) == expected);
     CHECK(intersection.test(s0, s1i) == expected);
     CHECK(intersection.test(s0i, s1i) == expected);

     CHECK(intersection.test(s1, s0) == expected);
     CHECK(intersection.test(s1i, s0) == expected);
     CHECK(intersection.test(s1, s0i) == expected);
     CHECK(intersection.test(s1i, s0i) == expected);
   },
   r);

  // intersection invariants: result independent of the order of the arguments
  // if we need this we can try to work it out later, probably not worth it
  // CHECK(intersection(s0, s1) == intersection(s1, s0));
  // CHECK(intersection(s0i, s1) == intersection(s1, s0i));
  // CHECK(intersection(s0, s1i) == intersection(s1i, s0));
  // CHECK(intersection(s0i, s1i) == intersection(s1i, s0i));

  CHECK(intersection(s0, s1) == r);
  if (intersection(s0i, s1) == r) {
    CHECK(intersection(s0i, s1) == r);
  } else {
    CHECK(intersection(s0i, s1) == ir);
  }
  CHECK(intersection(s0, s1i) == r);
  if (intersection(s0i, s1i) == r) {  // should alwaays be ir?
    CHECK(intersection(s0i, s1i) == r);
  } else {
    CHECK(intersection(s0i, s1i) == ir);
  }
  CHECK(intersection(s1, s0) == r);
  if (intersection(s1i, s0) == r) {
    CHECK(intersection(s1i, s0) == r);
  } else {
    CHECK(intersection(s1i, s0) == ir);
  }
  CHECK(intersection(s1, s0i) == r);
  if (intersection(s1i, s0i) == r) {  // ir
    CHECK(intersection(s1i, s0i) == r);
  } else {
    CHECK(intersection(s1i, s0i) == ir);
  }
}

int main() {
  {  // 1D segment intersection
    using p_t = geometry::point<1>;
    using s_t = geometry::segment<1>;

    auto p0 = p_t{0.};
    auto p1 = p_t{1.};
    auto p2 = p_t{0.5};
    auto p3 = p_t{2.};
    auto p4 = p_t{3.};

    // s0 - s1 intersect over volume (one sided)
    // s1 - s2 intersect over a face (point)
    // s0 - s2 don't intersect
    // s3 - s4 intersect over a volume (two-sided)

    auto s0 = s_t::through(p0, p1);
    auto s1 = s_t::through(p2, p3);
    auto s2 = s_t::through(p3, p4);
    auto s3 = s_t::through(p0, p3);
    auto s4 = s_t::through(p2, p1);

    using geometry::intersection;
    using r_t = decltype(intersection(s0, s1));

    test_intersection(s0, s1, r_t{s4});
    test_intersection(s1, s2, r_t{p3});
    test_intersection(s3, s4, r_t{s4});
    test_intersection(s0, s2, r_t{});
  }

  {  // 2D segment intersection
    using p_t = geometry::point<2>;
    using s_t = geometry::segment<2>;

    auto p0 = p_t{0., 0.};
    auto p1 = p_t{1., 1.};
    auto p2 = p_t{2., 2.};
    auto p3 = p_t{3., 3.};
    auto p4 = p_t{4., 4.};
    auto p5 = p_t{5., 5.};

    auto p6 = p_t{3., 2.};
    auto p7 = p_t{4., 3.};

    auto p8 = p_t{0., 2.};
    auto p9 = p_t{2., 0.};

    // s0 - s1 intersect over volume (two-sided)
    // s2 - s3 intersect over volume (one-sided)
    // s0 - s4 don't intersect (but in same line)
    // s0 - s5 don't intersect (but parallel)
    // s0 - s6 intersect over point (middle)
    // s7 - s8 intersect over point (side)

    auto s0 = s_t::through(p0, p3);
    auto s1 = s_t::through(p1, p2);
    auto s2 = s_t::through(p0, p2);

    auto s3  = s_t::through(p1, p3);
    auto s4  = s_t::through(p4, p5);
    auto s4_ = s_t::through(p5, p4);
    auto s5  = s_t::through(p6, p7);
    auto s6  = s_t::through(p8, p9);

    auto s7 = s_t::through(p0, p1);

    using geometry::intersection;
    using r_t = decltype(intersection(s0, s1));

    test_intersection(s0, s1, r_t{s1});
    test_intersection(s2, s3, r_t{s1});
    test_intersection(s0, s4, r_t{});
    test_intersection(s0, s4_, r_t{});
    test_intersection(s0, s5, r_t{});
    test_intersection(s0, s6, r_t{p_t::constant(1.)});
    test_intersection(s7, s6, r_t{p_t::constant(1.)});
  }
  {  // 3D segment intersection
    using p_t = geometry::point<3>;
    using s_t = geometry::segment<3>;

    auto p0 = p_t{0., 0., 0.};
    auto p1 = p_t{1., 1., 1.};
    auto p2 = p_t{2., 2., 2.};
    auto p3 = p_t{3., 3., 3.};
    auto p4 = p_t{4., 4., 4.};
    auto p5 = p_t{5., 5., 5.};

    // auto p6 = p_t{3., 2.};
    // auto p7 = p_t{4., 3.};

    // auto p8 = p_t{0., 2.};
    // auto p9 = p_t{2., 0.};

    // s0 - s1 intersect over volume (two-sided)
    // s2 - s3 intersect over volume (one-sided)
    // s0 - s4 don't intersect (but in same line)
    // s0 - s5 don't intersect (but parallel)
    // s0 - s6 intersect over point (middle)
    // s7 - s8 intersect over point (side)

    auto s0 = s_t::through(p0, p3);
    auto s1 = s_t::through(p1, p2);
    auto s2 = s_t::through(p0, p2);

    auto s3  = s_t::through(p1, p3);
    auto s4  = s_t::through(p4, p5);
    auto s4_ = s_t::through(p5, p4);
    // auto s5  = s_t::through(p6, p7);
    // auto s6  = s_t::through(p8, p9);

    // auto s7 = s_t::through(p0, p1);

    using geometry::intersection;
    using r_t = decltype(intersection(s0, s1));

    test_intersection(s0, s1, r_t{s1});
    test_intersection(s2, s3, r_t{s1});
    test_intersection(s0, s4, r_t{});
    test_intersection(s0, s4_, r_t{});
    // test_intersection(s0, s4_, r_t{});
    CHECK(intersection.test(s0, s1));
    CHECK(intersection.test(s2, s3));
    // CHECK(!intersection.test(s0, s5));
    // CHECK(intersection.test(s0, s6));
    // CHECK(intersection.test(s7, s6));
  }

  return test::result();
}
