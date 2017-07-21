#include <hm3/geometry/algorithms.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

#define CHECK_S(ACTUAL, SHOULD)                          \
  visit(                                                 \
   [&](auto&& a_) {                                      \
     if constexpr (Segment<uncvref_t<decltype(a_)>>{}) { \
       CHECK(a_ == (SHOULD));                            \
     } else {                                            \
       CHECK(false);                                     \
     }                                                   \
   },                                                    \
   (ACTUAL))

#define CHECK_P(ACTUAL, SHOULD)                        \
  visit(                                               \
   [&](auto&& a_) {                                    \
     if constexpr (Point<uncvref_t<decltype(a_)>>{}) { \
       CHECK(a_ == (SHOULD));                          \
     } else {                                          \
       CHECK(false);                                   \
     }                                                 \
   },                                                  \
   (ACTUAL))

#define CHECK_M(ACTUAL)                                          \
  visit(                                                         \
   [&](auto&& a_) {                                              \
     if constexpr (Same<uncvref_t<decltype(a_)>, monostate>{}) { \
       CHECK(true);                                              \
     } else {                                                    \
       CHECK(false);                                             \
     }                                                           \
   },                                                            \
   (ACTUAL))

template <dim_t Ad>  //
void test_ray_segment_intersection() {
  using p_t    = point<Ad>;
  using v_t    = vec<Ad>;
  using r_t    = ray<Ad>;
  using aabb_t = aabb<Ad>;
  using s_t    = segment<Ad>;

  auto xa0 = p_t::constant(1.);
  auto xa1 = p_t::constant(2.);

  auto a = aabb_t(xa0, xa1);

  auto rol = p_t::constant(1.);
  rol(0)   = 0.;

  auto ror = p_t::constant(2.);
  ror(0)   = 3.0;

  // Outside of the AABB, in line with the corners

  {  // left corner -> aabb
    auto rd = v_t::unit(0);
    auto r  = r_t(rol, rd);
    CHECK(intersection.test(r, a));
    auto p2 = p_t{xa0() + p_t::unit(0)()};
    auto rs = s_t{xa0, p2};
    CHECK_S(intersection(r, a), rs);
  }
  {  // left corner <- aabb
    auto rd = direction.invert(v_t::unit(0));
    auto r  = r_t(rol, rd);
    CHECK(not intersection.test(r, a));
  }

  {  // right corner -> aabb
    auto rd = v_t::unit(0);
    auto r  = r_t(ror, rd);
    CHECK(not intersection.test(r, a));
  }
  {  // right corner <- aabb
    auto rd = direction.invert(v_t::unit(0));
    auto r  = r_t(ror, rd);
    CHECK(intersection.test(r, a));
    auto p2 = p_t{xa1() - p_t::unit(0)()};
    auto rs = s_t{xa1, p2};
    CHECK_S(intersection(r, a), rs);
  }

  auto rolm = rol;
  if (Ad > 1) { rolm(1) = 1.5; }
  if (Ad > 2) { rolm(2) = 1.5; }

  auto rorm = ror;
  if (Ad > 1) { rorm(1) = 1.5; }
  if (Ad > 2) { rorm(2) = 1.5; }

  // Outside of the AABB in line with the surface centroids of the AABB
  {  // left midplane -> aabb
    auto rd = v_t::unit(0);
    auto r  = r_t(rolm, rd);
    CHECK(intersection.test(r, a));
    auto p0 = rolm;
    p0(0)   = 1;
    auto p1 = p0;
    p1(0)   = 2;
    auto rs = s_t{p0, p1};
    CHECK_S(intersection(r, a), rs);
  }
  {  // left midplane <- aabb
    auto rd = direction.invert(v_t::unit(0));
    auto r  = r_t(rolm, rd);
    CHECK(not intersection.test(r, a));
  }

  {  // right midplane -> aabb
    auto rd = v_t::unit(0);
    auto r  = r_t(rorm, rd);
    CHECK(not intersection.test(r, a));
  }
  {  // right midplane <- aabb
    auto rd = direction.invert(v_t::unit(0));
    auto r  = r_t(rorm, rd);
    CHECK(intersection.test(r, a));
    auto p0 = rorm;
    p0(0)   = 1;
    auto p1 = p0;
    p1(0)   = 2;
    auto rs = s_t{p1, p0};
    CHECK_S(intersection(r, a), rs);
  }

  // Inside of the AABB (always intersects)
  auto roi = centroid(a);
  for (dim_t d = 0; d < Ad; ++d) {
    auto v = v_t::unit(d);
    auto r = r_t(roi, v);
    CHECK(intersection.test(r, a));

    auto ri = r_t(roi, direction.invert(v));
    CHECK(intersection.test(ri, a));

    {
      auto p0 = roi;
      auto p1 = p0;
      p1(d)   = 2;
      auto rs = s_t{p0, p1};
      CHECK_S(intersection(r, a), rs);
    }
    {
      auto p0 = roi;
      auto p1 = p0;
      p1(d)   = 1;
      auto rs = s_t{p0, p1};
      CHECK_S(intersection(ri, a), rs);
    }
  }

  // Origin on aabb corners
  for (auto&& v : vertices(a)) {
    auto r = r_t(v, v_t::unit(0));
    CHECK(intersection.test(r, a));
  }

  // On aabb edge centroids
  if constexpr (Ad > 1) {
    for (auto&& e : edges(a)) {
      auto r = r_t(centroid(e), v_t::unit(0));
      CHECK(intersection.test(r, a));
    }
  }

  // On aabb surface centroids
  if constexpr (Ad > 2) {
    for (auto&& f : faces(a)) {
      auto r = r_t(centroid(f), v_t::unit(0));
      CHECK(intersection.test(r, a));
    }
  }

  // On
}

int main() {
  test_ray_segment_intersection<1>();
  test_ray_segment_intersection<2>();
  test_ray_segment_intersection<3>();

  {  // Intersection with an aabb corner
    using p_t = point<2>;
    vec<2> rd = vec<2>::constant(1. / std::sqrt(2.));
    p_t ro{1.5, 0.5};
    ray<2> r{ro, rd};

    aabb<2> a{p_t::constant(1.), p_t::constant(2.)};

    CHECK(intersection.test(r, a));

    p_t ip{2., 1.};
    CHECK_P(intersection(r, a), ip);
  }

  return test::result();
}
