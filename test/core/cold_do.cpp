////////////////////////////////////////////////////////////////////////////////
/// \file Test cold_do utility
////////////////////////////////////////////////////////////////////////////////
#include <hom3/core/test.hpp>
#include <hom3/core/cold_do.hpp>
////////////////////////////////////////////////////////////////////////////////
using namespace hom3;

int main() {
  auto f = []() { return 3; };
  CHECK(f() == cold_do(f));
  return test::result();
}
////////////////////////////////////////////////////////////////////////////////
