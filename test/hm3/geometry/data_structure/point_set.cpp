#include <hm3/geometry/data_structure/point_set.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  using namespace geometry;
  using namespace tree;

  static constexpr suint_t nd = 2;

  using p_t = point<nd>;
  using ps  = point_set<nd, 2>;

  {
    ps s;
    CHECK(s.size() == 0_u);
  }

  // clang-format off
  p_t points[] = {
    {-0.9, 2.9},  // 0
    {2.9, -0.9},  // 1
    {2.9, 2.9}, // 2
    {-0.9, -0.9}, // 3
    {0.9, 0.9}, // 4 
    {1.1, 1.1}, // 5
    {0.1, 0.1}, // 6
    {0.9, 0.1}  // 7
   //
  };
  p_t points_not_in_set[] = {
    {2., 0.},  // 0
    {0., 2.},  // 1
    //
  };
  // clang-format on

  box<nd> bbox(p_t::constant(1.), 4);

  ps s(40, bbox);  // 20 to trigger assertion

  for (int k = 0; k < 100; ++k) {
    for (suint_t i = 0; i < size(points); ++i) {
      CHECK(s.push_back(points[i]) == i);
      CHECK(s.contains(points[i]).value() == i);
    }
    CHECK(s.size() == size(points));
    for (suint_t i = 0; i < size(points_not_in_set); ++i) {
      CHECK(!s.contains(points_not_in_set[i]));
    }
  }

  CHECK(s.bounding_box() == bbox);

  return test::result();
}
