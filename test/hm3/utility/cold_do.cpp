/// \file cold_do.cpp Test cold_do utility
#include <hm3/utility/test.hpp>
#include <hm3/utility/cold_do.hpp>
using namespace hm3;

int main() {
  auto f = []() { return 3; };
  CHECK(f() == cold_do(f));
  return test::result();
}
