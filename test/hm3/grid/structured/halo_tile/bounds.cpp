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
      static_assert(b.size() == 14, "");
      static_assert(b.length() == 14, "");
      static_assert(b.halo_layers() == 2, "");
      static_assert(b.internal_cell_length() == 10, "");
      static_assert(b.internal_cell_size() == 10, "");
      static_assert(b.halo_cell_size() == 4, "");
      static_assert(b.first_halo_left() == 0, "");
      static_assert(b.last_halo_left() == 1, "");
      static_assert(b.first_internal() == 2, "");
      static_assert(b.last_internal() == 11, "");
      static_assert(b.first_halo_right() == 12, "");
      static_assert(b.last_halo_right() == 13, "");
    }

    constexpr auto h0 = halos(nidx{0}, b);
    constexpr auto h1 = halos(nidx{1}, b);

    static_assert(h0.from[0] == 0, "");
    static_assert(h0.to[0] == 1, "");
    static_assert(h1.from[0] == 12, "");
    static_assert(h1.to[0] == 13, "");
  }  // 1D

  {  // 2D, 10Cells, 2 Halo layers, Rank 0
    using bounds_t = grid::structured::halo_tile::cell::bounds<2, 10, 2>;
    constexpr bounds_t b{};
    {
      static_assert(b.size() == 196, "");
      static_assert(b.length() == 14, "");
      static_assert(b.halo_layers() == 2, "");
      static_assert(b.internal_cell_length() == 10, "");
      static_assert(b.internal_cell_size() == 100, "");
      static_assert(b.halo_cell_size() == 96, "");
      static_assert(b.first_halo_left() == 0, "");
      static_assert(b.last_halo_left() == 1, "");
      static_assert(b.first_internal() == 2, "");
      static_assert(b.last_internal() == 11, "");
      static_assert(b.first_halo_right() == 12, "");
      static_assert(b.last_halo_right() == 13, "");
    }

    {  // face neighbors:
      using nidx        = tree::neighbor_idx_t<tree::face_neighbors<2>>;
      constexpr auto h0 = halos(nidx{0}, b);
      constexpr auto h1 = halos(nidx{1}, b);
      constexpr auto h2 = halos(nidx{2}, b);
      constexpr auto h3 = halos(nidx{3}, b);
      // face neighbor 0:
      static_assert(h0.from[0] == 0, "");
      static_assert(h0.from[1] == 2, "");
      static_assert(h0.to[0] == 1, "");
      static_assert(h0.to[1] == 11, "");

      // face neighbor 1:
      static_assert(h1.from[0] == 12, "");
      static_assert(h1.from[1] == 2, "");
      static_assert(h1.to[0] == 13, "");
      static_assert(h1.to[1] == 11, "");

      // face neighbor 2:
      static_assert(h2.from[0] == 2, "");
      static_assert(h2.from[1] == 0, "");
      static_assert(h2.to[0] == 11, "");
      static_assert(h2.to[1] == 1, "");

      // face neighbor 3:
      static_assert(h3.from[0] == 2, "");
      static_assert(h3.from[1] == 12, "");
      static_assert(h3.to[0] == 11, "");
      static_assert(h3.to[1] == 13, "");
    }
    {  // edge neighbors
      using nidx        = tree::neighbor_idx_t<tree::edge_neighbors<2>>;
      constexpr auto h0 = halos(nidx{0}, b);
      constexpr auto h1 = halos(nidx{1}, b);
      constexpr auto h2 = halos(nidx{2}, b);
      constexpr auto h3 = halos(nidx{3}, b);

      // edge neighbor 0:
      static_assert(h0.from[0] == 0, "");
      static_assert(h0.from[1] == 0, "");
      static_assert(h0.to[0] == 1, "");
      static_assert(h0.to[1] == 1, "");

      // edge neighbor 1:
      static_assert(h1.from[0] == 12, "");
      static_assert(h1.from[1] == 0, "");
      static_assert(h1.to[0] == 13, "");
      static_assert(h1.to[1] == 1, "");

      // edge neighbor 2:
      static_assert(h2.from[0] == 0, "");
      static_assert(h2.from[1] == 12, "");
      static_assert(h2.to[0] == 1, "");
      static_assert(h2.to[1] == 13, "");

      // edge neighbor 3:
      static_assert(h3.from[0] == 12, "");
      static_assert(h3.from[1] == 12, "");
      static_assert(h3.to[0] == 13, "");
      static_assert(h3.to[1] == 13, "");
    }
  }  // 2D

  {  // 3D, 10Cells, 2 Halo layers, Rank 0
    using bounds_t = grid::structured::halo_tile::cell::bounds<3, 10, 2>;
    constexpr bounds_t b{};
    {
      static_assert(b.size() == 2744, "");
      static_assert(b.length() == 14, "");
      static_assert(b.halo_layers() == 2, "");
      static_assert(b.internal_cell_length() == 10, "");
      static_assert(b.internal_cell_size() == 1000, "");
      static_assert(b.halo_cell_size() == 1744, "");
      static_assert(b.first_halo_left() == 0, "");
      static_assert(b.last_halo_left() == 1, "");
      static_assert(b.first_internal() == 2, "");
      static_assert(b.last_internal() == 11, "");
      static_assert(b.first_halo_right() == 12, "");
      static_assert(b.last_halo_right() == 13, "");
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
      static_assert(h0.from[0] == 0, "");
      static_assert(h0.from[1] == 2, "");
      static_assert(h0.from[2] == 2, "");
      static_assert(h0.to[0] == 1, "");
      static_assert(h0.to[1] == 11, "");
      static_assert(h0.to[2] == 11, "");
      // face neighbor 1:
      static_assert(h1.from[0] == 12, "");
      static_assert(h1.from[1] == 2, "");
      static_assert(h1.from[2] == 2, "");
      static_assert(h1.to[0] == 13, "");
      static_assert(h1.to[1] == 11, "");
      static_assert(h1.to[2] == 11, "");
      // face neighbor 2:
      static_assert(h2.from[0] == 2, "");
      static_assert(h2.from[1] == 0, "");
      static_assert(h2.from[2] == 2, "");
      static_assert(h2.to[0] == 11, "");
      static_assert(h2.to[1] == 1, "");
      static_assert(h2.to[2] == 11, "");
      // face neighbor 3:
      static_assert(h3.from[0] == 2, "");
      static_assert(h3.from[1] == 12, "");
      static_assert(h3.from[2] == 2, "");
      static_assert(h3.to[0] == 11, "");
      static_assert(h3.to[1] == 13, "");
      static_assert(h3.to[2] == 11, "");
      // face neighbor 4:
      static_assert(h4.from[0] == 2, "");
      static_assert(h4.from[1] == 2, "");
      static_assert(h4.from[2] == 0, "");
      static_assert(h4.to[0] == 11, "");
      static_assert(h4.to[1] == 11, "");
      static_assert(h4.to[2] == 1, "");
      // face neighbor 5:
      static_assert(h5.from[0] == 2, "");
      static_assert(h5.from[1] == 2, "");
      static_assert(h5.from[2] == 12, "");
      static_assert(h5.to[0] == 11, "");
      static_assert(h5.to[1] == 11, "");
      static_assert(h5.to[2] == 13, "");
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
      static_assert(h0.from[0] == 0, "");
      static_assert(h0.from[1] == 0, "");
      static_assert(h0.from[2] == 2, "");
      static_assert(h0.to[0] == 1, "");
      static_assert(h0.to[1] == 1, "");
      static_assert(h0.to[2] == 11, "");
      // edge neighbor 1:
      static_assert(h1.from[0] == 12, "");
      static_assert(h1.from[1] == 0, "");
      static_assert(h1.from[2] == 2, "");
      static_assert(h1.to[0] == 13, "");
      static_assert(h1.to[1] == 1, "");
      static_assert(h1.to[2] == 11, "");
      // edge neighbor 2:
      static_assert(h2.from[0] == 0, "");
      static_assert(h2.from[1] == 12, "");
      static_assert(h2.from[2] == 2, "");
      static_assert(h2.to[0] == 1, "");
      static_assert(h2.to[1] == 13, "");
      static_assert(h2.to[2] == 11, "");
      // edge neighbor 3:
      static_assert(h3.from[0] == 12, "");
      static_assert(h3.from[1] == 12, "");
      static_assert(h3.from[2] == 2, "");
      static_assert(h3.to[0] == 13, "");
      static_assert(h3.to[1] == 13, "");
      static_assert(h3.to[2] == 11, "");
      // edge neighbor 4:
      static_assert(h4.from[0] == 0, "");
      static_assert(h4.from[1] == 2, "");
      static_assert(h4.from[2] == 0, "");
      static_assert(h4.to[0] == 1, "");
      static_assert(h4.to[1] == 11, "");
      static_assert(h4.to[2] == 1, "");
      // edge neighbor 5:
      static_assert(h5.from[0] == 12, "");
      static_assert(h5.from[1] == 2, "");
      static_assert(h5.from[2] == 0, "");
      static_assert(h5.to[0] == 13, "");
      static_assert(h5.to[1] == 11, "");
      static_assert(h5.to[2] == 1, "");
      // edge neighbor 6:
      static_assert(h6.from[0] == 2, "");
      static_assert(h6.from[1] == 0, "");
      static_assert(h6.from[2] == 0, "");
      static_assert(h6.to[0] == 11, "");
      static_assert(h6.to[1] == 1, "");
      static_assert(h6.to[2] == 1, "");
      // edge neighbor 7:
      static_assert(h7.from[0] == 2, "");
      static_assert(h7.from[1] == 12, "");
      static_assert(h7.from[2] == 0, "");
      static_assert(h7.to[0] == 11, "");
      static_assert(h7.to[1] == 13, "");
      static_assert(h7.to[2] == 1, "");
      // edge neighbor 8:
      static_assert(h8.from[0] == 0, "");
      static_assert(h8.from[1] == 2, "");
      static_assert(h8.from[2] == 12, "");
      static_assert(h8.to[0] == 1, "");
      static_assert(h8.to[1] == 11, "");
      static_assert(h8.to[2] == 13, "");
      // edge neighbor 9:
      static_assert(h9.from[0] == 12, "");
      static_assert(h9.from[1] == 2, "");
      static_assert(h9.from[2] == 12, "");
      static_assert(h9.to[0] == 13, "");
      static_assert(h9.to[1] == 11, "");
      static_assert(h9.to[2] == 13, "");
      // edge neighbor 10:
      static_assert(h10.from[0] == 2, "");
      static_assert(h10.from[1] == 0, "");
      static_assert(h10.from[2] == 12, "");
      static_assert(h10.to[0] == 11, "");
      static_assert(h10.to[1] == 1, "");
      static_assert(h10.to[2] == 13, "");
      // edge neighbor 11:
      static_assert(h11.from[0] == 2, "");
      static_assert(h11.from[1] == 12, "");
      static_assert(h11.from[2] == 12, "");
      static_assert(h11.to[0] == 11, "");
      static_assert(h11.to[1] == 13, "");
      static_assert(h11.to[2] == 13, "");
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
      static_assert(h0.from[0] == 0, "");
      static_assert(h0.from[1] == 0, "");
      static_assert(h0.from[2] == 0, "");
      static_assert(h0.to[0] == 1, "");
      static_assert(h0.to[1] == 1, "");
      static_assert(h0.to[2] == 1, "");
      // corner neighbor 1:
      static_assert(h1.from[0] == 12, "");
      static_assert(h1.from[1] == 0, "");
      static_assert(h1.from[2] == 0, "");
      static_assert(h1.to[0] == 13, "");
      static_assert(h1.to[1] == 1, "");
      static_assert(h1.to[2] == 1, "");
      // corner neighbor 2:
      static_assert(h2.from[0] == 0, "");
      static_assert(h2.from[1] == 12, "");
      static_assert(h2.from[2] == 0, "");
      static_assert(h2.to[0] == 1, "");
      static_assert(h2.to[1] == 13, "");
      static_assert(h2.to[2] == 1, "");
      // corner neighbor 3:
      static_assert(h3.from[0] == 12, "");
      static_assert(h3.from[1] == 12, "");
      static_assert(h3.from[2] == 0, "");
      static_assert(h3.to[0] == 13, "");
      static_assert(h3.to[1] == 13, "");
      static_assert(h3.to[2] == 1, "");
      // corner neighbor 4:
      static_assert(h4.from[0] == 0, "");
      static_assert(h4.from[1] == 0, "");
      static_assert(h4.from[2] == 12, "");
      static_assert(h4.to[0] == 1, "");
      static_assert(h4.to[1] == 1, "");
      static_assert(h4.to[2] == 13, "");
      // corner neighbor 5:
      static_assert(h5.from[0] == 12, "");
      static_assert(h5.from[1] == 0, "");
      static_assert(h5.from[2] == 12, "");
      static_assert(h5.to[0] == 13, "");
      static_assert(h5.to[1] == 1, "");
      static_assert(h5.to[2] == 13, "");
      // corner neighbor 6:
      static_assert(h6.from[0] == 0, "");
      static_assert(h6.from[1] == 12, "");
      static_assert(h6.from[2] == 12, "");
      static_assert(h6.to[0] == 1, "");
      static_assert(h6.to[1] == 13, "");
      static_assert(h6.to[2] == 13, "");
      // corner neighbor 7:
      static_assert(h7.from[0] == 12, "");
      static_assert(h7.from[1] == 12, "");
      static_assert(h7.from[2] == 12, "");
      static_assert(h7.to[0] == 13, "");
      static_assert(h7.to[1] == 13, "");
      static_assert(h7.to[2] == 13, "");
    }
  }  // 3D
  return hm3::test::result();
}
