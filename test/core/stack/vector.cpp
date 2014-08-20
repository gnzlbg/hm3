////////////////////////////////////////////////////////////////////////////////
/// \file Test stack vector
////////////////////////////////////////////////////////////////////////////////
#include <hom3/core/test.hpp>
#include <hom3/core/stack/vector.hpp>
////////////////////////////////////////////////////////////////////////////////
using namespace hom3;

template struct stack::vector<int, 20>;

int main() {
  using stack_vec = stack::vector<int, 5>;
  stack_vec vec1{0, 1, 2, 3, 4};
  {
    stack_vec vec2{5, 6, 7, 8, 9};
    CHECK(vec1[0] == 0);
    CHECK(vec1[4] == 4);
    CHECK(vec2[0] == 5);
    CHECK(vec2[4] == 9);
  }
  {
    auto vec2 = vec1;
    CHECK(vec2[0] == 0);
    CHECK(vec2[4] == 4);
    CHECK(vec1[0] == 0);
    CHECK(vec1[4] == 4);
  }
  {
    int count = 0;
    for (auto i : vec1) { CHECK(i == count++); }
  }
  {
    std::vector<int> vec2{4, 3, 2, 1, 0};
    stack_vec vec(vec2);
    int count = 4;
    for (auto i : vec) { CHECK(i == count--); }
  }
  {
    stack_vec vec(std::vector<int>{4, 3, 2, 1, 0});
    int count = 4;
    for (auto i : vec) { CHECK(i == count--); }
  }
  return test::result();
}
////////////////////////////////////////////////////////////////////////////////
