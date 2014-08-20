/// \file
///
/// Implicit signed-distance geometry tests
#include <hm3/geometry/sd.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  using namespace geometry;
  using namespace sd;

  constexpr uint_t nd = 1;

  {  // sphere tests:
    constexpr auto x_center = point<nd>{0.};
    constexpr auto radius   = 1.0;

    constexpr auto s = fixed_sphere<nd>{x_center, radius};

    static_assert(dimension(s) == 1, "");
  }

  {  // edge tests:
    constexpr auto x_center = point<nd>{0.};
    constexpr auto normal   = vector<nd>{1.};

    constexpr auto s = edge<nd>{x_center, normal};

    static_assert(dimension(s) == 1, "");
  }

  return test::result();
}
