/// \file
///
/// Test halo tile indices
#include <hm3/grid/structured/halo_tile/cell/bounds.hpp>
#include <hm3/utility/test.hpp>
#include "../print_grid.hpp"

using namespace hm3;

int main() {
  /// Test lookup tables:
  using grid::structured::halo_tile::cell::halos;

  {  // 1D, 10Cells, 2 Halo layers, Rank 0 (across faces)
    using nidx     = tree::neighbor_idx_t<tree::face_neighbors<1>>;
    using bounds_t = grid::structured::halo_tile::cell::bounds<1, 10, 2>;
    constexpr bounds_t b{};
    {
      STATIC_CHECK(b.size() == 14_su);
      STATIC_CHECK(b.length() == 14_su);
      STATIC_CHECK(b.halo_layers() == 2_su);
      STATIC_CHECK(b.internal_cell_length() == 10_su);
      STATIC_CHECK(b.internal_cell_size() == 10_su);
      STATIC_CHECK(b.halo_cell_size() == 4_su);
      STATIC_CHECK(b.first_halo_left() == 0_su);
      STATIC_CHECK(b.last_halo_left() == 1_su);
      STATIC_CHECK(b.first_internal() == 2_su);
      STATIC_CHECK(b.last_internal() == 11_su);
      STATIC_CHECK(b.first_halo_right() == 12_su);
      STATIC_CHECK(b.last_halo_right() == 13_su);
    }

    constexpr auto h0 = halos(nidx{0}, b);
    constexpr auto h1 = halos(nidx{1}, b);

    STATIC_CHECK(h0.from[0] == 0_su);
    STATIC_CHECK(h0.to[0] == 1_su);
    STATIC_CHECK(h1.from[0] == 12_su);
    STATIC_CHECK(h1.to[0] == 13_su);
  }  // 1D

  {  // 2D, 10Cells, 2 Halo layers, Rank 0
    using bounds_t = grid::structured::halo_tile::cell::bounds<2, 10, 2>;
    constexpr bounds_t b{};
    {
      STATIC_CHECK(b.size() == 196_su);
      STATIC_CHECK(b.length() == 14_su);
      STATIC_CHECK(b.halo_layers() == 2_su);
      STATIC_CHECK(b.internal_cell_length() == 10_su);
      STATIC_CHECK(b.internal_cell_size() == 100_su);
      STATIC_CHECK(b.halo_cell_size() == 96_su);
      STATIC_CHECK(b.first_halo_left() == 0_su);
      STATIC_CHECK(b.last_halo_left() == 1_su);
      STATIC_CHECK(b.first_internal() == 2_su);
      STATIC_CHECK(b.last_internal() == 11_su);
      STATIC_CHECK(b.first_halo_right() == 12_su);
      STATIC_CHECK(b.last_halo_right() == 13_su);
    }

    {  // face neighbors:
      using nidx        = tree::neighbor_idx_t<tree::face_neighbors<2>>;
      constexpr auto h0 = halos(nidx{0}, b);
      constexpr auto h1 = halos(nidx{1}, b);
      constexpr auto h2 = halos(nidx{2}, b);
      constexpr auto h3 = halos(nidx{3}, b);
      // face neighbor 0:
      STATIC_CHECK(h0.from[0] == 0_su);
      STATIC_CHECK(h0.from[1] == 2_su);
      STATIC_CHECK(h0.to[0] == 1_su);
      STATIC_CHECK(h0.to[1] == 11_su);

      // face neighbor 1:
      STATIC_CHECK(h1.from[0] == 12_su);
      STATIC_CHECK(h1.from[1] == 2_su);
      STATIC_CHECK(h1.to[0] == 13_su);
      STATIC_CHECK(h1.to[1] == 11_su);

      // face neighbor 2:
      STATIC_CHECK(h2.from[0] == 2_su);
      STATIC_CHECK(h2.from[1] == 0_su);
      STATIC_CHECK(h2.to[0] == 11_su);
      STATIC_CHECK(h2.to[1] == 1_su);

      // face neighbor 3:
      STATIC_CHECK(h3.from[0] == 2_su);
      STATIC_CHECK(h3.from[1] == 12_su);
      STATIC_CHECK(h3.to[0] == 11_su);
      STATIC_CHECK(h3.to[1] == 13_su);
    }
    {  // edge neighbors
      using nidx        = tree::neighbor_idx_t<tree::edge_neighbors<2>>;
      constexpr auto h0 = halos(nidx{0}, b);
      constexpr auto h1 = halos(nidx{1}, b);
      constexpr auto h2 = halos(nidx{2}, b);
      constexpr auto h3 = halos(nidx{3}, b);

      // edge neighbor 0:
      STATIC_CHECK(h0.from[0] == 0_su);
      STATIC_CHECK(h0.from[1] == 0_su);
      STATIC_CHECK(h0.to[0] == 1_su);
      STATIC_CHECK(h0.to[1] == 1_su);

      // edge neighbor 1:
      STATIC_CHECK(h1.from[0] == 12_su);
      STATIC_CHECK(h1.from[1] == 0_su);
      STATIC_CHECK(h1.to[0] == 13_su);
      STATIC_CHECK(h1.to[1] == 1_su);

      // edge neighbor 2:
      STATIC_CHECK(h2.from[0] == 0_su);
      STATIC_CHECK(h2.from[1] == 12_su);
      STATIC_CHECK(h2.to[0] == 1_su);
      STATIC_CHECK(h2.to[1] == 13_su);

      // edge neighbor 3:
      STATIC_CHECK(h3.from[0] == 12_su);
      STATIC_CHECK(h3.from[1] == 12_su);
      STATIC_CHECK(h3.to[0] == 13_su);
      STATIC_CHECK(h3.to[1] == 13_su);
    }
  }  // 2D

  {  // 3D, 10Cells, 2 Halo layers, Rank 0
    using bounds_t = grid::structured::halo_tile::cell::bounds<3, 10, 2>;
    constexpr bounds_t b{};
    {
      STATIC_CHECK(b.size() == 2744_su);
      STATIC_CHECK(b.length() == 14_su);
      STATIC_CHECK(b.halo_layers() == 2_su);
      STATIC_CHECK(b.internal_cell_length() == 10_su);
      STATIC_CHECK(b.internal_cell_size() == 1000_su);
      STATIC_CHECK(b.halo_cell_size() == 1744_su);
      STATIC_CHECK(b.first_halo_left() == 0_su);
      STATIC_CHECK(b.last_halo_left() == 1_su);
      STATIC_CHECK(b.first_internal() == 2_su);
      STATIC_CHECK(b.last_internal() == 11_su);
      STATIC_CHECK(b.first_halo_right() == 12_su);
      STATIC_CHECK(b.last_halo_right() == 13_su);
    }

    {  // face neighbors:
      using nidx        = tree::neighbor_idx_t<tree::face_neighbors<3>>;
      constexpr auto h0 = halos(nidx{0}, b);
      constexpr auto h1 = halos(nidx{1}, b);
      constexpr auto h2 = halos(nidx{2}, b);
      constexpr auto h3 = halos(nidx{3}, b);
      constexpr auto h4 = halos(nidx{4}, b);
      constexpr auto h5 = halos(nidx{5}, b);
      // face neighbor 0:
      STATIC_CHECK(h0.from[0] == 0_su);
      STATIC_CHECK(h0.from[1] == 2_su);
      STATIC_CHECK(h0.from[2] == 2_su);
      STATIC_CHECK(h0.to[0] == 1_su);
      STATIC_CHECK(h0.to[1] == 11_su);
      STATIC_CHECK(h0.to[2] == 11_su);
      // face neighbor 1:
      STATIC_CHECK(h1.from[0] == 12_su);
      STATIC_CHECK(h1.from[1] == 2_su);
      STATIC_CHECK(h1.from[2] == 2_su);
      STATIC_CHECK(h1.to[0] == 13_su);
      STATIC_CHECK(h1.to[1] == 11_su);
      STATIC_CHECK(h1.to[2] == 11_su);
      // face neighbor 2:
      STATIC_CHECK(h2.from[0] == 2_su);
      STATIC_CHECK(h2.from[1] == 0_su);
      STATIC_CHECK(h2.from[2] == 2_su);
      STATIC_CHECK(h2.to[0] == 11_su);
      STATIC_CHECK(h2.to[1] == 1_su);
      STATIC_CHECK(h2.to[2] == 11_su);
      // face neighbor 3:
      STATIC_CHECK(h3.from[0] == 2_su);
      STATIC_CHECK(h3.from[1] == 12_su);
      STATIC_CHECK(h3.from[2] == 2_su);
      STATIC_CHECK(h3.to[0] == 11_su);
      STATIC_CHECK(h3.to[1] == 13_su);
      STATIC_CHECK(h3.to[2] == 11_su);
      // face neighbor 4:
      STATIC_CHECK(h4.from[0] == 2_su);
      STATIC_CHECK(h4.from[1] == 2_su);
      STATIC_CHECK(h4.from[2] == 0_su);
      STATIC_CHECK(h4.to[0] == 11_su);
      STATIC_CHECK(h4.to[1] == 11_su);
      STATIC_CHECK(h4.to[2] == 1_su);
      // face neighbor 5:
      STATIC_CHECK(h5.from[0] == 2_su);
      STATIC_CHECK(h5.from[1] == 2_su);
      STATIC_CHECK(h5.from[2] == 12_su);
      STATIC_CHECK(h5.to[0] == 11_su);
      STATIC_CHECK(h5.to[1] == 11_su);
      STATIC_CHECK(h5.to[2] == 13_su);
    }
    {  // edge neighbors:
      using nidx         = tree::neighbor_idx_t<tree::edge_neighbors<3>>;
      constexpr auto h0  = halos(nidx{0}, b);
      constexpr auto h1  = halos(nidx{1}, b);
      constexpr auto h2  = halos(nidx{2}, b);
      constexpr auto h3  = halos(nidx{3}, b);
      constexpr auto h4  = halos(nidx{4}, b);
      constexpr auto h5  = halos(nidx{5}, b);
      constexpr auto h6  = halos(nidx{6}, b);
      constexpr auto h7  = halos(nidx{7}, b);
      constexpr auto h8  = halos(nidx{8}, b);
      constexpr auto h9  = halos(nidx{9}, b);
      constexpr auto h10 = halos(nidx{10}, b);
      constexpr auto h11 = halos(nidx{11}, b);

      // edge neighbor 0:
      STATIC_CHECK(h0.from[0] == 0_su);
      STATIC_CHECK(h0.from[1] == 0_su);
      STATIC_CHECK(h0.from[2] == 2_su);
      STATIC_CHECK(h0.to[0] == 1_su);
      STATIC_CHECK(h0.to[1] == 1_su);
      STATIC_CHECK(h0.to[2] == 11_su);
      // edge neighbor 1:
      STATIC_CHECK(h1.from[0] == 12_su);
      STATIC_CHECK(h1.from[1] == 0_su);
      STATIC_CHECK(h1.from[2] == 2_su);
      STATIC_CHECK(h1.to[0] == 13_su);
      STATIC_CHECK(h1.to[1] == 1_su);
      STATIC_CHECK(h1.to[2] == 11_su);
      // edge neighbor 2:
      STATIC_CHECK(h2.from[0] == 0_su);
      STATIC_CHECK(h2.from[1] == 12_su);
      STATIC_CHECK(h2.from[2] == 2_su);
      STATIC_CHECK(h2.to[0] == 1_su);
      STATIC_CHECK(h2.to[1] == 13_su);
      STATIC_CHECK(h2.to[2] == 11_su);
      // edge neighbor 3:
      STATIC_CHECK(h3.from[0] == 12_su);
      STATIC_CHECK(h3.from[1] == 12_su);
      STATIC_CHECK(h3.from[2] == 2_su);
      STATIC_CHECK(h3.to[0] == 13_su);
      STATIC_CHECK(h3.to[1] == 13_su);
      STATIC_CHECK(h3.to[2] == 11_su);
      // edge neighbor 4:
      STATIC_CHECK(h4.from[0] == 0_su);
      STATIC_CHECK(h4.from[1] == 2_su);
      STATIC_CHECK(h4.from[2] == 0_su);
      STATIC_CHECK(h4.to[0] == 1_su);
      STATIC_CHECK(h4.to[1] == 11_su);
      STATIC_CHECK(h4.to[2] == 1_su);
      // edge neighbor 5:
      STATIC_CHECK(h5.from[0] == 12_su);
      STATIC_CHECK(h5.from[1] == 2_su);
      STATIC_CHECK(h5.from[2] == 0_su);
      STATIC_CHECK(h5.to[0] == 13_su);
      STATIC_CHECK(h5.to[1] == 11_su);
      STATIC_CHECK(h5.to[2] == 1_su);
      // edge neighbor 6:
      STATIC_CHECK(h6.from[0] == 2_su);
      STATIC_CHECK(h6.from[1] == 0_su);
      STATIC_CHECK(h6.from[2] == 0_su);
      STATIC_CHECK(h6.to[0] == 11_su);
      STATIC_CHECK(h6.to[1] == 1_su);
      STATIC_CHECK(h6.to[2] == 1_su);
      // edge neighbor 7:
      STATIC_CHECK(h7.from[0] == 2_su);
      STATIC_CHECK(h7.from[1] == 12_su);
      STATIC_CHECK(h7.from[2] == 0_su);
      STATIC_CHECK(h7.to[0] == 11_su);
      STATIC_CHECK(h7.to[1] == 13_su);
      STATIC_CHECK(h7.to[2] == 1_su);
      // edge neighbor 8:
      STATIC_CHECK(h8.from[0] == 0_su);
      STATIC_CHECK(h8.from[1] == 2_su);
      STATIC_CHECK(h8.from[2] == 12_su);
      STATIC_CHECK(h8.to[0] == 1_su);
      STATIC_CHECK(h8.to[1] == 11_su);
      STATIC_CHECK(h8.to[2] == 13_su);
      // edge neighbor 9:
      STATIC_CHECK(h9.from[0] == 12_su);
      STATIC_CHECK(h9.from[1] == 2_su);
      STATIC_CHECK(h9.from[2] == 12_su);
      STATIC_CHECK(h9.to[0] == 13_su);
      STATIC_CHECK(h9.to[1] == 11_su);
      STATIC_CHECK(h9.to[2] == 13_su);
      // edge neighbor 10:
      STATIC_CHECK(h10.from[0] == 2_su);
      STATIC_CHECK(h10.from[1] == 0_su);
      STATIC_CHECK(h10.from[2] == 12_su);
      STATIC_CHECK(h10.to[0] == 11_su);
      STATIC_CHECK(h10.to[1] == 1_su);
      STATIC_CHECK(h10.to[2] == 13_su);
      // edge neighbor 11:
      STATIC_CHECK(h11.from[0] == 2_su);
      STATIC_CHECK(h11.from[1] == 12_su);
      STATIC_CHECK(h11.from[2] == 12_su);
      STATIC_CHECK(h11.to[0] == 11_su);
      STATIC_CHECK(h11.to[1] == 13_su);
      STATIC_CHECK(h11.to[2] == 13_su);
    }
    {  // corner neighbors:
      using nidx        = tree::neighbor_idx_t<tree::corner_neighbors<3>>;
      constexpr auto h0 = halos(nidx{0}, b);
      constexpr auto h1 = halos(nidx{1}, b);
      constexpr auto h2 = halos(nidx{2}, b);
      constexpr auto h3 = halos(nidx{3}, b);
      constexpr auto h4 = halos(nidx{4}, b);
      constexpr auto h5 = halos(nidx{5}, b);
      constexpr auto h6 = halos(nidx{6}, b);
      constexpr auto h7 = halos(nidx{7}, b);

      // corner neighbor 0:
      STATIC_CHECK(h0.from[0] == 0_su);
      STATIC_CHECK(h0.from[1] == 0_su);
      STATIC_CHECK(h0.from[2] == 0_su);
      STATIC_CHECK(h0.to[0] == 1_su);
      STATIC_CHECK(h0.to[1] == 1_su);
      STATIC_CHECK(h0.to[2] == 1_su);
      // corner neighbor 1:
      STATIC_CHECK(h1.from[0] == 12_su);
      STATIC_CHECK(h1.from[1] == 0_su);
      STATIC_CHECK(h1.from[2] == 0_su);
      STATIC_CHECK(h1.to[0] == 13_su);
      STATIC_CHECK(h1.to[1] == 1_su);
      STATIC_CHECK(h1.to[2] == 1_su);
      // corner neighbor 2:
      STATIC_CHECK(h2.from[0] == 0_su);
      STATIC_CHECK(h2.from[1] == 12_su);
      STATIC_CHECK(h2.from[2] == 0_su);
      STATIC_CHECK(h2.to[0] == 1_su);
      STATIC_CHECK(h2.to[1] == 13_su);
      STATIC_CHECK(h2.to[2] == 1_su);
      // corner neighbor 3:
      STATIC_CHECK(h3.from[0] == 12_su);
      STATIC_CHECK(h3.from[1] == 12_su);
      STATIC_CHECK(h3.from[2] == 0_su);
      STATIC_CHECK(h3.to[0] == 13_su);
      STATIC_CHECK(h3.to[1] == 13_su);
      STATIC_CHECK(h3.to[2] == 1_su);
      // corner neighbor 4:
      STATIC_CHECK(h4.from[0] == 0_su);
      STATIC_CHECK(h4.from[1] == 0_su);
      STATIC_CHECK(h4.from[2] == 12_su);
      STATIC_CHECK(h4.to[0] == 1_su);
      STATIC_CHECK(h4.to[1] == 1_su);
      STATIC_CHECK(h4.to[2] == 13_su);
      // corner neighbor 5:
      STATIC_CHECK(h5.from[0] == 12_su);
      STATIC_CHECK(h5.from[1] == 0_su);
      STATIC_CHECK(h5.from[2] == 12_su);
      STATIC_CHECK(h5.to[0] == 13_su);
      STATIC_CHECK(h5.to[1] == 1_su);
      STATIC_CHECK(h5.to[2] == 13_su);
      // corner neighbor 6:
      STATIC_CHECK(h6.from[0] == 0_su);
      STATIC_CHECK(h6.from[1] == 12_su);
      STATIC_CHECK(h6.from[2] == 12_su);
      STATIC_CHECK(h6.to[0] == 1_su);
      STATIC_CHECK(h6.to[1] == 13_su);
      STATIC_CHECK(h6.to[2] == 13_su);
      // corner neighbor 7:
      STATIC_CHECK(h7.from[0] == 12_su);
      STATIC_CHECK(h7.from[1] == 12_su);
      STATIC_CHECK(h7.from[2] == 12_su);
      STATIC_CHECK(h7.to[0] == 13_su);
      STATIC_CHECK(h7.to[1] == 13_su);
      STATIC_CHECK(h7.to[2] == 13_su);
    }
  }  // 3D
  return hm3::test::result();
}
