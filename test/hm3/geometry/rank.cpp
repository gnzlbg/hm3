/// \file
///
/// Test the ranked concept.
#include <hm3/geometry/rank.hpp>
#include <hm3/utility/test.hpp>

template <hm3::dim_t Nd, hm3::dim_t R, typename T>  //
constexpr void check_rank(T&&) {
  static_assert(hm3::geometry::Ranked<T>{}, "");
  static_assert(hm3::geometry::Ranked<T, Nd>{}, "");
  static_assert(hm3::geometry::Ranked<T, Nd, R>{}, "");
}

int main() {
  check_rank<1, 1>(hm3::geometry::ranked<1, 1>{});
  check_rank<1, 0>(hm3::geometry::ranked<1, 0>{});
  check_rank<2, 2>(hm3::geometry::ranked<2, 2>{});
  check_rank<2, 1>(hm3::geometry::ranked<2, 1>{});
  check_rank<2, 0>(hm3::geometry::ranked<2, 0>{});
  check_rank<3, 3>(hm3::geometry::ranked<3, 3>{});
  check_rank<3, 2>(hm3::geometry::ranked<3, 2>{});
  check_rank<3, 1>(hm3::geometry::ranked<3, 1>{});
  check_rank<3, 0>(hm3::geometry::ranked<3, 0>{});
  return hm3::test::result();
}
