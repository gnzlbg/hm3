#include <hm3/interpolation/line.hpp>

int main() {
  // line tests
  {
    auto l0 = line<2>::through(point<2>::constant(0.), point<2>::constant(1.));
    num_t l = std::sqrt(2.);
    point<2> p_c = point<2>::constant(0.5);
    CHECK(dimension(l0) == 2_u);
    CHECK(length(l0) == l);
    CHECK(centroid(l0) == point<2>::constant(0.5));
    CHECK(direction(l0) == point<2>::constant(1.0 / l));
    point<2> n;
    n(0) = -1. / l;
    n(1) = 1. / l;
    CHECK(normal(l0) == n);

    std::array<num_t, 2> vs{{0., 1.}};
    CHECK(interpolate(l0, vs, l / 2.) == 0.5);
    CHECK(distance_to_value(l0, vs, 0.5) == l / 2.);
    CHECK(point_with_value(l0, vs, 0.5) == p_c);
    std::array<num_t, 2> vs2{{-1., 1.}};
    CHECK(interpolate(l0, vs2, l / 2.) == 0.0);
    CHECK(distance_to_value(l0, vs2, 0.0) == l / 2.);
    CHECK(point_with_value(l0, vs2, 0.0) == p_c);
  }

  return test::result();
}
