/// \file
///
/// Geometry box tests
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/utility/test.hpp>
// clang-format off
#include "aabb.hpp"
// clang-format on

using namespace hm3;

#define UNWRAP(...) __VA_ARGS__

#define CHECK_UNIT_BOX(nd_, pL_, pR_, xc_, vertices_)                      \
  do {                                                                     \
    using namespace geometry;                                              \
    static constexpr uint_t nd = nd_;                                      \
    constexpr auto pL          = point<nd> UNWRAP pL_;                     \
    constexpr auto pR          = point<nd> UNWRAP pR_;                     \
                                                                           \
    constexpr auto xc  = point<nd> UNWRAP xc_;                             \
    constexpr auto len = 1.;                                               \
                                                                           \
    const auto s1     = geometry::box<nd>{pL, pR};                         \
    constexpr auto s2 = geometry::box<nd>{xc, len};                        \
                                                                           \
    constexpr auto s3 = s2;                                                \
    geometry::box<nd> s4;                                                  \
    s4 = s3;                                                               \
                                                                           \
    constexpr auto s5 = geometry::box<nd>{xc, len + 1.};                   \
                                                                           \
    CHECK(s1 == s2);                                                       \
    CHECK(s1 != s5);                                                       \
                                                                           \
    const auto s1_xc     = centroid(s1);                                   \
    constexpr auto s2_xc = centroid(s2);                                   \
                                                                           \
    const auto s1_v     = volume(s1);                                      \
    constexpr auto s2_v = volume(s2);                                      \
                                                                           \
    STATIC_CHECK(dimension(s1) == nd);                                     \
    STATIC_CHECK(dimension(s2) == dimension(s1));                          \
    STATIC_CHECK(dimension(s2) == dimension(s3));                          \
                                                                           \
    STATIC_CHECK(length(s2) == 1.);                                        \
    CHECK(math::approx(length(s1), 1.));                                   \
                                                                           \
    CHECK(s2_xc() == xc());                                                \
    CHECK(s1_xc() == xc());                                                \
                                                                           \
    CHECK(math::approx(s1_v, 1.));                                         \
    STATIC_CHECK(s2_v == 1.);                                              \
                                                                           \
    constexpr hm3::array<point<nd>, math::ipow(2_u, nd)> vertices{         \
     {UNWRAP vertices_}};                                                  \
    CHECK(size(vertices) == size(geometry::vertices(s1)));                 \
                                                                           \
    CHECK(equal(vertices, geometry::vertices(s1), [](auto i, auto j) {     \
      return equal(i, j,                                                   \
                   [](auto xi, auto xj) { return math::approx(xi, xj); }); \
    }));                                                                   \
  } while (false)

int main() {
  CHECK_UNIT_BOX(1, ({0.}), ({1.}), ({0.5}), ({0.}, {1.}));
  CHECK_UNIT_BOX(2, ({0., 0.}), ({1., 1.}), ({0.5, 0.5}),
                 ({0., 0.}, {1., 0.}, {1., 1.}, {0., 1.}));
  CHECK_UNIT_BOX(3, ({0., 0., 0.}), ({1., 1., 1.}), ({0.5, 0.5, 0.5}),
                 ({0., 0., 0.}, {1., 0., 0.}, {1., 1., 0.}, {0., 1., 0.},
                  {0., 0., 1.}, {1., 0., 1.}, {1., 1., 1.}, {0., 1., 1.}));

#define CHECK_CORNER_POS(p, v0, v1, v2)                                        \
  STATIC_CHECK(geometry::aabb_detail::relative_vertex_position(p, 0) == (v0)); \
  STATIC_CHECK(geometry::aabb_detail::relative_vertex_position(p, 1) == (v1)); \
  STATIC_CHECK(geometry::aabb_detail::relative_vertex_position(p, 2) == (v2)); \
  CHECK(equal(geometry::aabb_detail::relative_vertex_position<3>(p),           \
              geometry::point<3>{{(v0), (v1), (v2)}},                          \
              [](auto i, auto j) { return math::approx(i, j); }))

  CHECK_CORNER_POS(0, -1, -1, -1);
  CHECK_CORNER_POS(1, +1, -1, -1);
  CHECK_CORNER_POS(2, +1, +1, -1);
  CHECK_CORNER_POS(3, -1, +1, -1);
  CHECK_CORNER_POS(4, -1, -1, +1);
  CHECK_CORNER_POS(5, +1, -1, +1);
  CHECK_CORNER_POS(6, +1, +1, +1);
  CHECK_CORNER_POS(7, -1, +1, +1);

  {  // bounds
    using p2d = geometry::point<2>;
    auto s2d  = geometry::unit(geometry::box<2>{});
    auto bs   = geometry::bounds(s2d);
    CHECK(bs.min == p2d{p2d::c(0.)});
    CHECK(bs.max == p2d{p2d::c(1.)});
  }

  test_unit_aabbs<geometry::box>();

  return test::result();
}
