/// \file
///
/// Test square structured grid indices
///
/// teach for each neighbor, for each halo
#include <hm3/grid/structured/halo_tile/cell/indices.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <suint_t Nd, suint_t Nic, suint_t Nhl>  //
void test_indices() {
  using ci_t = grid::structured::halo_tile::cell::indices<Nd, Nic, Nhl>;
  static_assert(std::is_literal_type<ci_t>{}, "");

  // Size tests:
  static_assert(ci_t::dimension() == Nd, "");
  static_assert(ci_t::halo_layers() == Nhl, "");
  static_assert(ci_t::internal_cell_length() == Nic, "");
  static_assert(ci_t::length() == Nic + 2 * Nhl, "");
  static_assert(ci_t::size() == math::ipow(ci_t::length(), Nd), "");
  static_assert(ci_t::internal_cell_size()
                 == math::ipow(ci_t::internal_cell_length(), Nd),
                "");
  static_assert(
   ci_t::halo_cell_size() == ci_t::size() - ci_t::internal_cell_size(), "");

  // Coordinate tests
  using x = typename ci_t::coordinate;
  static_assert(std::is_literal_type<x>{}, "");

  static_assert(ci_t::first_internal() == Nhl, "");
  static_assert(ci_t::first_internal(1) == Nhl - 1, "");
  static_assert(ci_t::last_internal() == Nhl + Nic - 1, "");
  static_assert(ci_t::last_internal(1) == Nhl + Nic + 1 - 1, "");

  static_assert(ci_t::first_halo_left() == 0, "");
  static_assert(ci_t::last_halo_left() == Nhl - 1, "");
  static_assert(ci_t::first_halo_right() == Nhl + Nic, "");
  static_assert(ci_t::last_halo_right() == 2 * Nhl + Nic - 1, "");

  constexpr ci_t g{};

  unsigned c = 0;
  g.for_each([&](auto i) {
    bool should_be_internal = true;
    for (auto d : g.dimensions()) {
      if (i[d] < g.first_internal() or i[d] > g.last_internal()) {
        should_be_internal = false;
        break;
      }
    }

    if (should_be_internal) {
      CHECK(i.is_internal());
      CHECK(!i.is_halo());
    } else {
      CHECK(!i.is_internal());
      CHECK(i.is_halo());
    }
    ++c;
  });
  CHECK(c == g.size());

  c = 0;
  g.for_each_internal([&](auto i) {
    CHECK(i.is_internal());
    CHECK(!i.is_halo());
    ++c;
  });
  CHECK(c == g.internal_cell_size());

  c = 0;
  g.for_each_halo([&](auto i) {
    CHECK(!i.is_internal());
    CHECK(i.is_halo());
    ++c;
  });
  CHECK(c == g.halo_cell_size());
}

int main() {
  {
    // 1D 1.1
    //
    static constexpr suint_t nd  = 1;
    static constexpr suint_t nic = 1;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 1D 2.1
    //
    static constexpr suint_t nd  = 1;
    static constexpr suint_t nic = 2;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 1D 1.2
    //
    static constexpr suint_t nd  = 1;
    static constexpr suint_t nic = 1;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 1D 2.2
    //
    static constexpr suint_t nd  = 1;
    static constexpr suint_t nic = 2;
    static constexpr suint_t nhl = 2;

    test_indices<nd, nic, nhl>();
  }

  {
    // 1D 4.2
    //
    static constexpr suint_t nd  = 1;
    static constexpr suint_t nic = 4;
    static constexpr suint_t nhl = 2;

    test_indices<nd, nic, nhl>();
  }

  {
    // 2D 1.1
    //
    static constexpr suint_t nd  = 2;
    static constexpr suint_t nic = 1;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 2D 2.1
    //
    static constexpr suint_t nd  = 2;
    static constexpr suint_t nic = 2;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 2D 1.2
    //
    static constexpr suint_t nd  = 2;
    static constexpr suint_t nic = 1;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 2D 2.2
    //
    static constexpr suint_t nd  = 2;
    static constexpr suint_t nic = 2;
    static constexpr suint_t nhl = 2;

    test_indices<nd, nic, nhl>();
  }

  {
    // 2D 4.2
    //
    static constexpr suint_t nd  = 2;
    static constexpr suint_t nic = 4;
    static constexpr suint_t nhl = 2;

    test_indices<nd, nic, nhl>();
  }

  {  // 2D 10.2
    static constexpr suint_t nd  = 2;
    static constexpr suint_t nic = 10;
    static constexpr suint_t nhl = 2;

    test_indices<nd, nic, nhl>();
    using ci_t = grid::structured::halo_tile::cell::indices<nd, nic, nhl>;
    constexpr ci_t ci;

    // test all halo cells
    using halo_cells = std::vector<std::vector<suint_t>>;
    {  // face halos:
      using nidx = tree::neighbor_idx_t<tree::face_neighbors<2>>;
      halo_cells face_halos;
      // pos 0
      face_halos.push_back({28, 29, 42,  43,  56,  57,  70,  71,  84,  85,
                            98, 99, 112, 113, 126, 127, 140, 141, 154, 155});
      // pos 1:
      face_halos.push_back({40,  41,  54,  55,  68,  69,  82,  83,  96,  97,
                            110, 111, 124, 125, 138, 139, 152, 153, 166, 167});
      // pos 2:
      face_halos.push_back({2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
                            16, 17, 18, 19, 20, 21, 22, 23, 24, 25});
      // pos 3:
      face_halos.push_back({170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
                            184, 185, 186, 187, 188, 189, 190, 191, 192, 193});

      for (auto p : nidx::rng()) {
        suint_t c = 0;
        ci.for_each_halo(p, [&](auto x) {
          CHECK(face_halos[*p][c] == *x.idx);
          ++c;
        });
        CHECK(c == face_halos[*p].size());
      }
    }
    {  // corner halos:
      using nidx = tree::neighbor_idx_t<tree::edge_neighbors<2>>;
      halo_cells face_halos;
      // pos 0
      face_halos.push_back({0, 1, 14, 15});
      // pos 1:
      face_halos.push_back({12, 13, 26, 27});
      // pos 2:
      face_halos.push_back({168, 169, 182, 183});
      // pos 3:
      face_halos.push_back({180, 181, 194, 195});

      for (auto p : nidx::rng()) {
        suint_t c = 0;
        ci.for_each_halo(p, [&](auto x) {
          CHECK(face_halos[*p][c] == *x.idx);
          ++c;
        });
        CHECK(c == face_halos[*p].size());
      }
    }
  }

  {
    // 3D 1.1
    //
    static constexpr suint_t nd  = 3;
    static constexpr suint_t nic = 1;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 3D 2.1
    //
    static constexpr suint_t nd  = 3;
    static constexpr suint_t nic = 2;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 3D 1.2
    //
    static constexpr suint_t nd  = 3;
    static constexpr suint_t nic = 1;
    static constexpr suint_t nhl = 1;

    test_indices<nd, nic, nhl>();
  }

  {
    // 3D 2.2
    //
    static constexpr suint_t nd  = 3;
    static constexpr suint_t nic = 2;
    static constexpr suint_t nhl = 2;

    test_indices<nd, nic, nhl>();
  }

  {
    // 3D 4.2
    //
    static constexpr suint_t nd  = 3;
    static constexpr suint_t nic = 4;
    static constexpr suint_t nhl = 2;

    test_indices<nd, nic, nhl>();
  }

  return test::result();
}
