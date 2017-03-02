/// \file
///
/// \file
///
/// Ray tests.
#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/concept/ray.hpp>
#include <hm3/geometry/io/ascii.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template struct hm3::geometry::ray_primitive::ray<1>;
template struct hm3::geometry::ray_primitive::ray<2>;
template struct hm3::geometry::ray_primitive::ray<3>;

template <dim_t Ad>
void basic_ray_test() {
  using namespace geometry;

  static constexpr dim_t ad = Ad;
  using r_t                 = ray<ad>;
  using p_t                 = point<ad>;
  using v_t                 = vec<ad>;

  static_assert(AmbientDimension<r_t>{}, "");
  static_assert(AmbientDimension<r_t, Ad>{}, "");

  static_assert(ElementDimension<r_t>{}, "");
  static_assert(ElementDimension<r_t, Ad>{}, "");
  static_assert(ElementDimension<r_t, Ad, 1>{}, "");

  static_assert(GeometryObject<r_t>{}, "");
  static_assert(GeometryObject<r_t, Ad>{}, "");
  static_assert(GeometryObject<r_t, Ad, 1>{}, "");

  static_assert(!Line<r_t>{}, "");
  static_assert(!Line<r_t, Ad>{}, "");

  static_assert(LineLike<r_t>{}, "");

  static_assert(Ray<r_t>{}, "");
  static_assert(Ray<r_t, Ad>{}, "");

  auto o0 = p_t::constant(0.);
  auto o1 = p_t::constant(1.);
  auto o2 = p_t::constant(-1.);

  v_t d0  = v_t::c(1.).normalized();
  auto d1 = v_t::unit(0);

  auto r0 = r_t(o0, d0);
  auto r1 = r_t(o1, d0);

  auto r2 = r_t(o2, d1);
  auto r3 = r_t(o0, d1);

  auto r0_ = r_t(o0, o1);

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
  if (ad != 1) { CHECK(!parallel(r0, r2)); }

  // colinear
  CHECK(coincidental_lines(r0.line(), r1.line()));
  if (ad != 1) {
    CHECK(!coincidental_lines(r2.line(), r3.line()));
    CHECK(!coincidental_lines(r0.line(), r2.line()));
  }

  // parameter
  auto check_parameter = [&](auto&& l, auto&& o, num_t val) {
    auto p = line_intersection_parameter(l.line(), o);
    CHECK(p);
    if (p) { CHECK(p.value() == val); }
  };
  check_parameter(r0, o0, 0.);
  check_parameter(r0, o1, std::sqrt(ad));
  check_parameter(r1, o0, -std::sqrt(ad));
  check_parameter(r1, o1, 0.);
}

int main() {
  basic_ray_test<1>();
  basic_ray_test<2>();
  basic_ray_test<3>();

  return test::result();
}
