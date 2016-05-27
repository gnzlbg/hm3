#include "indices.hpp"

using namespace hm3;

using tidx_t = grid::structured::tile::tidx_t;

template <dim_t Nd, tidx_t Nc>
using surfaces = grid::structured::tile::surface::indices<Nd, Nc>;

int main() {
  {
    static constexpr surfaces<1, 5> s1d{};
    check_surface_indices(s1d, test_1d_5{});
  }
  {
    static constexpr surfaces<2, 3> s2d{};
    check_surface_indices(s2d, test_2d_3{});
  }
  {
    static constexpr surfaces<3, 2> s3d{};
    check_surface_indices(s3d, test_3d_2{});
  }

  return test::result();
}
