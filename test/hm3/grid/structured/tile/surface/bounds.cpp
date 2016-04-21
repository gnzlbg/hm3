#include <hm3/grid/structured/tile/surface/bounds.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  {
    using bounds_t = grid::structured::tile::surface::bounds<1, 1>;
    constexpr bounds_t b{};
    constexpr auto sides_pd = 2_u;
    constexpr auto size_ps  = 1_u;
    constexpr auto size_pd  = 2_u;
    constexpr auto size_    = 2_u;
    constexpr auto size_pc  = 2_u;
    CHECK(b.sides_per_dir() == sides_pd);
    CHECK(b.size_per_side() == size_ps);
    CHECK(b.size_per_dir() == size_pd);
    CHECK(b.size() == size_);
    CHECK(b.size_per_cell() == size_pc);
    static_assert(b.sides_per_dir() == sides_pd, "");
    static_assert(b.size_per_side() == size_ps, "");
    static_assert(b.size_per_dir() == size_pd, "");
    static_assert(b.size() == size_, "");
    static_assert(b.size_per_cell() == size_pc, "");
  }

  {
    using bounds_t = grid::structured::tile::surface::bounds<2, 1>;
    constexpr bounds_t b{};
    constexpr auto sides_pd = 2_u;
    constexpr auto size_ps  = 1_u;
    constexpr auto size_pd  = 2_u;
    constexpr auto size_    = 4_u;
    constexpr auto size_pc  = 4_u;
    CHECK(b.sides_per_dir() == sides_pd);
    CHECK(b.size_per_side() == size_ps);
    CHECK(b.size_per_dir() == size_pd);
    CHECK(b.size() == size_);
    CHECK(b.size_per_cell() == size_pc);
    static_assert(b.sides_per_dir() == sides_pd, "");
    static_assert(b.size_per_side() == size_ps, "");
    static_assert(b.size_per_dir() == size_pd, "");
    static_assert(b.size() == size_, "");
    static_assert(b.size_per_cell() == size_pc, "");
  }

  {
    using bounds_t = grid::structured::tile::surface::bounds<3, 1>;
    constexpr bounds_t b{};
    constexpr auto sides_pd = 2_u;
    constexpr auto size_ps  = 1_u;
    constexpr auto size_pd  = 2_u;
    constexpr auto size_    = 6_u;
    constexpr auto size_pc  = 6_u;
    CHECK(b.sides_per_dir() == sides_pd);
    CHECK(b.size_per_side() == size_ps);
    CHECK(b.size_per_dir() == size_pd);
    CHECK(b.size() == size_);
    CHECK(b.size_per_cell() == size_pc);
    static_assert(b.sides_per_dir() == sides_pd, "");
    static_assert(b.size_per_side() == size_ps, "");
    static_assert(b.size_per_dir() == size_pd, "");
    static_assert(b.size() == size_, "");
    static_assert(b.size_per_cell() == size_pc, "");
  }

  {
    using bounds_t = grid::structured::tile::surface::bounds<1, 4>;
    constexpr bounds_t b{};
    constexpr auto sides_pd = 5_u;
    constexpr auto size_ps  = 1_u;
    constexpr auto size_pd  = 5_u;
    constexpr auto size_    = 5_u;
    CHECK(b.sides_per_dir() == sides_pd);
    CHECK(b.size_per_side() == size_ps);
    CHECK(b.size_per_dir() == size_pd);
    CHECK(b.size() == size_);
    static_assert(b.sides_per_dir() == sides_pd, "");
    static_assert(b.size_per_side() == size_ps, "");
    static_assert(b.size_per_dir() == size_pd, "");
    static_assert(b.size() == size_, "");
  }

  {
    using bounds_t = grid::structured::tile::surface::bounds<2, 4>;
    constexpr bounds_t b{};
    constexpr auto sides_pd = 5_u;
    constexpr auto size_ps  = 4_u;
    constexpr auto size_pd  = 20_u;
    constexpr auto size_    = 40_u;
    CHECK(b.sides_per_dir() == sides_pd);
    CHECK(b.size_per_side() == size_ps);
    CHECK(b.size_per_dir() == size_pd);
    CHECK(b.size() == size_);
    static_assert(b.sides_per_dir() == sides_pd, "");
    static_assert(b.size_per_side() == size_ps, "");
    static_assert(b.size_per_dir() == size_pd, "");
    static_assert(b.size() == size_, "");
  }

  {
    using bounds_t = grid::structured::tile::surface::bounds<3, 4>;
    constexpr bounds_t b{};
    constexpr auto sides_pd = 5_u;
    constexpr auto size_ps  = 16_u;
    constexpr auto size_pd  = 80_u;
    constexpr auto size_    = 240_u;
    CHECK(b.sides_per_dir() == sides_pd);
    CHECK(b.size_per_side() == size_ps);
    CHECK(b.size_per_dir() == size_pd);
    CHECK(b.size() == size_);
    static_assert(b.sides_per_dir() == sides_pd, "");
    static_assert(b.size_per_side() == size_ps, "");
    static_assert(b.size_per_dir() == size_pd, "");
    static_assert(b.size() == size_, "");
  }

  return test::result();
}
