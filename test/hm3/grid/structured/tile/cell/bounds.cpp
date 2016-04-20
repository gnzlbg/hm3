#include <hm3/grid/structured/tile/cell/bounds.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  {
    using bounds_t   = grid::structured::tile::cell::bounds<1, 1>;
    constexpr auto b = bounds_t{};
    CHECK(b.length() == 1_u);
    CHECK(b.size() == 1_u);
    static_assert(b.length() == 1_u, "");
    static_assert(b.size() == 1_u, "");
  }

  {
    using bounds_t   = grid::structured::tile::cell::bounds<1, 10>;
    constexpr auto b = bounds_t{};
    CHECK(b.length() == 10_u);
    CHECK(b.size() == 10_u);
    static_assert(b.length() == 10_u, "");
    static_assert(b.size() == 10_u, "");
  }

  {
    using bounds_t   = grid::structured::tile::cell::bounds<2, 1>;
    constexpr auto b = bounds_t{};
    CHECK(b.length() == 1_u);
    CHECK(b.size() == 1_u);
    static_assert(b.length() == 1_u, "");
    static_assert(b.size() == 1_u, "");
  }

  {
    using bounds_t   = grid::structured::tile::cell::bounds<2, 10>;
    constexpr auto b = bounds_t{};
    CHECK(b.length() == 10_u);
    CHECK(b.size() == 100_u);
    static_assert(b.length() == 10_u, "");
    static_assert(b.size() == 100_u, "");
  }

  {
    using bounds_t   = grid::structured::tile::cell::bounds<3, 1>;
    constexpr auto b = bounds_t{};
    CHECK(b.length() == 1_u);
    CHECK(b.size() == 1_u);
    static_assert(b.length() == 1_u, "");
    static_assert(b.size() == 1_u, "");
  }

  {
    using bounds_t   = grid::structured::tile::cell::bounds<3, 10>;
    constexpr auto b = bounds_t{};
    CHECK(b.length() == 10_u);
    CHECK(b.size() == 1000_u);
    static_assert(b.length() == 10_u, "");
    static_assert(b.size() == 1000_u, "");
  }

  return test::result();
}
