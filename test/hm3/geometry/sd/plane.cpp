#include <hm3/geometry/sd/functions/plane.hpp>
#include <hm3/utility/test.hpp>
// triangle stuff:
#include <hm3/geometry/sd/functions/triangle.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  {  // 1D
    static constexpr dim_t ad = 1;
    using p_t                 = point<ad>;
    using v_t                 = vec<ad>;
    using e_t                 = sd::plane_at_point<ad>;

    static_assert(SignedDistance<e_t, p_t>{});

    p_t o  = {2.0};
    v_t np = {1.0};
    v_t nm = {-1.0};

    p_t x0 = o;
    p_t xp = {4.0};
    p_t xm = {-1.0};

    auto fep = e_t(o, np);
    auto fem = e_t(o, nm);
    CHECK(fep(x0) == 0.0);
    CHECK(fep(xp) == 2.0);
    CHECK(fep(xm) == -3.0);

    CHECK(fem(x0) == 0.0);
    CHECK(fem(xp) == -2.0);
    CHECK(fem(xm) == 3.0);
  }

  {  // 2D
    static constexpr dim_t ad = 2;
    using p_t                 = point<ad>;
    using v_t                 = vec<ad>;
    using e_t                 = sd::plane_at_point<ad>;
    using re_t                = sd::rotated_plane_at_point<ad>;

    static_assert(SignedDistance<e_t, p_t>{});

    p_t o  = {2.0, 2.0};
    v_t np = {1. / std::sqrt(2.), 1. / std::sqrt(2.)};
    v_t nm = {-1. / std::sqrt(2.), -1. / std::sqrt(2.)};

    p_t x0 = o;
    p_t xp = {4.0, 4.0};
    p_t xm = {0., 0.};

    auto fep = e_t(o, np);
    auto fem = e_t(o, nm);
    CHECK(fep(x0) == 0.0);
    CHECK(fep(xp) == 2.0 * std::sqrt(2.));
    CHECK(fep(xm) == -2.0 * std::sqrt(2.));

    CHECK(fem(x0) == 0.0);
    CHECK(fem(xp) == -2.0 * std::sqrt(2.));
    CHECK(fem(xm) == 2.0 * std::sqrt(2.));

    auto rep = re_t(o, math::pi / 4., v_t::unit(0));
    CHECK(rep(x0) == 0.0);
    CHECK(rep(xp) == 2.0 * std::sqrt(2.));
    CHECK(rep(xm) == -2.0 * std::sqrt(2.));
  }

  // TODO: 3D

  // TODO: move triangle tests to their own file
  {  // triangle 2D
    using p2d_t      = hm3::geometry::point<2>;
    p2d_t tri_vxs[3] = {p2d_t{-1., 0.}, p2d_t{1., 0.0}, p2d_t{0., 1.0}};
    geometry::triangle<2> tri{make_segment_range(tri_vxs)};
    geometry::sd::triangle tri0{tri};

    // vertices:
    CHECK(tri0(tri_vxs[0]) == 0.);
    CHECK(tri0(tri_vxs[1]) == 0.);
    CHECK(tri0(tri_vxs[2]) == 0.);
    // edge mid points:
    CHECK(tri0(p2d_t{0., 0.0}) == 0.);
    CHECK(geometry::approx(tri0(p2d_t{-0.5, 0.5}), 0.));
    CHECK(geometry::approx(tri0(p2d_t{0.5, 0.5}),0.));
    // across bottom edge:
    CHECK(tri0(p2d_t{0., 0.1}) == -0.1);
    CHECK(tri0(p2d_t{0., -0.25}) == +0.25);
    // outside the triangle along a plane:
    CHECK(tri0(p2d_t{-1., 2.}) == std::sqrt(2.));
  }

  return test::result();
}
