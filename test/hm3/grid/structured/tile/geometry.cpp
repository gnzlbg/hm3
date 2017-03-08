#include <hm3/grid/structured/tile/geometry.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

using tidx_t = grid::structured::tile::tidx_t;

template <dim_t Ad, tidx_t Nc>
using tile_geom = grid::structured::tile::tile_geometry<Ad, Nc>;

// Bounding box at the origin with length 1
template <dim_t Ad>
constexpr geometry::box<Ad> bbox_orig() {
  auto x_c = geometry::point<Ad>::constant(0.);
  auto l   = 1.0;
  return geometry::box<Ad>{x_c, l};
}

// Bounding box at x=(-3., -3, ...) ith length 2
template <dim_t Ad>
constexpr geometry::box<Ad> bbox_off() {
  auto x_c = geometry::point<Ad>::constant(-3.);
  auto l   = 2.0;
  return geometry::box<Ad>{x_c, l};
}

template <typename TileGeometry, dim_t Ad = TileGeometry::ambient_dimension()>
void test_orig(TileGeometry, tidx_t nc) {
  TileGeometry g = TileGeometry(bbox_orig<Ad>());
  using point_t  = geometry::point<Ad>;

  CHECK(g.cell_length() == (1.0 / nc));
  CHECK(g.cell_volume() == std::pow(1.0 / nc, Ad));
  CHECK(g.cell_surface_area() == std::pow(1.0 / nc, Ad - 1));
  CHECK(g.tile_length() == 1.0);
  CHECK(g.tile_centroid() == point_t::constant(0.0));
  CHECK(g.tile_bounding_box() == bbox_orig<Ad>());

  point_t p_inside  = point_t::constant(-0.49999999);
  point_t p_outside = point_t::constant(1.5);
  CHECK(g.contains(p_inside));
  CHECK(!g.contains(p_outside));
  {
    auto c_i = g.cell_containing(p_inside);
    CHECK(c_i);
    CHECK(c_i.idx() == 0);
  }
  {
    auto c_o = g.cell_containing(p_outside);
    CHECK(!c_o);
  }
}

template <typename TileGeometry, dim_t Ad = TileGeometry::ambient_dimension()>
void test_off5(TileGeometry) {
  TileGeometry g = TileGeometry(bbox_off<Ad>());
  using point_t  = geometry::point<Ad>;

  tidx_t nc = 5;
  CHECK(g.cell_length() == 0.4);

  CHECK(g.cell_volume() == std::pow(2.0 / nc, Ad));
  CHECK(g.cell_surface_area() == std::pow(2.0 / nc, Ad - 1));
  CHECK(g.tile_length() == 2.0);
  CHECK(g.tile_centroid() == point_t::constant(-3.0));
  CHECK(g.tile_bounding_box() == bbox_off<Ad>());

  point_t p_inside  = point_t::constant(-3.9999999);
  point_t p_outside = point_t::constant(1.5);
  CHECK(g.contains(p_inside));
  CHECK(!g.contains(p_outside));
  {
    auto c_i = g.cell_containing(p_inside);
    CHECK(c_i);
    CHECK(c_i.idx() == 0);
  }
  {
    auto c_o = g.cell_containing(p_outside);
    CHECK(!c_o);
  }
}

template <dim_t Ad>
void test_tile() {
  {  // orig, 1 cell
    using g_t = tile_geom<Ad, 1>;
    test_orig(g_t{}, 1);
  }
  {  // orig, 5 cell
    using g_t = tile_geom<Ad, 5>;
    test_orig(g_t{}, 5);
  }
  {
    // off, 5 cells
    using g_t = tile_geom<Ad, 5>;
    test_off5(g_t{});
  }
}

int main() {
  test_tile<1>();
  test_tile<2>();
  test_tile<3>();

  {  // 2d, 2x2 tile
    using t_g = tile_geom<2, 2>;
    using x_t = typename t_g::cell_coordinate;
    using p_t = geometry::point<2>;
    t_g g(bbox_orig<2>());

    std::vector<p_t> xs{
     {-0.25, -0.25}, {0.25, -0.25}, {-0.25, 0.25}, {0.25, 0.25}};

    // this works, but the above segfaults in release:
    // std::vector<p_t> xs(4);
    // {
    //   xs[0][0] = -0.25;
    //   xs[0][1] = -0.25;
    // }
    // {
    //   xs[1][0] = 0.25;
    //   xs[1][1] = -0.25;
    // }
    // {
    //   xs[2][0] = -0.25;
    //   xs[2][1] = 0.25;
    // }
    // {
    //   xs[3][0] = 0.25;
    //   xs[3][1] = 0.25;
    // }

    for (tidx_t i = 0; i < 4; ++i) {
      CHECK(g.cell_centroid(x_t::from(i)) == xs[i]);
    }
  }

  return test::result();
}
