#include <hm3/utility/vector.hpp>

#include <hm3/geometry/algorithm/sd_intersection/polygon.hpp>
#include <hm3/geometry/algorithm/sd_split/polygon.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/sd/combinators.hpp>
#include <hm3/geometry/sd/edge.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  static constexpr dim_t nd = 2;
  using plg_t               = geometry::small_polygon<nd, 10>;
  using pll_t               = geometry::small_polyline<nd, 4>;
  using vpll_t              = vector<pll_t>;
  using b_t                 = geometry::box<nd>;
  using p_t                 = geometry::point<nd>;
  using e_t                 = geometry::sd::fixed_edge<nd>;
  using vps_t               = vector<p_t>;
  using geometry::sd_intersection;

  auto pl0 = plg_t(b_t(p_t::constant(0.5), 1.0));

  using r_t = decltype(sd_intersection(pl0, e_t{}));

  auto mpl = [&](auto&&... args) {
    return r_t{geometry::polygon_primitive::sd_intersection_result_t<nd>{
     vps_t(), vpll_t({args...})}};
  };

  auto mpp = [&](auto&&... args) {
    return r_t{geometry::polygon_primitive::sd_intersection_result_t<nd>{
     vps_t({args...}), vpll_t()}};
  };

  {  // right pointing plane at box center
    auto e    = e_t({.5, .5}, {1.0, 0.0});
    auto pll0 = pll_t({p_t{0.5, 1.0}, p_t{0.5, 0.0}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = plg_t({p_t{0.5, 1.}, p_t{0., 1.}, p_t{0.0, 0.0}, p_t{0.5, 0.}});
    auto plg1 = plg_t({p_t{0.5, 0.}, p_t{1.0, 0.}, p_t{1., 1.}, p_t{0.5, 1.}});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // top pointing plane at box center
    auto e    = e_t({.5, .5}, {0.0, 1.0});
    auto pll0 = pll_t({p_t{0.0, 0.5}, p_t{1.0, 0.5}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = plg_t({p_t{0., .5}, p_t{0., 0.}, p_t{1.0, 0.0}, p_t{1., .5}});
    auto plg1 = plg_t({p_t{1., 0.5}, p_t{1., 1.}, p_t{0., 1.}, p_t{0., .5}});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // 45deg plane at box center pointing into TL corner
    auto e    = e_t({.5, .5}, {1.0, -1.0});
    auto pll0 = pll_t({p_t{1.0, 1.0}, p_t{0.0, 0.0}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = plg_t({p_t{0., 0.}, p_t{1., 0.}, p_t{1., 1.}});
    auto plg1 = plg_t({p_t{1., 1.}, p_t{0., 1.}, p_t{0., 0.}});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // 45 degree plane displaced by a delta = 0.25 pointing into the BL corner
    auto e    = e_t({.25, .25}, {-1.0, -1.0});
    auto pll0 = pll_t({p_t{0.0, 0.5}, p_t{0.5, 0.0}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = plg_t({p_t{0., .5}, p_t{0., 0.}, p_t{.5, 0.0}});
    auto plg1 = plg_t(
     {p_t{0.5, 0.}, p_t{1.0, 0.}, p_t{1., 1.}, p_t{0., 1.}, p_t{0., 0.5}});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // 45 degree plane displaced by a delta = 0.25 pointing into the BR corner
    auto e    = e_t({.75, .25}, {1.0, -1.0});
    auto pll0 = pll_t({p_t{1., 0.5}, p_t{0.5, 0.0}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = plg_t(
     {p_t{1., .5}, p_t{1., 1.}, p_t{0., 1.}, p_t{0., 0.}, p_t{0.5, 0.}});
    auto plg1 = plg_t({p_t{0.5, 0.}, p_t{1.0, 0.}, p_t{1., .5}});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // 45 degree plane displaced by a delta = 0.25 pointing into the TR corner
    auto e    = e_t({.75, .75}, {.5, .5});
    auto pll0 = pll_t({p_t{0.5, 1.}, p_t{1., 0.5}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = plg_t(
     {p_t{0.5, 1.}, p_t{0., 1.}, p_t{0.0, 0.0}, p_t{1., 0.}, p_t{1., 0.5}});
    auto plg1 = plg_t({p_t{1., 0.5}, p_t{1.0, 1.}, p_t{0.5, 1.}});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // 45 degree plane displaced by a delta = 0.25 pointing into the TL corner
    auto e    = e_t({.25, .75}, {-.5, .5});
    auto pll0 = pll_t({p_t{0., .5}, p_t{0.5, 1.}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = plg_t(
     {p_t{0., 0.5}, p_t{0., 0.}, p_t{1., 0.}, p_t{1., 1.}, p_t{0.5, 1.}});
    auto plg1 = plg_t({p_t{0.5, 1.}, p_t{0.0, 1.}, p_t{0., 0.5}});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // plane aligned with the left axis: L
    auto e    = e_t({.0, .5}, {1., .0});
    auto pll0 = pll_t({p_t{0., 1.}, p_t{0., 0.}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // plane aligned with the right axis: R
    auto e    = e_t({1., .5}, {1., .0});
    auto pll0 = pll_t({p_t{1., 0.}, p_t{1., 1.}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // plane aligned with the top axis: T
    auto e    = e_t({.5, 1.}, {0., 1.});
    auto pll0 = pll_t({p_t{1., 1.}, p_t{0., 1.}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // plane aligned with the bottom axis: B
    auto e    = e_t({.5, 0.}, {0., 1.});
    auto pll0 = pll_t({p_t{0., 0.}, p_t{1., 0.}});
    CHECK(sd_intersection(pl0, e) == mpl(pll0));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // plane at TR:
    auto e = e_t({1., 1.}, {1., 1.});
    CHECK(sd_intersection(pl0, e) == mpp(p_t{1., 1.}));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // plane at TL:
    auto e = e_t({0., 1.}, {1., -1.});
    CHECK(sd_intersection(pl0, e) == mpp(p_t{0., 1.}));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // plane at BR:
    auto e = e_t({1., 0.}, {1., -1.});
    CHECK(sd_intersection(pl0, e) == mpp(p_t{1., 0.}));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // plane at BL:
    auto e = e_t({0., 0.}, {-1., -1.});
    CHECK(sd_intersection(pl0, e) == mpp(p_t{0., 0.}));

    auto sr = sd_split(pl0, e);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // two planes B and L (corner-like sdf)
    auto e0         = e_t({.5, 0.}, {0., 1.});  // B
    auto e1         = e_t({.0, .5}, {1., .0});  // L
    auto corner_sdf = geometry::sd::neg_union(e0, e1);

    auto pll0 = pll_t({p_t{0., 1.}, p_t{0., 0.}, p_t{1., 0.}});
    CHECK(sd_intersection(pl0, corner_sdf) == mpl(pll0));

    auto sr = sd_split(pl0, corner_sdf);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // two planes BL and TR (parallel-like sdf)
    auto e0           = e_t({.0, 0.}, {1., 1.});    // BL
    auto e1           = e_t({1., 1.}, {-1., -1.});  // TR
    auto parallel_sdf = geometry::sd::neg_union(e0, e1);

    CHECK(sd_intersection(pl0, parallel_sdf) == mpp(p_t{0., 0.}, p_t{1., 1.}));

    auto sr = sd_split(pl0, parallel_sdf);
    CHECK(sr.size() == 1_u);

    auto plg0 = plg_t({p_t{0.0, 0.}, p_t{1., 0.}, p_t{1.0, 1.0}, p_t{0., 1.}});
    CHECK(sr[0] == plg0);
  }

  {  // two planes BL and TR (parallel-like sdf) shifted by 0.25 inside
    auto e0           = e_t({.25, .25}, {1., 1.});    // BL
    auto e1           = e_t({.75, .75}, {-1., -1.});  // TR
    auto parallel_sdf = geometry::sd::neg_union(e0, e1);

    auto pll0 = pll_t({p_t{0.0, 0.5}, p_t{0.5, 0.0}});  // BL
    auto pll1 = pll_t({p_t{0.5, 1.}, p_t{1., 0.5}});
    CHECK(sd_intersection(pl0, parallel_sdf) == mpl(pll0, pll1));

    auto sr = sd_split(pl0, parallel_sdf);
    CHECK(sr.size() == 3_u);

    auto plg0 = plg_t({p_t{0., 0.5}, p_t{0., 0.}, p_t{0.5, 0.0}});
    auto plg1 = plg_t({p_t{.5, 1.}, p_t{0., 1.}, p_t{0., .5}, p_t{0.5, 0.},
                       p_t{1., 0.}, p_t{1., 0.5}});
    auto plg2 = plg_t({p_t{1., 0.5}, p_t{1., 1.}, p_t{.5, 1.}});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
    CHECK(sr[2] == plg2);
  }

  {  // square like sdf
  }

  return test::result();
}
