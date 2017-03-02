#include <hm3/geometry/sd/functions/sphere.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <uint_t Ad>
struct p_and_v {
  point<Ad> p;
  num_t v;
};

template <typename SD, typename PVs>
void check_sd(SD&& sd, PVs&& pvs) {
  for (auto&& pv : pvs) { CHECK(sd(pv.p) == pv.v); }
}

int main() {
  using namespace geometry;

  {  // sd-function 1D
    static constexpr dim_t ad = 1;

    using p_t = point<ad>;

    static_assert(SignedDistance<sd::sphere_at_point<ad>, p_t>{});

    p_t xc  = p_t::constant(0.5);
    num_t r = 0.5;

    p_and_v<ad> pvs[] = {
     {{-.5}, .5},  //
     {{.0}, .0},   //
     {{.5}, -.5},  //
     {{1.}, .0},   //
     {{1.5}, .5}   //
    };

    sd::sphere_at_point<ad> s(xc, r);
    check_sd([&](auto&& x) { return s(x); }, pvs);
  }

  {  // sd-function 2D
    static constexpr dim_t ad = 2;

    using p_t = point<ad>;
    static_assert(SignedDistance<sd::sphere_at_point<ad>, p_t>{});

    p_t xc{0.5, 0.};
    num_t r = 0.5;

    p_and_v<ad> pvs[] = {
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

    sd::sphere_at_point<ad> s(xc, r);
    check_sd([&](auto&& x) { return s(x); }, pvs);
  }

  return test::result();
}
