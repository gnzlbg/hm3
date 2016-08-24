#include <hm3/geometry/primitive/line/coincidental.hpp>
#include <hm3/geometry/primitive/line/parameter.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd>
void basic_ray_test() {
  using namespace geometry;

  static constexpr dim_t nd = Nd;
  using r_t                 = ray<nd>;
  using p_t                 = point<nd>;
  using v_t                 = vec<nd>;

  static_assert(Dimensional<r_t>{}, "");
  static_assert(!Primitive<r_t>{}, "");

  auto o0 = p_t::constant(0.);
  auto o1 = p_t::constant(1.);
  auto o2 = p_t::constant(-1.);

  v_t d0  = v_t::c(1.).normalized();
  auto d1 = v_t::unit(0);

  auto r0 = r_t(o0, d0);
  auto r1 = r_t(o1, d0);

  auto r2 = r_t(o2, d1);
  auto r3 = r_t(o0, d1);

  auto r0_ = r_t::through(o0, o1);

  {  // assignment
    r_t l = r0;
    CHECK(l == r0);
  }

  // relational
  CHECK(r0 == r0);
  CHECK(!(r0 != r0));
  CHECK(r0 != r1);

  // direction
  CHECK(direction(r0) == d0);
  CHECK(direction(r1) == d0);

  // origin
  CHECK(r0.origin() == o0);
  CHECK(r1.origin() == o1);

  // parallel
  CHECK(parallel(r0, r1));
  CHECK(parallel(r2, r3));
  if (nd != 1) { CHECK(!parallel(r0, r2)); }

  // colinear
  CHECK(coincidental(r0.as_line(), r1.as_line()));
  if (nd != 1) {
    CHECK(!coincidental(r2.as_line(), r3.as_line()));
    CHECK(!coincidental(r0.as_line(), r2.as_line()));
  }

  // parameter
  auto check_parameter = [&](auto&& l, auto&& o, num_t val) {
    auto p = parameter(l.as_line(), o);
    CHECK(p);
    if (p) { CHECK(p.value() == val); }
  };
  check_parameter(r0, o0, 0.);
  check_parameter(r0, o1, std::sqrt(nd));
  check_parameter(r1, o0, -std::sqrt(nd));
  check_parameter(r1, o1, 0.);
}

int main() {
  basic_ray_test<1>();
  basic_ray_test<2>();
  basic_ray_test<3>();

  return test::result();
}
