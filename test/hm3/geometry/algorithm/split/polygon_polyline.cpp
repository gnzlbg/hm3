#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/algorithm/split/polygon_polyline.hpp>
#include <hm3/geometry/io/vtk.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <typename ResultRng, typename ShouldRng>
void test_contains(ResultRng&& result, ShouldRng&& should) {
  CHECK(ranges::size(result) == ranges::size(should));
  if (ranges::size(result) != ranges::size(should)) { return; }

  for (suint_t i = 0; i < ranges::size(should); ++i) {
    auto it = ranges::find_if(
     result, [&](auto&& pg) { return approx(pg, ranges::at(should, i)); });
    bool found = it != ranges::end(result);
    if (not found) {
      ascii_fmt::err("ERROR: value {} not found in range {}\n\n",
                     ranges::at(should, i), result);
      continue;
    }
    CHECK(*it == ranges::at(should, i));
  }
}

// Binary split "splits" a polygon with a polyline that has exactly two
// intersection points with the polygon.
void test_polygon_polyline_binary_split() {
  static constexpr dim_t ad = 2;
  using p_t                 = point<ad>;
  using pl_t                = polyline<ad>;
  using pg_t                = polygon<ad>;

  auto p0 = p_t{0., 0.};
  auto p1 = p_t{1., 0.};
  auto p2 = p_t{1., 1.};
  auto p3 = p_t{0., 1.};
  auto p4 = p_t{0.25, 0.};
  auto p5 = p_t{0.5, 0.5};
  auto p6 = p_t{0.75, 0.0};
  auto p7 = p_t{0., 0.5};
  auto p8 = p_t{1., 0.5};

  auto pg0_vxs = {p0, p1, p2, p3};
  auto pl0_vxs = {p3, p1};
  auto pl1_vxs = {p4, p5, p6};
  auto pl2_vxs = {p4, p6};
  auto pl3_vxs = {p0, p4};
  auto pl4_vxs = {p4, p1};
  auto pl5_vxs = {p7, p8};
  auto pl6_vxs = {p8, p5, p7};

  auto pg0 = pg_t(make_segment_range(pg0_vxs));

  auto pl0 = pl_t(make_open_segment_range(pl0_vxs));
  auto pl1 = pl_t(make_open_segment_range(pl1_vxs));
  auto pl2 = pl_t(make_open_segment_range(pl2_vxs));
  auto pl3 = pl_t(make_open_segment_range(pl3_vxs));
  auto pl4 = pl_t(make_open_segment_range(pl4_vxs));
  auto pl5 = pl_t(make_open_segment_range(pl5_vxs));
  auto pl6 = pl_t(make_open_segment_range(pl6_vxs));

  {  // diagonal segment through corners:
    auto r     = binary_split_polygon_polyline(pg0, pl0);
    auto vxsr0 = {p1, p2, p3};
    auto r0    = pg_t(make_segment_range(vxsr0));
    auto vxsr1 = {p3, p0, p1};
    auto r1    = pg_t(make_segment_range(vxsr1));

    auto rs = {r0, r1};
    test_contains(r, rs);

    auto ri = binary_split_polygon_polyline(pg0, direction.invert(pl0));
    test_contains(ri, rs);
  }

  {  // triangular split
    auto r     = binary_split_polygon_polyline(pg0, pl1);
    auto vxsr0 = {p6, p1, p2, p3, p0, p4, p5};
    auto r1    = pg_t(make_segment_range(vxsr0));
    auto vxsr1 = {p4, p6, p5};
    auto r0    = pg_t(make_segment_range(vxsr1));
    auto rs    = {r0, r1};

    test_contains(r, rs);

    auto ri = binary_split_polygon_polyline(pg0, direction.invert(pl1));
    test_contains(ri, rs);
  }

  {  // polyline on polygon edge (middle)
    auto r     = binary_split_polygon_polyline(pg0, pl2);
    auto vxsr0 = {p6, p1, p2, p3, p0, p4};
    auto r0    = pg_t(make_segment_range(vxsr0));

    CHECK(r.size() == 1_u);
    CHECK(r[0] == r0);

    auto ri = binary_split_polygon_polyline(pg0, direction.invert(pl2));
    CHECK(ri.size() == 1_u);
    CHECK(ri[0] == r0);
  }

  {  // polyline on polygon edge (first-half)
    auto r     = binary_split_polygon_polyline(pg0, pl3);
    auto vxsr0 = {p4, p1, p2, p3, p0};
    auto r0    = pg_t(make_segment_range(vxsr0));

    auto rs = {r0};
    test_contains(r, rs);
    auto ri = binary_split_polygon_polyline(pg0, direction.invert(pl3));
    test_contains(ri, rs);
  }

  {  // polyline on polygon edge (last-half)
    auto r     = binary_split_polygon_polyline(pg0, pl4);
    auto vxsr0 = {p1, p2, p3, p0, p4};
    auto r0    = pg_t(make_segment_range(vxsr0));

    auto rs = {r0};
    test_contains(r, rs);
    auto ri = binary_split_polygon_polyline(pg0, direction.invert(pl4));
    test_contains(ri, rs);
  }

  {  // split on half
    auto r     = binary_split_polygon_polyline(pg0, pl5);
    auto vxsr0 = {p8, p2, p3, p7};
    auto r0    = pg_t(make_segment_range(vxsr0));
    auto vxsr1 = {p7, p0, p1, p8};
    auto r1    = pg_t(make_segment_range(vxsr1));

    auto rs = {r0, r1};
    test_contains(r, rs);
    auto ri = binary_split_polygon_polyline(pg0, direction.invert(pl5));
    test_contains(ri, rs);
  }

  {  // split on half
    auto r     = binary_split_polygon_polyline(pg0, pl6);
    auto vxsr0 = {p8, p2, p3, p7, p5};
    auto r0    = pg_t(make_segment_range(vxsr0));
    auto vxsr1 = {p7, p0, p1, p8, p5};
    auto r1    = pg_t(make_segment_range(vxsr1));

    auto rs = {r0, r1};
    test_contains(r, rs);
    auto ri = binary_split_polygon_polyline(pg0, direction.invert(pl6));
    test_contains(ri, rs);
  }
}

