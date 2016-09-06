#include <hm3/geometry/algorithm/intersection/aabb_aabb.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  {  // 1D
    static constexpr dim_t nd = 1;
    using a_t                 = geometry::aabb<nd>;
    using p_t                 = geometry::point<nd>;

    auto pM2 = p_t::constant(-2.);
    auto pM1 = p_t::constant(-1.);
    auto p0  = p_t::constant(0.);
    auto p1  = p_t::constant(1.);
    auto p2  = p_t::constant(2.);
    auto p3  = p_t::constant(3.);

    a_t a0(p0, p1);
    a_t a1(p1, p2);
    a_t a2(p2, p3);
    a_t a3(p0, p2);
    a_t a4(p1, p3);
    a_t a5(pM2, pM1);

    using geometry::intersection;
    CHECK(intersection.test(a0, a1));
    CHECK(!intersection.test(a0, a2));
    CHECK(intersection.test(a0, a0));
    CHECK(intersection.test(a3, a4));
    CHECK(intersection.test(a0, a4));
    CHECK(!intersection.test(a0, a5));

    using r_t = decltype(intersection(a0, a1));
    CHECK(intersection(a0, a1) == r_t{p1});
    CHECK(intersection(a0, a2) == r_t{});
    CHECK(intersection(a0, a0) == r_t{a0});
    CHECK(intersection(a3, a4) == r_t{a1});
    CHECK(intersection(a0, a4) == r_t{p1});
    CHECK(intersection(a0, a5) == r_t{});
  }

  {  // 2D
    static constexpr dim_t nd = 2;
    using a_t                 = geometry::aabb<nd>;
    using p_t                 = geometry::point<nd>;
    using s_t                 = geometry::segment<nd>;

    auto pM2 = p_t::constant(-2.);
    auto pM1 = p_t::constant(-1.);
    auto p0  = p_t::constant(0.);
    auto p1  = p_t::constant(1.);
    auto p2  = p_t::constant(2.);
    auto p3  = p_t::constant(3.);

    a_t a0(p0, p1);
    a_t a1(p1, p2);
    a_t a2(p2, p3);
    a_t a3(p0, p2);
    a_t a4(p1, p3);
    a_t a5(pM2, pM1);

    a_t a6(p_t{-1., 0.}, p_t{0., 1.});
    a_t a7(p_t{1., 0.}, p_t{2., 1.});

    s_t s0(p0, p_t{0., 1.});
    s_t s1(p_t{1., 0.}, p1);
    s_t s2(p1, p_t{2., 1.});

    using geometry::intersection;
    CHECK(intersection.test(a0, a1));
    CHECK(!intersection.test(a0, a2));
    CHECK(intersection.test(a0, a0));
    CHECK(intersection.test(a3, a4));
    CHECK(intersection.test(a0, a4));
    CHECK(!intersection.test(a0, a5));

    using r_t = decltype(intersection(a0, a1));
    CHECK(intersection(a0, a1) == r_t{p1});
    CHECK(intersection(a0, a2) == r_t{});
    CHECK(intersection(a0, a0) == r_t{a0});
    CHECK(intersection(a3, a4) == r_t{a1});
    CHECK(intersection(a1, a4) == r_t{a1});
    CHECK(intersection(a0, a4) == r_t{p1});
    CHECK(intersection(a0, a5) == r_t{});
    CHECK(intersection(a0, a6) == r_t{s0});
    CHECK(intersection(a0, a7) == r_t{s1});
    CHECK(intersection(a1, a7) == r_t{s2});
    CHECK(intersection(a3, a7) == r_t{a7});
  }

  {  // 3D
    static constexpr dim_t nd = 3;
    using a_t                 = geometry::aabb<nd>;
    using p_t                 = geometry::point<nd>;
    using s_t                 = geometry::segment<nd>;
    using q_t                 = geometry::quad<nd>;

    auto pM2 = p_t::constant(-2.);
    auto pM1 = p_t::constant(-1.);
    auto p0  = p_t::constant(0.);
    auto p1  = p_t::constant(1.);
    auto p2  = p_t::constant(2.);
    auto p3  = p_t::constant(3.);

    a_t a0(p0, p1);
    a_t a1(p1, p2);
    a_t a2(p2, p3);
    a_t a3(p0, p2);
    a_t a4(p1, p3);
    a_t a5(pM2, pM1);

    // check all faces: L, R, front, Back, top, bottom for a0
    a_t a6(p_t{-1., 0., 0.}, p_t{0., 1., 1.});  // L
    q_t q06{
     {{p_t{0., 0., 0.}, p_t{0., 0., 1.}, p_t{0., 1., 1.}, p_t{0., 1., 0.}}}};
    a_t a7(p_t{1., 0., 0.}, p_t{2., 1., 1.});  // R
    q_t q07{
     {{p_t{1., 0., 0.}, p_t{1., 0., 1.}, p_t{1., 1., 1.}, p_t{1., 1., 0.}}}};

    a_t a8(p_t{0., 0., -1.}, p_t{1., 1., 0.});  // F
    q_t q08{
     {{p_t{0., 0., 0.}, p_t{0., 1., 0.}, p_t{1., 1., 0.}, p_t{1., 0., 0.}}}};

    a_t a9(p_t{0., 0., 1.}, p_t{1., 1., 2.});  // Back
    q_t q09{
     {{p_t{0., 0., 1.}, p_t{0., 1., 1.}, p_t{1., 1., 1.}, p_t{1., 0., 1.}}}};

    a_t a10(p_t{0., 1., 0.}, p_t{1., 2., 1.});  // T
    q_t q010{
     {{p_t{0., 1., 0.}, p_t{1., 1., 0.}, p_t{1., 1., 1.}, p_t{0., 1., 1.}}}};

    a_t a11(p_t{0., -1., 0.}, p_t{1., 0., 1.});  // Bottom
    q_t q011{
     {{p_t{0., 0., 0.}, p_t{1., 0., 0.}, p_t{1., 0., 1.}, p_t{0., 0., 1.}}}};

    // check all corners for a0, front:
    a_t a12(p_t{-1., -1., -1.}, p_t{0., 0., 0.});  // BFL
    p_t p012{0., 0., 0.};
    a_t a13(p_t{1., -1., -1.}, p_t{2., 0., 0.});  // BFR
    p_t p013{1., 0., 0.};
    a_t a14(p_t{1., 1., -1.}, p_t{2., 2., 0.});  // TFR
    p_t p014{1., 1., 0.};
    a_t a15(p_t{-1., 1., -1.}, p_t{0., 2., 0.});  // TFL
    p_t p015{0., 1., 0.};
    // back:
    a_t a16(p_t{-1., -1., 1.}, p_t{0., 0., 2.});  // BBL
    p_t p016{0., 0., 1.};
    a_t a17(p_t{1., -1., 1.}, p_t{2., 0., 2.});  // BBR
    p_t p017{1., 0., 1.};
    a_t a18(p_t{1., 1., 1.}, p_t{2., 2., 2.});  // TBR
    p_t p018{1., 1., 1.};
    a_t a19(p_t{-1., 1., 1.}, p_t{0., 2., 2.});  // TBL
    p_t p019{0., 1., 1.};

    // check all edges
    // Mid:
    a_t a20(p_t{-1., -1., 0.}, p_t{0., 0., 1.});  // MBL
    s_t s020(p_t{0., 0., 0.}, p_t{0., 0., 1.});
    a_t a21(p_t{1., -1., 0.}, p_t{2., 0., 1.});  // MBR
    s_t s021(p_t{1., 0., 0.}, p_t{1., 0., 1.});
    a_t a22(p_t{1., 1., 0.}, p_t{2., 2., 1.});  // MTR
    s_t s022(p_t{1., 1., 0.}, p_t{1., 1., 1.});
    a_t a23(p_t{-1., 1., 0.}, p_t{0., 2., 1.});  // MTL
    s_t s023(p_t{0., 1., 0.}, p_t{0., 1., 1.});
    // Front:
    a_t a24(p_t{-1., 0., -1.}, p_t{0., 1., 0.});  // FL
    s_t s024(p_t{0., 0., 0.}, p_t{0., 1., 0.});
    a_t a25(p_t{1., 0., -1.}, p_t{2., 1., 0.});  // FR
    s_t s025(p_t{1., 0., 0.}, p_t{1., 1., 0.});
    a_t a26(p_t{0., -1., -1.}, p_t{1., 0., 0.});  // FB
    s_t s026(p_t{0., 0., 0.}, p_t{1., 0., 0.});
    a_t a27(p_t{0., 1., -1.}, p_t{1., 2., 0.});  // FT
    s_t s027(p_t{0., 1., 0.}, p_t{1., 1., 0.});
    // Back:
    a_t a28(p_t{-1., 0., 1.}, p_t{0., 1., 2.});  // BL
    s_t s028(p_t{0., 0., 1.}, p_t{0., 1., 1.});
    a_t a29(p_t{1., 0., 1.}, p_t{2., 1., 2.});  // BR
    s_t s029(p_t{1., 0., 1.}, p_t{1., 1., 1.});
    a_t a30(p_t{0., -1., 1.}, p_t{1., 0., 2.});  // BB
    s_t s030(p_t{0., 0., 1.}, p_t{1., 0., 1.});
    a_t a31(p_t{0., 1., 1.}, p_t{1., 2., 2.});  // BT
    s_t s031(p_t{0., 1., 1.}, p_t{1., 1., 1.});

    s_t s0(p0, p_t{0., 1., 1.});
    s_t s1(p_t{1., 0., 0.}, p1);
    s_t s2(p1, p_t{2., 1., 1.});

    using geometry::intersection;
    CHECK(intersection.test(a0, a1));
    CHECK(!intersection.test(a0, a2));
    CHECK(intersection.test(a0, a0));
    CHECK(intersection.test(a3, a4));
    CHECK(intersection.test(a0, a4));
    CHECK(!intersection.test(a0, a5));

    using r_t       = decltype(intersection(a0, a1));
    auto check_both = [&](auto&& a, auto&& b, auto&& r) {
      CHECK(intersection(a, b) == r_t{r});
      CHECK(intersection(b, a) == r_t{r});
    };

    check_both(a0, a1, p1);
    check_both(a0, a2, r_t{});
    check_both(a0, a0, a0);
    check_both(a3, a4, a1);
    check_both(a0, a4, p1);
    check_both(a0, a5, r_t{});

    // faces (6) of a0
    check_both(a0, a6, q06);    // L
    check_both(a0, a7, q07);    // R
    check_both(a0, a8, q08);    // F
    check_both(a0, a9, q09);    // B
    check_both(a0, a10, q010);  // T
    check_both(a0, a11, q011);  // B

    // corners (8)
    check_both(a0, a12, p012);  // BFL
    check_both(a0, a13, p013);  // BFR
    check_both(a0, a14, p014);  // TFR
    check_both(a0, a15, p015);  // TFL
    check_both(a0, a16, p016);  // BBL
    check_both(a0, a17, p017);  // BBR
    check_both(a0, a18, p018);  // TBR
    check_both(a0, a19, p019);  // TBL

    // edges (12)
    // mid (4):
    check_both(a0, a20, s020);  // MBL
    check_both(a0, a21, s021);  // MBR
    check_both(a0, a22, s022);  // MTR
    check_both(a0, a23, s023);  // MTL
    // front (4):
    check_both(a0, a24, s024);  // FL
    check_both(a0, a25, s025);  // FR
    check_both(a0, a26, s026);  // FB
    check_both(a0, a27, s027);  // FT
    // back (4):
    check_both(a0, a28, s028);  // FL
    check_both(a0, a29, s029);  // FR
    check_both(a0, a30, s030);  // FB
    check_both(a0, a31, s031);  // FT

    // CHECK(intersection(a0, a8)
    //       == r_t{q_t{{{p_t{0., 0., 0.}, p_t{0., 0., 1.}, p_t{0., 1., 1.},
    //                    p_t{0., 1., 0.}}}}});
    // CHECK(intersection(a0, a9)
    //       == r_t{q_t{{{p_t{1., 0., 0.}, p_t{1., 0., 1.}, p_t{1., 1., 1.},
    //                    p_t{1., 1., 0.}}}}});
    // CHECK(intersection(a0, a10)
    //       == r_t{q_t{{{p_t{1., 0., 0.}, p_t{1., 0., 1.}, p_t{1., 1., 1.},
    //                    p_t{1., 1., 0.}}}}});

    // CHECK(intersection(a0, a11)
    //       == r_t{q_t{{{p_t{1., 0., 0.}, p_t{1., 0., 1.}, p_t{1., 1., 1.},
    //                    p_t{1., 1., 0.}}}}});

    // CHECK(intersection(a1, a7) == r_t{s2});
    // CHECK(intersection(a3, a7) == r_t{a7});
  }

  return test::result();
}
