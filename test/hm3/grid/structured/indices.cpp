/// \file
///
/// Test square structured grid indices
#include <hm3/grid/structured/indices.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <suint_t Nd, suint_t Nic, suint_t Nhl>  //
void test_indices() {
  using g = grid::structured::indices<Nd, Nic, Nhl>;
  static_assert(std::is_literal_type<g>{}, "");

  // Size tests:
  static_assert(g::dimension() == Nd, "");
  static_assert(g::halo_layers() == Nhl, "");
  static_assert(g::internal_cells_per_length() == Nic, "");
  static_assert(g::cells_per_length() == Nic + 2 * Nhl, "");
  static_assert(g::size() == math::ipow(g::cells_per_length(), Nd), "");
  static_assert(
   g::internal_cells() == math::ipow(g::internal_cells_per_length(), Nd), "");
  static_assert(g::halo_cells() == g::size() - g::internal_cells(), "");

  // Coordinate tests
  using x = typename g::coordinate;
  static_assert(std::is_literal_type<x>{}, "");

  static_assert(g::first_internal() == Nhl, "");
  static_assert(g::first_internal(1) == Nhl - 1, "");
  static_assert(g::last_internal() == Nhl + Nic - 1, "");
  static_assert(g::last_internal(1) == Nhl + Nic + 1 - 1, "");

  static_assert(g::first_halo_left() == 0, "");
  static_assert(g::last_halo_left() == Nhl - 1, "");
  static_assert(g::first_halo_right() == Nhl + Nic, "");
  static_assert(g::last_halo_right() == 2 * Nhl + Nic - 1, "");
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