void test_polygon_polyline_split() {
  static constexpr dim_t ad = 2;
  using p_t                 = point<ad>;
  using pl_t                = polyline<ad>;
  using pg_t                = polygon<ad>;

  auto pg_p0  = p_t{0., 0.};
  auto pg_p1  = p_t{1., 0.};
  auto pg_p2  = p_t{1., 1.};
  auto pg_p3  = p_t{0., 1.};
  auto pg_vxs = {pg_p0, pg_p1, pg_p2, pg_p3};
  auto pg     = pg_t(make_segment_range(pg_vxs));

  auto pl_p0 = p_t{1.5, 0.5};
  auto pl_p1 = p_t{0.5, 0.5};
  auto pl_p2 = p_t{0.5, 1.5};
  auto pl_p3 = p_t{-0.5, 1.5};
  auto pl_p4 = p_t{-0.5, 0.25};
  auto pl_p5 = p_t{0.25, 0.25};
  auto pl_p6 = p_t{0.25, -0.25};

  auto pl0_vxs = {pl_p0, pl_p1, pl_p2, pl_p3, pl_p4, pl_p5, pl_p6};
  auto pl0     = pl_t(make_open_segment_range(pl0_vxs));
  {
    auto r0 = split(pg, pl0);

    auto r0_pg0_vxs = {p_t{1.0, 0.5}, p_t{1., 1}, p_t{0.5, 1.0}, p_t{0.5, 0.5}};
    auto r0_pg0     = pg_t(make_segment_range(r0_pg0_vxs));

    auto r0_pg1_vxs
     = {p_t{0.25, 0.}, p_t{1., 0.}, p_t{1.0, 0.5}, p_t{0.5, 0.5},
        p_t{0.5, 1.0}, p_t{0., 1.}, p_t{0., 0.25}, p_t{0.25, 0.25}};
    auto r0_pg1 = pg_t(make_segment_range(r0_pg1_vxs));

    auto r0_pg2_vxs
     = {p_t{0.0, 0.25}, p_t{0., 0.}, p_t{0.25, 0.}, p_t{0.25, 0.25}};
    auto r0_pg2 = pg_t(make_segment_range(r0_pg2_vxs));

    auto rs = {r0_pg0, r0_pg1, r0_pg2};

    test_contains(r0, rs);

#ifdef HM3_ENABLE_VTK
    {
      geometry::io::vtk<ad> oi{};
      oi.push_cell(pg);
      oi.push_cell(pl0);
      oi.write("polygon_polyline_split_input0");

      geometry::io::vtk<ad> oo{};
      oo.push_range(r0);
      oo.write("polygon_polyline_split_output0");
    }
#endif  // HM3_ENABLE_VTK
  }
  {
    auto pl1_vxs
     = {p_t{-0.25, 0.5}, p_t{0.25, -0.25}, p_t{0.35, 0.25}, p_t{0.5, -0.25},
        p_t{0.65, 0.25}, p_t{0.75, -0.25}, p_t{1.25, 0.5},  p_t{0.75, 1.25},
        p_t{0.25, 1.25}, p_t{-0.25, 0.5}};
    auto pl1 = pl_t(make_open_segment_range(pl1_vxs));

    auto r1 = split(pg, pl1);

#ifdef HM3_ENABLE_VTK
    {
      geometry::io::vtk<ad> oi{};
      oi.push_cell(pg);
      oi.push_cell(pl1);
      oi.write("polygon_polyline_split_input1");

      geometry::io::vtk<ad> oo{};
      oo.push_range(r1);
      oo.write("polygon_polyline_split_output1");
    }
#endif  // HM3_ENABLE_VTK

    auto r1_pg0_vxs
     = {p_t{0.0, 0.1250000000000001}, p_t{0., 0.}, p_t{825. / 9900., 0.}};
    auto r1_pg0 = pg_t(make_segment_range(r1_pg0_vxs));

    auto r1_pg1_vxs = {p_t{0.3, 0.0}, p_t{0.425, 0.}, p_t{.35, 0.25}};
    auto r1_pg1     = pg_t(make_segment_range(r1_pg1_vxs));

    auto r1_pg2_vxs = {p_t{0.575, 0.0}, p_t{0.7, 0.}, p_t{.65, 0.25}};
    auto r1_pg2     = pg_t(make_segment_range(r1_pg2_vxs));

    auto r1_pg3_vxs = {p_t{11. / 12., 0.0}, p_t{1.0, 0.}, p_t{1.0, 0.125}};
    auto r1_pg3     = pg_t(make_segment_range(r1_pg3_vxs));

    auto r1_pg4_vxs = {p_t{1., 0.875}, p_t{1.0, 1.0}, p_t{11. / 12., 1.}};
    auto r1_pg4     = pg_t(make_segment_range(r1_pg4_vxs));

    num_t v_ = 0.08333333333333337;

    auto r1_pg5_vxs
     = {p_t{825. / 9900., 0.}, p_t{0.3, 0.},       p_t{0.35, 0.25},
        p_t{0.425, 0.},        p_t{0.575, 0.},     p_t{0.65, 0.25},
        p_t{0.7, 0.},          p_t{11. / 12., 0.}, p_t{1., 0.125},
        p_t{1., 0.875},        p_t{11. / 12., 1.}, p_t{v_, 1.},
        p_t{0., 0.875},        p_t{0.0, 0.125}};
    auto r1_pg5 = pg_t(make_segment_range(r1_pg5_vxs));

    auto r1_pg6_vxs = {p_t{v_, 1.}, p_t{0.0, 1.0}, p_t{0., 0.875}};
    auto r1_pg6     = pg_t(make_segment_range(r1_pg6_vxs));

    auto rs = {r1_pg0, r1_pg1, r1_pg2, r1_pg3, r1_pg4, r1_pg5, r1_pg6};
    test_contains(r1, rs);
  }
  {
    auto pl2_vxs
     = {p_t{0.5, -0.2}, p_t{-.25, 0.25},  p_t{-0.25, 0.5}, p_t{1.0, -0.25},
        p_t{1.25, 0.},  p_t{-0.25, 0.75}, p_t{0., 1.25},   p_t{1.25, 0.75}};
    auto pl2 = pl_t(make_open_segment_range(pl2_vxs));

    auto r2 = split(pg, pl2);

#ifdef HM3_ENABLE_VTK
    {
      geometry::io::vtk<ad> oi{};
      oi.push_cell(pg);
      oi.push_cell(pl2);
      oi.write("polygon_polyline_split_input2");

      geometry::io::vtk<ad> oo{};
      oo.push_range(r2);
      oo.write("polygon_polyline_split_output2");
    }
#endif  // HM3_ENABLE_VTK
  }
  {
    auto pl3_vxs = {p_t{0.0, 0.0}, p_t{1.0, 0.}, p_t{1., 1.}, p_t{0., 1.}};
    auto pl3     = pl_t(make_open_segment_range(pl3_vxs));

    auto r3 = split(pg, pl3);
    auto rs = {pg};
    test_contains(r3, rs);
  }
  {
    auto pg2_vxs = {p_t{0., 0.2083333333333334}, p_t{0., 0.}, p_t{1., 0.},
                    p_t{1., 0.2083333333333333}, p_t{0.5, 0.25}};
    auto pg2     = pg_t(make_segment_range(pg2_vxs));

    auto tpl_vxs = {p_t{-.1, .2}, p_t{.5, .25}, p_t{1.1, .2}};
    auto tpl     = pl_t(make_open_segment_range(tpl_vxs));
    auto tr      = split(pg2, tpl);
    auto rs      = {pg2};
    test_contains(tr, rs);
  }
}

int main() {
  test_polygon_polyline_binary_split();
  test_polygon_polyline_split();

  return test::result();
}
