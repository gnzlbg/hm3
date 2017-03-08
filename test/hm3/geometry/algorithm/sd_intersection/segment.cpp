#include <hm3/geometry/algorithm/sd_intersection.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/geometry/sd.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

template <dim_t Ad>
void test_sd_segment_intersection() {
  using p_t = point<Ad>;
  using s_t = segment<Ad>;

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

  auto f = [&](auto&& vs) {
    return sd::make(dim_v<Ad>, [&](auto&& x) { return sdf(x, vs); });
  };

  {
    using sdt = uncvref_t<decltype(f(vs0))>;
    static_assert(SignedDistance<sdt, p_t>{});
    static_assert(AmbientDimension<sdt, Ad>{});
    static_assert(trait::check<sdt, trait::signed_distance<Ad>>);
  }
  

  using r_t = decltype(sd::intersection(s, f(vs0)));

  CHECK(sd::intersection(s, f(vs0)) == r_t{p05});
  CHECK(sd::intersection.test(s, f(vs0)) == true);
  CHECK(sd::intersection(s, f(vs1)) == r_t{p0});
  CHECK(sd::intersection.test(s, f(vs1)) == true);
  CHECK(sd::intersection(s, f(vs2)) == r_t{p1});
  CHECK(sd::intersection.test(s, f(vs2)) == true);
  CHECK(sd::intersection(s, f(vs3)) == r_t{s});
  CHECK(sd::intersection.test(s, f(vs3)) == true);
  CHECK(sd::intersection(s, f(vs4)) == r_t{});
  CHECK(sd::intersection.test(s, f(vs4)) == false);
  CHECK(sd::intersection(s, f(vs5)) == r_t{});
  CHECK(sd::intersection.test(s, f(vs5)) == false);
}

int main() {
  test_sd_segment_intersection<1>();
  test_sd_segment_intersection<2>();
  test_sd_segment_intersection<3>();
  return test::result();
}
