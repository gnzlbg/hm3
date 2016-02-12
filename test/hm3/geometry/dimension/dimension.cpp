#include <hm3/geometry/dimension/dimension.hpp>
#include <hm3/utility/test.hpp>
#include "dimension.hpp"

int main() {
  using hm3::geometry::dimension;
  static_assert(dimension(with_member_dimension{}) == 2, "");
  static_assert(dimension(other::with_nonmember_dimension{}) == 3, "");

  return hm3::test::result();
}
