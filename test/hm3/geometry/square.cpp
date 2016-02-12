/// \file
///
/// Geometry square tests
#include <hm3/geometry/square.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

static_assert(Dimensional<square<1>>{}, "");
static_assert(Dimensional<square<2>>{}, "");
static_assert(Dimensional<square<3>>{}, "");

static_assert(Polygon<square<1>>{}, "");
static_assert(Polygon<square<2>>{}, "");

static_assert(Rectangle<square<1>>{}, "");
static_assert(Rectangle<square<2>>{}, "");
static_assert(Rectangle<square<3>>{}, "");

#define UNWRAP(...) __VA_ARGS__

#define CHECK_UNIT_SQUARE(nd_, pL_, pR_, xc_, corners_)                    \
  do {                                                                     \
    using namespace geometry;                                              \
    static constexpr uint_t nd = nd_;                                      \
    constexpr auto pL          = point<nd> UNWRAP pL_;                     \
    constexpr auto pR          = point<nd> UNWRAP pR_;                     \
                                                                           \
    constexpr auto xc  = point<nd> UNWRAP xc_;                             \
    constexpr auto len = 1.;                                               \
                                                                           \
    const auto s1     = square<nd>{pL, pR};                                \
    constexpr auto s2 = square<nd>{xc, len};                               \
                                                                           \
    constexpr auto s3 = s2;                                                \
    square<nd> s4;                                                         \
    s4 = s3;                                                               \
                                                                           \
    constexpr auto s5 = square<nd>{xc, len + 1.};                          \
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
    static_assert(dimension(s1) == nd, "");                                \
    static_assert(dimension(s2) == dimension(s1), "");                     \
    static_assert(dimension(s2) == dimension(s3), "");                     \
                                                                           \
    static_assert(length(s2) == 1., "");                                   \
    CHECK(math::approx(length(s1), 1.));                                   \
                                                                           \
    CHECK(s2_xc() == xc());                                                \
    CHECK(s1_xc() == xc());                                                \
                                                                           \
    CHECK(math::approx(s1_v, 1.));                                         \
    static_assert(s2_v == 1., "");                                         \
                                                                           \
    constexpr std::array<point<nd>, math::ipow(2_u, nd)> corners{          \
     {UNWRAP corners_}};                                                   \
    CHECK(size(corners) == size(geometry::corners(s1)));                   \
                                                                           \
    CHECK(equal(corners, geometry::corners(s1), [](auto i, auto j) {       \
      return equal(i, j,                                                   \
                   [](auto xi, auto xj) { return math::approx(xi, xj); }); \
    }));                                                                   \
  } while (false)

int main() {
  CHECK_UNIT_SQUARE(1, ({0.}), ({1.}), ({0.5}), ({0.}, {1.}));
  CHECK_UNIT_SQUARE(2, ({0., 0.}), ({1., 1.}), ({0.5, 0.5}),
                    ({0., 0.}, {1., 0.}, {1., 1.}, {0., 1.}));
  CHECK_UNIT_SQUARE(3, ({0., 0., 0.}), ({1., 1., 1.}), ({0.5, 0.5, 0.5}),
                    ({0., 0., 0.}, {1., 0., 0.}, {1., 1., 0.}, {0., 1., 0.},
                     {0., 0., 1.}, {1., 0., 1.}, {1., 1., 1.}, {0., 1., 1.}));

#define CHECK_CORNER_POS(p, v0, v1, v2)                                     \
  static_assert(                                                            \
   geometry::rectangle_detail::relative_corner_position(p, 0) == (v0), ""); \
  static_assert(                                                            \
   geometry::rectangle_detail::relative_corner_position(p, 1) == (v1), ""); \
  static_assert(                                                            \
   geometry::rectangle_detail::relative_corner_position(p, 2) == (v2), ""); \
  CHECK(equal(geometry::rectangle_detail::relative_corner_position<3>(p),   \
              geometry::point<3>{{(v0), (v1), (v2)}},                       \
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
    auto s2d  = geometry::unit(geometry::square<2>{});
    auto bs   = geometry::bounds(s2d);
    CHECK(bs.min == p2d{p2d::c(0.)});
    CHECK(bs.max == p2d{p2d::c(1.)});
  }

  return test::result();
}
