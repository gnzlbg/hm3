#include <hm3/geometry/algorithm/split/polygon_polyline.hpp>
#include <hm3/geometry/io/vtk.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

void test_polygon_polyline_binary_split() {
  static constexpr dim_t nd = 2;
  using p_t                 = geometry::point<nd>;
  using pl_t                = geometry::small_polyline<nd, 4>;
  using pg_t                = geometry::small_polygon<nd, 8>;

  auto pg_p0 = p_t{0., 0.};
  auto pg_p1 = p_t{1., 0.};
  auto pg_p2 = p_t{1., 1.};
  auto pg_p3 = p_t{0., 1.};

  auto pg = pg_t({pg_p0, pg_p1, pg_p2, pg_p3});

  auto pl_p0 = p_t{1.0, 0.5};
  auto pl_p1 = p_t{0.5, 0.5};
  auto pl_p2 = p_t{0.5, 1.0};

  auto pl0 = pl_t({pl_p0, pl_p1, pl_p2});

  using geometry::polygon_primitive::split_detail::binary_split;

  auto r0 = binary_split(pg, pl0);

  auto r0_pg0
   = pg_t({p_t{0.5, 1.0}, pg_p3, pg_p0, pg_p1, p_t{1.0, 0.5}, p_t{0.5, 0.5}});
  auto r0_pg1 = pg_t({p_t{1.0, 0.5}, pg_p2, p_t{0.5, 1.0}, p_t{0.5, 0.5}});

  CHECK(r0[0] == r0_pg0);
  CHECK(r0[1] == r0_pg1);

#ifdef HM3_ENABLE_VTK
  {
    geometry::io::vtk<2> oi{};
    oi.push_cell(pg);
    oi.push_cell(pl0);
    oi.write("binary_split_input");

    geometry::io::vtk<2> oo{};
    oo.push_range(r0);
    oo.write("binary_split_output");
  }
#endif  // HM3_ENABLE_VTK
}

