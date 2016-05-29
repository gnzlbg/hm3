#include "dimension.hpp"

#include <hm3/geometry/dimension/dimensions.hpp>
#include <hm3/utility/test.hpp>

int main() {
  using hm3::dim_t;
  {
    int c = 0;
    RANGES_FOR (auto&& d, hm3::dimensions(with_member_dimension{})) {
      c++;
      CHECK(d < dim_t{2});
    }
    CHECK(c == 2);
  }

  {
    int c = 0;
    RANGES_FOR (auto&& d, hm3::dimensions(with_member_dimension2{})) {
      c++;
      CHECK(d < dim_t{2});
    }
    CHECK(c == 2);
  }

  {
    int c = 0;
    RANGES_FOR (auto&& d, hm3::dimensions(other::with_nonmember_dimension{})) {
      c++;
      CHECK(d < dim_t{3});
    }
    CHECK(c == 3);
  }

  {
    int c = 0;
    RANGES_FOR (auto&& d, hm3::dimensions(other::with_nonmember_dimension2{})) {
      c++;
      CHECK(d < dim_t{3});
    }
    CHECK(c == 3);
  }

  return hm3::test::result();
}
