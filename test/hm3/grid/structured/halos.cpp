/// \file
///
/// Test halo tile indices
#include <hm3/structured/square/grid/halos.hpp>
#include <hm3/utility/test.hpp>
#include <iomanip>

void print_2d_grid() {
  for (int j = 0; j < 14; ++j) {
    for (int i = 0; i < 14; ++i) {
      std::cout << std::setw(3) << (j * 10 + i) << " ";
      if (i == 1 || i == 11) { std::cout << "  "; }
    }
    std::cout << std::endl;
    if (j == 1 or j == 11) { std::cout << std::endl; }
  }
}

int main() {
  /// Test lookup tables:

  {  // 1D, 10Cells, 2 Halo layers, Rank 0 (across faces)
    using fn_t
     = hm3::structured::square::grid::halo_tiles_lookup_table<1, 0, 10, 2>;
    static constexpr fn_t fn{};

    static_assert(fn.indices[0].from[0] == 0, "");
    static_assert(fn.indices[0].to[0] == 1, "");

    static_assert(fn.indices[1].from[0] == 12, "");
    static_assert(fn.indices[1].to[0] == 13, "");
  }

  {  // 2D, 10Cells, 2 Halo layers, Rank 0
    // print_2d_grid();
    using fn_t
     = hm3::structured::square::grid::halo_tiles_lookup_table<2, 0, 10, 2>;
    static constexpr fn_t fn{};

    // face neighbor 0:
    static_assert(fn.indices[0].from[0] == 0, "");
    static_assert(fn.indices[0].from[1] == 2, "");
    static_assert(fn.indices[0].to[0] == 1, "");
    static_assert(fn.indices[0].to[1] == 11, "");

    // face neighbor 1:
    static_assert(fn.indices[1].from[0] == 12, "");
    static_assert(fn.indices[1].from[1] == 2, "");
    static_assert(fn.indices[1].to[0] == 13, "");
    static_assert(fn.indices[1].to[1] == 11, "");

    // face neighbor 2:
    static_assert(fn.indices[2].from[0] == 2, "");
    static_assert(fn.indices[2].from[1] == 0, "");
    static_assert(fn.indices[2].to[0] == 11, "");
    static_assert(fn.indices[2].to[1] == 1, "");

    // face neighbor 3:
    static_assert(fn.indices[3].from[0] == 2, "");
    static_assert(fn.indices[3].from[1] == 12, "");
    static_assert(fn.indices[3].to[0] == 11, "");
    static_assert(fn.indices[3].to[1] == 13, "");

    using en_t
     = hm3::structured::square::grid::halo_tiles_lookup_table<2, 1, 10, 2>;
    static constexpr en_t en{};

    // edge neighbor 0:
    static_assert(en.indices[0].from[0] == 0, "");
    static_assert(en.indices[0].from[1] == 0, "");
    static_assert(en.indices[0].to[0] == 1, "");
    static_assert(en.indices[0].to[1] == 1, "");

    // edge neighbor 1:
    static_assert(en.indices[1].from[0] == 12, "");
    static_assert(en.indices[1].from[1] == 0, "");
    static_assert(en.indices[1].to[0] == 13, "");
    static_assert(en.indices[1].to[1] == 1, "");

    // edge neighbor 2:
    static_assert(en.indices[2].from[0] == 0, "");
    static_assert(en.indices[2].from[1] == 12, "");
    static_assert(en.indices[2].to[0] == 1, "");
    static_assert(en.indices[2].to[1] == 13, "");

    // edge neighbor 3:
    static_assert(en.indices[3].from[0] == 12, "");
    static_assert(en.indices[3].from[1] == 12, "");
    static_assert(en.indices[3].to[0] == 13, "");
    static_assert(en.indices[3].to[1] == 13, "");
  }

  {  // 3D, 10Cells, 2 Halo layers, Rank 0
    using fn_t
     = hm3::structured::square::grid::halo_tiles_lookup_table<3, 0, 10, 2>;
    static constexpr fn_t fn{};

    // face neighbor 0:
    static_assert(fn.indices[0].from[0] == 0, "");
    static_assert(fn.indices[0].from[1] == 2, "");
    static_assert(fn.indices[0].from[2] == 2, "");
    static_assert(fn.indices[0].to[0] == 1, "");
    static_assert(fn.indices[0].to[1] == 11, "");
    static_assert(fn.indices[0].to[2] == 11, "");
    // face neighbor 1:
    static_assert(fn.indices[1].from[0] == 12, "");
    static_assert(fn.indices[1].from[1] == 2, "");
    static_assert(fn.indices[1].from[2] == 2, "");
    static_assert(fn.indices[1].to[0] == 13, "");
    static_assert(fn.indices[1].to[1] == 11, "");
    static_assert(fn.indices[1].to[2] == 11, "");
    // face neighbor 2:
    static_assert(fn.indices[2].from[0] == 2, "");
    static_assert(fn.indices[2].from[1] == 0, "");
    static_assert(fn.indices[2].from[2] == 2, "");
    static_assert(fn.indices[2].to[0] == 11, "");
    static_assert(fn.indices[2].to[1] == 1, "");
    static_assert(fn.indices[2].to[2] == 11, "");
    // face neighbor 3:
    static_assert(fn.indices[3].from[0] == 2, "");
    static_assert(fn.indices[3].from[1] == 12, "");
    static_assert(fn.indices[3].from[2] == 2, "");
    static_assert(fn.indices[3].to[0] == 11, "");
    static_assert(fn.indices[3].to[1] == 13, "");
    static_assert(fn.indices[3].to[2] == 11, "");
    // face neighbor 4:
    static_assert(fn.indices[4].from[0] == 2, "");
    static_assert(fn.indices[4].from[1] == 2, "");
    static_assert(fn.indices[4].from[2] == 0, "");
    static_assert(fn.indices[4].to[0] == 11, "");
    static_assert(fn.indices[4].to[1] == 11, "");
    static_assert(fn.indices[4].to[2] == 1, "");
    // face neighbor 5:
    static_assert(fn.indices[5].from[0] == 2, "");
    static_assert(fn.indices[5].from[1] == 2, "");
    static_assert(fn.indices[5].from[2] == 12, "");
    static_assert(fn.indices[5].to[0] == 11, "");
    static_assert(fn.indices[5].to[1] == 11, "");
    static_assert(fn.indices[5].to[2] == 13, "");

    using en_t
     = hm3::structured::square::grid::halo_tiles_lookup_table<3, 1, 10, 2>;
    static constexpr en_t en{};
    // edge neighbor 0:
    static_assert(en.indices[0].from[0] == 0, "");
    static_assert(en.indices[0].from[1] == 0, "");
    static_assert(en.indices[0].from[2] == 2, "");
    static_assert(en.indices[0].to[0] == 1, "");
    static_assert(en.indices[0].to[1] == 1, "");
    static_assert(en.indices[0].to[2] == 11, "");
    // edge neighbor 1:
    static_assert(en.indices[1].from[0] == 12, "");
    static_assert(en.indices[1].from[1] == 0, "");
    static_assert(en.indices[1].from[2] == 2, "");
    static_assert(en.indices[1].to[0] == 13, "");
    static_assert(en.indices[1].to[1] == 1, "");
    static_assert(en.indices[1].to[2] == 11, "");
    // edge neighbor 2:
    static_assert(en.indices[2].from[0] == 0, "");
    static_assert(en.indices[2].from[1] == 12, "");
    static_assert(en.indices[2].from[2] == 2, "");
    static_assert(en.indices[2].to[0] == 1, "");
    static_assert(en.indices[2].to[1] == 13, "");
    static_assert(en.indices[2].to[2] == 11, "");
    // edge neighbor 3:
    static_assert(en.indices[3].from[0] == 12, "");
    static_assert(en.indices[3].from[1] == 12, "");
    static_assert(en.indices[3].from[2] == 2, "");
    static_assert(en.indices[3].to[0] == 13, "");
    static_assert(en.indices[3].to[1] == 13, "");
    static_assert(en.indices[3].to[2] == 11, "");
    // edge neighbor 4:
    static_assert(en.indices[4].from[0] == 0, "");
    static_assert(en.indices[4].from[1] == 2, "");
    static_assert(en.indices[4].from[2] == 0, "");
    static_assert(en.indices[4].to[0] == 1, "");
    static_assert(en.indices[4].to[1] == 11, "");
    static_assert(en.indices[4].to[2] == 1, "");
    // edge neighbor 5:
    static_assert(en.indices[5].from[0] == 12, "");
    static_assert(en.indices[5].from[1] == 2, "");
    static_assert(en.indices[5].from[2] == 0, "");
    static_assert(en.indices[5].to[0] == 13, "");
    static_assert(en.indices[5].to[1] == 11, "");
    static_assert(en.indices[5].to[2] == 1, "");
    // edge neighbor 6:
    static_assert(en.indices[6].from[0] == 2, "");
    static_assert(en.indices[6].from[1] == 0, "");
    static_assert(en.indices[6].from[2] == 0, "");
    static_assert(en.indices[6].to[0] == 11, "");
    static_assert(en.indices[6].to[1] == 1, "");
    static_assert(en.indices[6].to[2] == 1, "");
    // edge neighbor 7:
    static_assert(en.indices[7].from[0] == 2, "");
    static_assert(en.indices[7].from[1] == 12, "");
    static_assert(en.indices[7].from[2] == 0, "");
    static_assert(en.indices[7].to[0] == 11, "");
    static_assert(en.indices[7].to[1] == 13, "");
    static_assert(en.indices[7].to[2] == 1, "");
    // edge neighbor 8:
    static_assert(en.indices[8].from[0] == 0, "");
    static_assert(en.indices[8].from[1] == 2, "");
    static_assert(en.indices[8].from[2] == 12, "");
    static_assert(en.indices[8].to[0] == 1, "");
    static_assert(en.indices[8].to[1] == 11, "");
    static_assert(en.indices[8].to[2] == 13, "");
    // edge neighbor 9:
    static_assert(en.indices[9].from[0] == 12, "");
    static_assert(en.indices[9].from[1] == 2, "");
    static_assert(en.indices[9].from[2] == 12, "");
    static_assert(en.indices[9].to[0] == 13, "");
    static_assert(en.indices[9].to[1] == 11, "");
    static_assert(en.indices[9].to[2] == 13, "");
    // edge neighbor 10:
    static_assert(en.indices[10].from[0] == 2, "");
    static_assert(en.indices[10].from[1] == 0, "");
    static_assert(en.indices[10].from[2] == 12, "");
    static_assert(en.indices[10].to[0] == 11, "");
    static_assert(en.indices[10].to[1] == 1, "");
    static_assert(en.indices[10].to[2] == 13, "");
    // edge neighbor 11:
    static_assert(en.indices[11].from[0] == 2, "");
    static_assert(en.indices[11].from[1] == 12, "");
    static_assert(en.indices[11].from[2] == 12, "");
    static_assert(en.indices[11].to[0] == 11, "");
    static_assert(en.indices[11].to[1] == 13, "");
    static_assert(en.indices[11].to[2] == 13, "");

    using cn_t
     = hm3::structured::square::grid::halo_tiles_lookup_table<3, 2, 10, 2>;
    static constexpr cn_t cn{};
    // corner neighbor 0:
    static_assert(cn.indices[0].from[0] == 0, "");
    static_assert(cn.indices[0].from[1] == 0, "");
    static_assert(cn.indices[0].from[2] == 0, "");
    static_assert(cn.indices[0].to[0] == 1, "");
    static_assert(cn.indices[0].to[1] == 1, "");
    static_assert(cn.indices[0].to[2] == 1, "");
    // corner neighbor 1:
    static_assert(cn.indices[1].from[0] == 12, "");
    static_assert(cn.indices[1].from[1] == 0, "");
    static_assert(cn.indices[1].from[2] == 0, "");
    static_assert(cn.indices[1].to[0] == 13, "");
    static_assert(cn.indices[1].to[1] == 1, "");
    static_assert(cn.indices[1].to[2] == 1, "");
    // corner neighbor 2:
    static_assert(cn.indices[2].from[0] == 0, "");
    static_assert(cn.indices[2].from[1] == 12, "");
    static_assert(cn.indices[2].from[2] == 0, "");
    static_assert(cn.indices[2].to[0] == 1, "");
    static_assert(cn.indices[2].to[1] == 13, "");
    static_assert(cn.indices[2].to[2] == 1, "");
    // corner neighbor 3:
    static_assert(cn.indices[3].from[0] == 12, "");
    static_assert(cn.indices[3].from[1] == 12, "");
    static_assert(cn.indices[3].from[2] == 0, "");
    static_assert(cn.indices[3].to[0] == 13, "");
    static_assert(cn.indices[3].to[1] == 13, "");
    static_assert(cn.indices[3].to[2] == 1, "");
    // corner neighbor 4:
    static_assert(cn.indices[4].from[0] == 0, "");
    static_assert(cn.indices[4].from[1] == 0, "");
    static_assert(cn.indices[4].from[2] == 12, "");
    static_assert(cn.indices[4].to[0] == 1, "");
    static_assert(cn.indices[4].to[1] == 1, "");
    static_assert(cn.indices[4].to[2] == 13, "");
    // corner neighbor 5:
    static_assert(cn.indices[5].from[0] == 12, "");
    static_assert(cn.indices[5].from[1] == 0, "");
    static_assert(cn.indices[5].from[2] == 12, "");
    static_assert(cn.indices[5].to[0] == 13, "");
    static_assert(cn.indices[5].to[1] == 1, "");
    static_assert(cn.indices[5].to[2] == 13, "");
    // corner neighbor 6:
    static_assert(cn.indices[6].from[0] == 0, "");
    static_assert(cn.indices[6].from[1] == 12, "");
    static_assert(cn.indices[6].from[2] == 12, "");
    static_assert(cn.indices[6].to[0] == 1, "");
    static_assert(cn.indices[6].to[1] == 13, "");
    static_assert(cn.indices[6].to[2] == 13, "");
    // corner neighbor 7:
    static_assert(cn.indices[7].from[0] == 12, "");
    static_assert(cn.indices[7].from[1] == 12, "");
    static_assert(cn.indices[7].from[2] == 12, "");
    static_assert(cn.indices[7].to[0] == 13, "");
    static_assert(cn.indices[7].to[1] == 13, "");
    static_assert(cn.indices[7].to[2] == 13, "");
  }

  return hm3::test::result();
}
