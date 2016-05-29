#include "dimension.hpp"
#include <hm3/geometry/dimension/dimension.hpp>
#include <hm3/utility/test.hpp>

int main() {
  using hm3::geometry::dimension;
  using hm3::dim_t;
  STATIC_CHECK(dimension(with_member_dimension{}) == dim_t{2});
  STATIC_CHECK(dimension(other::with_nonmember_dimension{}) == dim_t{3});

  return hm3::test::result();
}
