#include <hm3/grid/structured/tile/geometry.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <suint_t Nd, suint_t Nc>
using tile_geom = grid::structured::tile::geometry<Nd, Nc>;

// Bounding box at the origin with length 1
template <suint_t Nd> constexpr geometry::square<Nd> bbox_orig() {
  auto x_c = geometry::point<Nd>::constant(0.);
  auto l   = 1.0;
  return geometry::square<Nd>{x_c, l};
}

// Bounding box at x=(-3., -3, ...) ith length 2
template <suint_t Nd> constexpr geometry::square<Nd> bbox_off() {
  auto x_c = geometry::point<Nd>::constant(-3.);
  auto l   = 2.0;
  return geometry::square<Nd>{x_c, l};
}

template <typename TileGeometry, suint_t Nd = TileGeometry::dimension()>
void test_orig(TileGeometry, suint_t nc) {
  TileGeometry g = TileGeometry(bbox_orig<Nd>());
  using point_t  = geometry::point<Nd>;

  CHECK(g.cell_length() == (1.0 / nc));
  CHECK(g.cell_volume() == std::pow(1.0 / nc, Nd));
  CHECK(g.cell_surface_area() == std::pow(1.0 / nc, Nd - 1));
  CHECK(g.tile_length() == 1.0);
  CHECK(g.tile_center() == point_t::constant(0.0));
  CHECK(g.tile_bounding_box() == bbox_orig<Nd>());

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

template <typename TileGeometry, suint_t Nd = TileGeometry::dimension()>
void test_off5(TileGeometry) {
  TileGeometry g = TileGeometry(bbox_off<Nd>());
  using point_t  = geometry::point<Nd>;

  suint_t nc = 5;
  CHECK(g.cell_length() == 0.4);

  CHECK(g.cell_volume() == std::pow(2.0 / nc, Nd));
  CHECK(g.cell_surface_area() == std::pow(2.0 / nc, Nd - 1));
  CHECK(g.tile_length() == 2.0);
  CHECK(g.tile_center() == point_t::constant(-3.0));
  CHECK(g.tile_bounding_box() == bbox_off<Nd>());

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

template <suint_t Nd> void test_tile() {
  {  // orig, 1 cell
    using g_t = tile_geom<Nd, 1>;
    test_orig(g_t{}, 1);
  }
  {  // orig, 5 cell
    using g_t = tile_geom<Nd, 5>;
    test_orig(g_t{}, 5);
  }
  {
    // off, 5 cells
    using g_t = tile_geom<Nd, 5>;
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
    auto g    = t_g::from(bbox_orig<2>());

    std::vector<geometry::point<2>> xs;
    xs.push_back({-0.25, -0.25});
    xs.push_back({0.25, -0.25});
    xs.push_back({-0.25, 0.25});
    xs.push_back({0.25, 0.25});

    for (suint_t i = 0; i < 4; ++i) {
      CHECK(g.cell_center(x_t::from(i)) == xs[i]);
    }
  }

  return test::result();
}
