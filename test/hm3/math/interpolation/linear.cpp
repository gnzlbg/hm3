/// \file
///
/// Linear interpolation tests
///
/// TODO: rename to linear.cpp
#include <hm3/geometry/all.hpp>
#include <hm3/math/interpolation/linear.hpp>
#include <hm3/utility/test.hpp>

int main() {
  using namespace hm3;
  using namespace math::ip::linear;
  using namespace geometry;
  // linear interpolation tests
  {
    auto l0      = segment<2>(point<2>::constant(0.), point<2>::constant(1.));
    num_t l      = std::sqrt(2.);
    point<2> p_c = point<2>::constant(0.5);
    CHECK(ambient_dimension(l0) == 2_u);
    CHECK(bounding_length(l0, 0) == l);
    CHECK(centroid(l0) == point<2>::constant(0.5));
    CHECK(direction(l0) == vec<2>::constant(1.0 / l));
    vec<2> n;
    n(0) = -1. / l;
    n(1) = 1. / l;
    CHECK(geometry::approx(normal(l0), n));

    hm3::array<num_t, 2> vs{{0., 1.}};
    CHECK(interpolate(l / 2., l0, vs) == 0.5);
    CHECK(distance_to_value(.5, l0, vs) == l / 2.);
    CHECK(point_with_value(.5, l0, vs) == p_c);
    hm3::array<num_t, 2> vs2{{-1., 1.}};
    CHECK(interpolate(l / 2., l0, vs2) == 0.0);
    CHECK(distance_to_value(0., l0, vs2) == l / 2.);
    CHECK(point_with_value(0., l0, vs2) == p_c);
  }

  return test::result();
}