void test_polygon_polyline_split() {
  static constexpr dim_t nd = 2;
  using p_t                 = geometry::point<nd>;
  using pl_t                = geometry::small_polyline<nd, 4>;
  using pg_t                = geometry::small_polygon<nd, 8>;

  auto pg_p0 = p_t{0., 0.};
  auto pg_p1 = p_t{1., 0.};
  auto pg_p2 = p_t{1., 1.};
  auto pg_p3 = p_t{0., 1.};

  auto pg = pg_t({pg_p0, pg_p1, pg_p2, pg_p3});

  auto pl_p0 = p_t{1.5, 0.5};
  auto pl_p1 = p_t{0.5, 0.5};
  auto pl_p2 = p_t{0.5, 1.5};
  auto pl_p3 = p_t{-0.5, 1.5};
  auto pl_p4 = p_t{-0.5, 0.25};
  auto pl_p5 = p_t{0.25, 0.25};
  auto pl_p6 = p_t{0.25, -0.25};

  auto pl0 = pl_t({pl_p0, pl_p1, pl_p2, pl_p3, pl_p4, pl_p5, pl_p6});

  auto r0 = geometry::split(pg, pl0);
  CHECK(r0.size() == 3_u);

  auto r0_pg0 = pg_t({p_t{1.0, 0.5}, p_t{1., 1}, p_t{0.5, 1.0}, p_t{0.5, 0.5}});
  CHECK(r0[0] == r0_pg0);

  auto r0_pg1
   = pg_t({p_t{0.25, 0.}, p_t{1., 0.}, p_t{1.0, 0.5}, p_t{0.5, 0.5},
           p_t{0.5, 1.0}, p_t{0., 1.}, p_t{0., 0.25}, p_t{0.25, 0.25}});
  CHECK(r0[1] == r0_pg1);

  auto r0_pg2
   = pg_t({p_t{0.0, 0.25}, p_t{0., 0.}, p_t{0.25, 0.}, p_t{0.25, 0.25}});

  CHECK(r0[2] == r0_pg2);

#ifdef HM3_ENABLE_VTK
  {
    geometry::io::vtk<2> oi{};
    oi.push_cell(pg);
    oi.push_cell(pl0);
    oi.write("polygon_polyline_split_input0");

    geometry::io::vtk<2> oo{};
    oo.push_range(r0);
    oo.write("polygon_polyline_split_output0");
  }
#endif  // HM3_ENABLE_VTK

  auto pl1
   = pl_t({p_t{-0.25, 0.5}, p_t{0.25, -0.25}, p_t{0.35, 0.25}, p_t{0.5, -0.25},
           p_t{0.65, 0.25}, p_t{0.75, -0.25}, p_t{1.25, 0.5}, p_t{0.75, 1.25},
           p_t{0.25, 1.25}, p_t{-0.25, 0.5}});

  auto r1 = geometry::split(pg, pl1);
  CHECK(r1.size() == 7_u);

#ifdef HM3_ENABLE_VTK
  {
    geometry::io::vtk<2> oi{};
    oi.push_cell(pg);
    oi.push_cell(pl1);
    oi.write("polygon_polyline_split_input1");

    geometry::io::vtk<2> oo{};
    oo.push_range(r1);
    oo.write("polygon_polyline_split_output1");
  }
#endif  // HM3_ENABLE_VTK

  // fmt::print("{}\n", view::all(r1));

  auto r1_pg0
   = pg_t({p_t{0.0, 0.1250000000000001}, p_t{0., 0.}, p_t{825. / 9900., 0.}});
  CHECK(r1[0] == r1_pg0);

  auto r1_pg1 = pg_t({p_t{0.3, 0.0}, p_t{0.425, 0.}, p_t{.35, 0.25}});
  CHECK(r1[1] == r1_pg1);

  auto r1_pg2 = pg_t({p_t{0.575, 0.0}, p_t{0.7, 0.}, p_t{.65, 0.25}});
  CHECK(r1[2] == r1_pg2);

  auto r1_pg3 = pg_t({p_t{11. / 12., 0.0}, p_t{1.0, 0.}, p_t{1.0, 0.125}});
  CHECK(r1[3] == r1_pg3);

  auto r1_pg4 = pg_t({p_t{1., 0.875}, p_t{1.0, 1.0}, p_t{11. / 12., 1.}});
  CHECK(r1[4] == r1_pg4);

  num_t v_ = 0.08333333333333337;

  auto r1_pg5
   = pg_t({p_t{0., 0.875}, p_t{0.0, 0.125}, p_t{825. / 9900., 0.}, p_t{0.3, 0.},
           p_t{0.35, 0.25}, p_t{0.425, 0.}, p_t{0.575, 0.}, p_t{0.65, 0.25},
           p_t{0.7, 0.}, p_t{11. / 12., 0.}, p_t{1., 0.125}, p_t{1., 0.875},
           p_t{11. / 12., 1.}, p_t{v_, 1.}});
  CHECK(r1[5] == r1_pg5);

  auto r1_pg6 = pg_t({p_t{v_, 1.}, p_t{0.0, 1.0}, p_t{0., 0.875}});
  CHECK(r1[6] == r1_pg6);

  auto pl2
   = pl_t({p_t{0.5, -0.2}, p_t{-.25, 0.25}, p_t{-0.25, 0.5}, p_t{1.0, -0.25},
           p_t{1.25, 0.}, p_t{-0.25, 0.75}, p_t{0., 1.25}, p_t{1.25, 0.75}});

  auto r2 = geometry::split(pg, pl2);
  CHECK(r2.size() == 5_u);

#ifdef HM3_ENABLE_VTK
  {
    geometry::io::vtk<2> oi{};
    oi.push_cell(pg);
    oi.push_cell(pl2);
    oi.write("polygon_polyline_split_input2");

    geometry::io::vtk<2> oo{};
    oo.push_range(r2);
    oo.write("polygon_polyline_split_output2");
  }
#endif  // HM3_ENABLE_VTK

  auto pl3 = pl_t({p_t{0.0, 0.0}, p_t{1.0, 0.}, p_t{1., 1.}, p_t{0., 1.}});

  auto r3 = geometry::split(pg, pl3);
  CHECK(r3.size() == 1_u);
  CHECK(r3[0] == pg);

  {
    auto pg2 = pg_t({p_t{0., 0.2083333333333334}, p_t{0., 0.}, p_t{1., 0.},
                     p_t{1., 0.2083333333333333}, p_t{0.5, 0.25}});

    auto tpl = pl_t({p_t{-.1, .2}, p_t{.5, .25}, p_t{1.1, .2}});
    auto tr  = geometry::split(pg2, tpl);
    CHECK(tr.size() == 1_u);
    CHECK(tr[0] == pg2);
  }
  // ipg: [v0: (0, 0.2083333333333334), v1: (0, 0), v2: (1, 0), v3: (1,
  // 0.2083333333333333), v4: (0.5, 0.25)], ipl: [v0: (-0.1, 0.2), v1: (0.5,
  // 0.25), v2: (1.1, 0.2)]

  // fmt::print("{}\n", view::all(r2));
}

int main() {
  test_polygon_polyline_binary_split();
  test_polygon_polyline_split();

  return test::result();
}
