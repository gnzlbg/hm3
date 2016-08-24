#include <hm3/geometry/sd/concepts.hpp>
#include <hm3/geometry/sd/sphere.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

static_assert(geometry::SignedDistance<geometry::sd::sphere_t<1>, 1>{}, "");
// static_assert(geometry::SignedDistance<geometry::sd::fixed_sphere<2>, 2>{},
// "");

template <uint_t Nd>
struct p_and_v {
  geometry::point<Nd> p;
  num_t v;
};

template <typename SD, typename PVs>
void check_sd(SD&& sd, PVs&& pvs) {
  for (auto&& pv : pvs) { CHECK(sd(pv.p) == pv.v); }
}

int main() {
  using namespace geometry;

  {  // sd-function 1D
    point<1> xc = point<1>::constant(0.5);
    num_t r     = 0.5;

    p_and_v<1> pvs[] = {
     {{-.5}, .5},  //
     {{.0}, .0},   //
     {{.5}, -.5},  //
     {{1.}, .0},   //
     {{1.5}, .5}   //
    };

    check_sd([&](auto&& x) { return sd::sphere(x, xc, r); }, pvs);
  }

  {  // sd-function 2D
    point<2> xc{0.5, 0.};
    num_t r = 0.5;

    p_and_v<2> pvs[] = {
     {{-1., 0.}, 1.},   //
     {{-.5, 0.}, .5},   //
     {{0.0, 0.}, .0},   //
     {{.5, 0.}, -.5},   //
     {{1., 0.}, .0},    //
     {{1.5, 0.}, .5},   //
     {{.5, -0.5}, .0},  //
     {{.5, -1.}, .5},   //
     {{.5, .5}, .0},    //
     {{.5, 1.}, .5}     //
    };

    check_sd([&](auto&& x) { return sd::sphere(x, xc, r); }, pvs);
  }

  return test::result();
}
