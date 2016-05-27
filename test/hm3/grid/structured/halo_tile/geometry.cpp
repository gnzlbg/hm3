#include <hm3/grid/structured/halo_tile/geometry.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  constexpr dim_t nd = 2;

  using tile_geometry = grid::structured::halo_tile::tile_geometry<nd, 2, 2>;
  using tidx_t        = grid::structured::halo_tile::tidx_t;

  using p_t = geometry::point<nd>;

  auto tile_centroid        = p_t::constant(0.);
  auto tile_internal_length = 1.0;
  auto tile_internal_bbox
   = geometry::square<nd>{tile_centroid, tile_internal_length};
  auto tile_external_length = 3.0;
  auto tile_external_bbox
   = geometry::square<nd>{tile_centroid, tile_external_length};

  tile_geometry g(tile_internal_bbox);
  CHECK(g.cell_length() == 0.5);
  CHECK(g.cell_surface_area() == (0.5));
  CHECK(g.cell_volume() == (0.5 * 0.5));
  CHECK(g.tile_centroid() == p_t::constant(0.));
  CHECK(g.tile_external_length() == tile_external_length);
  CHECK(g.tile_internal_length() == tile_internal_length);
  CHECK(g.tile_external_bounding_box() == tile_external_bbox);
  CHECK(g.tile_internal_bounding_box() == tile_internal_bbox);

  p_t x_i = p_t::constant(-.25);   // internal
  p_t x_h = p_t::constant(-.75);   // halo
  p_t x_o = p_t::constant(-1.75);  // outside of the tile

  CHECK(g.contains(x_i));
  CHECK(g.contains(x_h));
  CHECK(!g.contains(x_o));

  tidx_t idx_i = 14;
  tidx_t idx_h = 7;

  CHECK(g.cell_containing(x_i).idx() == idx_i);
  CHECK(g.cell_containing(x_h).idx() == idx_h);
  CHECK(!g.cell_containing(x_o));

  CHECK(g.internal_cell_containing(x_i).idx() == idx_i);
  CHECK(!g.internal_cell_containing(x_h));
  CHECK(!g.internal_cell_containing(x_o));

  CHECK(!g.halo_cell_containing(x_i));
  CHECK(g.halo_cell_containing(x_h).idx() == idx_h);
  CHECK(!g.halo_cell_containing(x_o));

  return test::result();
}
