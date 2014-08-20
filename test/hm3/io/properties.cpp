/// \file properties.cpp Tests I/O Properties
#include <memory>
#include <hm3/utility/test.hpp>
#include <hm3/io/properties.hpp>
#include <hm3/geometry/point.hpp>

using namespace hm3;

int main() {
  io::properties ps;
  io::insert<int>(ps, "p_a", 0);
  io::insert<double>(ps, "p_b", 0.5);

  CHECK(io::read<int>(ps, "p_a") == 0);
  CHECK(io::read<double>(ps, "p_b") == 0.5);

  int v_a;
  io::read(ps, "p_a", v_a);
  CHECK(v_a == 0);

  double v_b;
  io::read(ps, "p_b", v_b);
  CHECK(v_b == 0.5);

  geometry::point<2> v_p{0.0, 1.0};
  io::insert<geometry::point<2>>(ps, "p_p", v_p);
  geometry::point<2> v_pr;
  io::read(ps, "p_p", v_pr);
  CHECK(v_pr(0) == v_p(0));
  CHECK(v_pr(1) == v_p(1));

  return test::result();
}
