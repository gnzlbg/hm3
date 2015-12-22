#include <hm3/geometry/dimension/dimension.hpp>
#include <hm3/utility/test.hpp>
#include "dimension.hpp"

int main() {
  static_assert(hm3::dimension(with_member_dimension{}) == 2, "");
  static_assert(hm3::dimension(other::with_nonmember_dimension{}) == 3, "");

  return hm3::test::result();
}
