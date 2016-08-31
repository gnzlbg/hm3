#include <hm3/geometry/algorithm/sd_intersection/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <dim_t Nd>
void test_sd_segment_intersection() {
  using p_t = geometry::point<Nd>;
  using s_t = geometry::segment<Nd>;

  auto p0  = p_t::constant(0.);
  auto p05 = p_t::constant(0.5);
  auto p1  = p_t::constant(1.);

  auto s = s_t(p0, p1);

  num_t vs0[] = {-1., 1.};
  num_t vs1[] = {0., 1.};
  num_t vs2[] = {-1., 0.};
  num_t vs3[] = {0., 0.};
  num_t vs4[] = {1., 2.};
  num_t vs5[] = {-1., -2.};

  auto sdf = [&](auto&& x, auto&& vs) {
    if (x == s.x(0)) { return vs[0]; }
    return vs[1];
  };

  auto f = [&](auto&& vs) { return [&](auto&& x) { return sdf(x, vs); }; };

  using geometry::sd_intersection;
  using r_t = decltype(sd_intersection(s, f(vs0)));

  CHECK(sd_intersection(s, f(vs0)) == r_t{p05});
  CHECK(sd_intersection(s, f(vs1)) == r_t{p0});
  CHECK(sd_intersection(s, f(vs2)) == r_t{p1});
  CHECK(sd_intersection(s, f(vs3)) == r_t{s});
  CHECK(sd_intersection(s, f(vs4)) == r_t{});
  CHECK(sd_intersection(s, f(vs5)) == r_t{});
}

int main() {
  test_sd_segment_intersection<1>();
  test_sd_segment_intersection<2>();
  test_sd_segment_intersection<3>();
  return test::result();
}
