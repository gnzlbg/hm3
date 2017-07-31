#include <hm3/solver/geometry/domain/discrete.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;
using solver::geometry::boundary_surface;
using solver::geometry::discrete_domain;
using namespace solver::geometry;

// template struct hm3::solver::geometry::discrete_domain<1>;
// template struct hm3::solver::geometry::discrete_domain<2>;
// template struct hm3::solver::geometry::discrete_domain<3>;

int main() {
  {  // 1D segment
    static constexpr dim_t ad = 1;

    using bs_t = boundary_surface<ad>;
    static_assert(geometry::VertexAccess<bs_t>{});
    static_assert(geometry::Vertex<bs_t>{});
    static_assert(geometry::Point<bs_t>{});
    static_assert(Simplex<bs_t>{});

    using p_t = geometry::point<ad>;

    p_t x0 = {0.};
    p_t x1 = {2.};

    bs_t ps[2] = {bs_t{x0, 1}, bs_t{x1, 0}};

    discrete_domain<ad> mesh(ps);

#ifdef HM3_ENABLE_VTK
    solver::geometry::vtk::serialize(mesh, "discrete_domain_1d");
#endif
  }

  {  // 2D pentagon
    static constexpr dim_t ad = 2;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using s_t = geometry::segment<ad>;
    using p_t = geometry::point<ad>;
    using b_t = geometry::box<ad>;

    p_t x0 = {0., 0.};
    p_t x1 = {2., 0.};
    p_t x2 = {2., 1.};
    p_t x3 = {1., 2.};
    p_t x4 = {0., 1.};

    s_t s1 = {x0, x1};
    s_t s2 = {x1, x2};
    s_t s3 = {x4, x3};
    s_t s4 = {x3, x2};
    s_t s5 = {x0, x4};

    bs_t ss[5]
     = {bs_t(s1, 1), bs_t(s2, 2), bs_t(s3, 3), bs_t(s4, 4), bs_t(s5, 5)};

    discrete_domain<ad> mesh(ss);

#ifdef HM3_ENABLE_VTK
    solver::geometry::vtk::serialize(mesh, "discrete_domain_2d");
#endif

    {  // test relative position of box inside the domain
      b_t b{p_t::constant(0.75), 0.5};
      CHECK(mesh.relative_position(b) == inside);
    }
  }

  {  // 2D multiple shapes
    static constexpr dim_t ad = 2;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using a_t = geometry::aabb<ad>;
    using p_t = geometry::point<ad>;

    a_t out(p_t{0., 0.}, p_t{6., 5.});
    a_t b1(p_t{1., 1.}, p_t{2., 2.});
    a_t b2(p_t{3., 3.}, p_t{4., 4.});
    a_t b3(p_t{4., 1.}, p_t{5., 2.});

    auto rou = geometry::edges(out)
               | view::transform([](auto e) { return bs_t(e, 0); });
    auto rb1
     = geometry::edges(b1) | view::transform([](auto e) { return bs_t(e, 1); });
    auto rb2
     = geometry::edges(b2) | view::transform([](auto e) { return bs_t(e, 2); });
    auto rb3
     = geometry::edges(b3) | view::transform([](auto e) { return bs_t(e, 3); });

    discrete_domain<ad> mesh(view::concat(rou, rb1, rb2, rb3));

    // outside points: outside the outer box
    p_t po0{-0.5, 0.5};
    p_t po1{-0.5, 1.5};
    p_t po2{-0.5, 2.5};
    p_t po3{2.5, -0.5};
    p_t po4{3.5, -0.5};
    p_t po5{6.5, 0.5};

    // outside points: in the inner boxes
    p_t po6{1.5, 1.5};
    p_t po7{4.5, 1.5};
    p_t po8{3.5, 3.5};

    // inside points: in the domain between outer and inner boxes
    p_t pi0{0.5, 0.5};
    p_t pi1{0.5, 1.5};
    p_t pi2{3.5, 1.5};
    p_t pi3{5.5, 1.5};
    p_t pi4{2.5, 3.5};
    p_t pi5{3.5, 4.5};

    // boundary points: on the domain boundary
    p_t pb0{0., 0.};
    p_t pb1{0., 0.5};
    p_t pb2{0., 1.};
    p_t pb3{0., 1.5};
    p_t pb4{0., 2.};
    p_t pb5{0., 2.5};
    p_t pb6{0., 3.};
    p_t pb7{0.5, 0.};
    p_t pb8{1., 0.};
    p_t pb9{1.5, 0.};
    p_t pb10{2., 0.};
    p_t pb11{3., 0.};
    p_t pb12{1., 2.};
    p_t pb13{2., 1.5};
    p_t pb14{2., 2.};
    p_t pb15{4., 1.};
    p_t pb16{4., 1.5};
    p_t pb17{4., 2.};

    CHECK(mesh.relative_position(po0) == outside);
    CHECK(mesh.relative_position(po1) == outside);
    CHECK(mesh.relative_position(po2) == outside);
    CHECK(mesh.relative_position(po3) == outside);
    CHECK(mesh.relative_position(po4) == outside);
    CHECK(mesh.relative_position(po5) == outside);

    CHECK(mesh.relative_position(po6) == outside);
    CHECK(mesh.relative_position(po7) == outside);
    CHECK(mesh.relative_position(po8) == outside);

    CHECK(mesh.relative_position(pi0) == inside);
    CHECK(mesh.relative_position(pi1) == inside);
    CHECK(mesh.relative_position(pi2) == inside);
    CHECK(mesh.relative_position(pi3) == inside);
    CHECK(mesh.relative_position(pi4) == inside);
    CHECK(mesh.relative_position(pi5) == inside);

    CHECK(mesh.relative_position(pb0) == intersected);
    CHECK(mesh.relative_position(pb1) == intersected);
    CHECK(mesh.relative_position(pb2) == intersected);
    CHECK(mesh.relative_position(pb3) == intersected);
    CHECK(mesh.relative_position(pb4) == intersected);
    CHECK(mesh.relative_position(pb5) == intersected);
    CHECK(mesh.relative_position(pb6) == intersected);
    CHECK(mesh.relative_position(pb7) == intersected);
    CHECK(mesh.relative_position(pb8) == intersected);
    CHECK(mesh.relative_position(pb9) == intersected);
    CHECK(mesh.relative_position(pb10) == intersected);
    CHECK(mesh.relative_position(pb11) == intersected);
    CHECK(mesh.relative_position(pb12) == intersected);
    CHECK(mesh.relative_position(pb13) == intersected);
    CHECK(mesh.relative_position(pb14) == intersected);
    CHECK(mesh.relative_position(pb15) == intersected);
    CHECK(mesh.relative_position(pb16) == intersected);
    CHECK(mesh.relative_position(pb17) == intersected);

    // Check intersected simplices of aabbs:
    {
      a_t a0(p_t{-0.5, -0.5}, p_t{0.5, 0.5});
      auto r = {sidx_t{0}, sidx_t{3}};
      test::check_equal(mesh.intersected_simplices(a0), r);
    }
    {
      a_t a0(p_t{-0.5, -0.5}, p_t{1.5, 1.5});
      auto r = {sidx_t{0}, sidx_t{3}, sidx_t{4}, sidx_t{7}};
      test::check_equal(mesh.intersected_simplices(a0), r);
    }

    // split and clip
    {
      using plg_t = geometry::polygon<ad>;

      a_t a(p_t{-0.5, -0.5}, p_t{0.5, 0.5});
      plg_t p(edges(a));
      auto r = mesh.split(p);
      ascii_fmt::out("r: {}\n", r);

      p_t p0{0.5, 0.};
      p_t p1{0.5, 0.5};
      p_t p2{0.0, 0.5};
      p_t p3{0.0, 0.0};

      auto pos_vxs = {p0, p1, p2, p3};

      p_t p4{0.0, 0.5};
      p_t p5{-0.5, 0.5};
      p_t p6{-0.5, -0.5};
      p_t p7{0.5, -0.5};
      p_t p8{0.5, 0.0};
      p_t p9{0.0, 0.0};

      auto neg_vxs = {p4, p5, p6, p7, p8, p9};

      plg_t pos(make_segment_range(pos_vxs));
      plg_t neg(make_segment_range(neg_vxs));

      CHECK(r.size() == 2_u);
      CHECK(r[0] == pos);
      CHECK(r[1] == neg);

      auto rpos = mesh.clip(p, inside);
      CHECK(rpos.size() == 1_u);

      auto rneg = mesh.clip(p, outside);
      CHECK(rneg.size() == 1_u);

      CHECK(rpos[0] == pos);
      CHECK(rneg[0] == neg);
    }

#ifdef HM3_ENABLE_VTK
    solver::geometry::vtk::serialize(mesh, "discrete_domain_2d_multiple");
#endif
  }

  {  // 2D - case A
    static constexpr dim_t ad = 2;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using p_t   = geometry::point<ad>;
    using plg_t = geometry::polygon<ad>;

    p_t p0{-2., -2.};
    p_t p1{1., -2.};
    p_t p2{1., 0.5};
    p_t p3{1., 2.};
    p_t p4{-2., 2.};

    auto vxs = {p0, p1, p2, p3, p4};

    plg_t plg(make_segment_range(vxs));

    discrete_domain<ad> mesh(geometry::edges(plg) | view::transform([](auto e) {
                               return bs_t{e, 1};
                             }));

    CHECK(mesh.relative_position(p_t{-2.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p_t{0.5, 0.5}) == inside);
    CHECK(mesh.relative_position(p_t{1.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p2) == intersected);
  }

  {  // 2D - case B
    static constexpr dim_t ad = 2;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using p_t   = geometry::point<ad>;
    using plg_t = geometry::polygon<ad>;

    p_t p0{-5., -5.};
    p_t p1{1., -5.};
    p_t p2{1., 0.5};
    p_t p3{2., 0.5};
    p_t p4{2., 5.};
    p_t p5{-5., 5.};

    auto vxs = {p0, p1, p2, p3, p4, p5};

    plg_t plg(make_segment_range(vxs));

    discrete_domain<ad> mesh(geometry::edges(plg) | view::transform([](auto e) {
                               return bs_t{e, 1};
                             }));

    CHECK(mesh.relative_position(p_t{-5.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p_t{0.5, 0.5}) == inside);
    CHECK(mesh.relative_position(p_t{2.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p2) == intersected);
    CHECK(mesh.relative_position(p3) == intersected);
  }

  {  // 2D - case C
    static constexpr dim_t ad = 2;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using p_t   = geometry::point<ad>;
    using plg_t = geometry::polygon<ad>;

    p_t p0{-8., -8.};
    p_t p1{1., -8.};
    p_t p2{2., 0.5};
    p_t p3{3., 0.5};
    p_t p4{4., -8.};
    p_t p5{5., -8.};
    p_t p6{5., 8.};
    p_t p7{-8., 8.};

    auto vxs = {p0, p1, p2, p3, p4, p5, p6, p7};

    plg_t plg(make_segment_range(vxs));

    discrete_domain<ad> mesh(geometry::edges(plg) | view::transform([](auto e) {
                               return bs_t{e, 1};
                             }));

    CHECK(mesh.relative_position(p_t{-8.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p_t{0.5, 0.5}) == inside);
    CHECK(mesh.relative_position(p_t{1.5, 0.5}) == inside);
    CHECK(mesh.relative_position(p_t{5.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p2) == intersected);
    CHECK(mesh.relative_position(p3) == intersected);
  }

  {  // 2D - case D
    static constexpr dim_t ad = 2;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using p_t   = geometry::point<ad>;
    using plg_t = geometry::polygon<ad>;

    p_t p0{-9., -9.};
    p_t p1{1., -9.};
    p_t p2{2., 0.5};
    p_t p3{3., 0.5};
    p_t p4{4., -9.};
    p_t p5{5., -9.};
    p_t p6{5., 0.5};
    p_t p7{6., 0.5};
    p_t p8{6., 9.};
    p_t p9{-9., 9.};

    auto vxs = {p0, p1, p2, p3, p4, p5, p6, p7, p8, p9};

    plg_t plg(make_segment_range(vxs));

    discrete_domain<ad> mesh(geometry::edges(plg) | view::transform([](auto e) {
                               return bs_t{e, 1};
                             }));

    CHECK(mesh.relative_position(p_t{-9.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p_t{0.5, 0.5}) == inside);
    CHECK(mesh.relative_position(p_t{6.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p_t{4.5, 0.5}) == inside);
    CHECK(mesh.relative_position(p2) == intersected);
    CHECK(mesh.relative_position(p3) == intersected);
    CHECK(mesh.relative_position(p6) == intersected);
    CHECK(mesh.relative_position(p7) == intersected);
  }

  {  // 2D - case E
    static constexpr dim_t ad = 2;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using p_t   = geometry::point<ad>;
    using plg_t = geometry::polygon<ad>;

    p_t p0{-9., -9.};
    p_t p1{1., -9.};
    p_t p2{2., 0.5};
    p_t p3{2.5, 0.5};
    p_t p4{3., 0.5};
    p_t p5{4., -9.};
    p_t p6{5., -9.};
    p_t p7{5., 0.5};
    p_t p8{6., 0.5};
    p_t p9{6., 9.};
    p_t p10{-9., 9.};

    auto vxs = {p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10};

    plg_t plg(make_segment_range(vxs));

    discrete_domain<ad> mesh(geometry::edges(plg) | view::transform([](auto e) {
                               return bs_t{e, 1};
                             }));

    CHECK(mesh.relative_position(p_t{-9.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p_t{0.5, 0.5}) == inside);
    CHECK(mesh.relative_position(p_t{6.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p_t{4.5, 0.5}) == inside);
    CHECK(mesh.relative_position(p2) == intersected);
    CHECK(mesh.relative_position(p3) == intersected);
    CHECK(mesh.relative_position(p6) == intersected);
    CHECK(mesh.relative_position(p7) == intersected);
  }

  {  // 2D - case F (triangle with point on ray positive direction)
    static constexpr dim_t ad = 2;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using p_t   = geometry::point<ad>;
    using plg_t = geometry::polygon<ad>;

    p_t p0{-1., -2.};
    p_t p1{1., 0.5};
    p_t p2{-1., 2.0};

    auto vxs = {p0, p1, p2};

    plg_t plg(make_segment_range(vxs));

    discrete_domain<ad> mesh(geometry::edges(plg) | view::transform([](auto e) {
                               return bs_t{e, 1};
                             }));

    CHECK(mesh.relative_position(p_t{-1.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p_t{0.75, 0.5}) == inside);
    CHECK(mesh.relative_position(p_t{1.5, 0.5}) == outside);
    CHECK(mesh.relative_position(p0) == intersected);
    CHECK(mesh.relative_position(p1) == intersected);
    CHECK(mesh.relative_position(p2) == intersected);
  }

  {  // 3D tet
    static constexpr dim_t ad = 3;

    using bs_t = boundary_surface<ad>;
    static_assert(Simplex<bs_t>{});

    using s_t = geometry::simplex<ad>;
    using p_t = geometry::point<ad>;

    p_t x0 = {0., 0., 0.};
    p_t x1 = {1., 0., 0.};
    p_t x2 = {0., 1., 0.};
    p_t x3 = {0., 0., 1.};

    auto vs0 = {x0, x1, x2};
    auto vs1 = {x3, x0, x1};
    auto vs2 = {x3, x0, x2};
    auto vs3 = {x1, x2, x3};

    s_t s0(make_segment_range(vs0));
    s_t s1(make_segment_range(vs1));
    s_t s2(make_segment_range(vs2));
    s_t s3(make_segment_range(vs3));

    bs_t ss[4] = {bs_t(s0, 1), bs_t(s1, 2), bs_t(s2, 3), bs_t(s3, 4)};

    discrete_domain<ad> mesh(ss);

#ifdef HM3_ENABLE_VTK
    solver::geometry::vtk::serialize(mesh, "discrete_domain_3d");
#endif
  }

  return test::result();
}
