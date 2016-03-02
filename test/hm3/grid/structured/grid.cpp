/// \file
///
/// Test square structured grid indices
#include <hm3/structured/square/grid/indices.hpp>
#include <hm3/utility/test.hpp>
#include "print_grid.hpp"

using namespace hm3;

template <typename G> void test_grid(G) {
  constexpr G g{};

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
  CHECK(c == g.internal_cells());

  c = 0;
  g.for_each_halo([&](auto i) {
    CHECK(!i.is_internal());
    CHECK(i.is_halo());
    ++c;
  });
  CHECK(c == g.halo_cells());
}

int main() {
  using g1d1 = structured::square::grid::indices<1, 10, 1>;
  using g2d1 = structured::square::grid::indices<2, 10, 1>;
  using g3d1 = structured::square::grid::indices<3, 10, 1>;

  using g1d2 = structured::square::grid::indices<1, 10, 2>;
  using g2d2 = structured::square::grid::indices<2, 10, 2>;
  using g3d2 = structured::square::grid::indices<3, 10, 2>;

  test_grid(g1d1{});
  test_grid(g2d1{});
  test_grid(g3d1{});

  test_grid(g1d2{});
  test_grid(g2d2{});
  test_grid(g3d2{});

  // todo: test for_each_neighbor
  // todo: test for_each_halo
  return test::result();
}
