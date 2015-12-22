#include <hm3/geometry/dimensions.hpp>
#include <hm3/utility/test.hpp>
#include "dimension.hpp"

using namespace hm3::geometry;

int main() {
  static_assert(Dimensional<with_member_dimension>{}, "");

  static_assert(NDimensional<with_member_dimension, 2>{}, "");

  static_assert(Dimensional<with_member_dimension2>{}, "");
  static_assert(NDimensional<with_member_dimension2, 2>{}, "");

  static_assert(Dimensional<other::with_nonmember_dimension>{}, "");
  static_assert(NDimensional<other::with_nonmember_dimension, 3>{}, "");

  static_assert(Dimensional<other::with_nonmember_dimension2>{}, "");
  static_assert(NDimensional<other::with_nonmember_dimension2, 3>{}, "");

  return hm3::test::result();
}
