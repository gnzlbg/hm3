/// \file
///
/// Geometry box tests
#include <hm3/geometry/primitive/box.hpp>
#include "aabb.hpp"

int main() {
  test_unit_aabbs<box>();
  return test::result();
}
