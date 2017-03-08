#include <hm3/geometry/algorithm/sd_intersection.hpp>
#include <hm3/geometry/algorithm/sd_split.hpp>
#include <hm3/geometry/primitives.hpp>
#include <hm3/geometry/sd.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;

int main() {
  static constexpr dim_t ad = 2;
  using plg_t               = polygon<ad>;
  using pll_t               = polyline<ad>;
  using vpll_t              = small_vector<pll_t, 4>;
  using b_t                 = box<ad>;
  using p_t                 = point<ad>;
  using v_t                 = vec<ad>;
  using e_t                 = sd::plane_at_point<ad>;
  using vps_t               = small_vector<p_t, 4>;

  auto pl0 = plg_t(edges(b_t(p_t::constant(0.5), 1.0)));

  using r_t = decltype(sd::intersection(pl0, e_t{}));
  auto mpl  = [&](auto&&... args) { return r_t{vps_t(), vpll_t({args...})}; };
  auto mpp  = [&](auto&&... args) { return r_t{vps_t({args...}), vpll_t()}; };

  auto make_pl = [](auto... vxs) {
    auto vxs0 = {vxs...};
    return pll_t(make_open_segment_range(vxs0));
  };

  auto make_pg = [](auto... vxs) {
    auto vxs0 = {vxs...};
    return plg_t(make_segment_range(vxs0));
  };

  {  // right pointing plane at box center
    auto e    = e_t(p_t{.5, .5}, v_t{1.0, 0.0});
    auto pll0 = make_pl(p_t{0.5, 0.0}, p_t{0.5, 1.0});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = make_pg(p_t{0.5, 0.}, p_t{1.0, 0.}, p_t{1., 1.}, p_t{0.5, 1.});
    auto plg1 = make_pg(p_t{0.5, 1.}, p_t{0., 1.}, p_t{0.0, 0.0}, p_t{0.5, 0.});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // top pointing plane at box center
    auto e    = e_t(p_t{.5, .5}, v_t{0.0, 1.0});
    auto pll0 = make_pl(p_t{1.0, 0.5}, p_t{0.0, 0.5});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = make_pg(p_t{0., .5}, p_t{0., 0.}, p_t{1.0, 0.0}, p_t{1., .5});
    auto plg1 = make_pg(p_t{1., 0.5}, p_t{1., 1.}, p_t{0., 1.}, p_t{0., .5});

    CHECK(sr[0] == plg1);
    CHECK(sr[1] == plg0);
  }

  {  // 45deg plane at box center pointing into TL corner
    auto e    = e_t(p_t{.5, .5}, v_t{1.0, -1.0}.normalized());
    auto pll0 = make_pl(p_t{1.0, 1.0}, p_t{0.0, 0.0});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = make_pg(p_t{0., 0.}, p_t{1., 0.}, p_t{1., 1.});
    auto plg1 = make_pg(p_t{1., 1.}, p_t{0., 1.}, p_t{0., 0.});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg1);
  }

  {  // 45 degree plane displaced by a delta = 0.25 pointing into the BL corner
    auto e    = e_t(p_t{.25, .25}, v_t{-1.0, -1.0}.normalized());
    auto pll0 = make_pl(p_t{0.5, 0.0}, p_t{0.0, 0.5});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = make_pg(p_t{0., .5}, p_t{0., 0.}, p_t{.5, 0.0});
    auto plg1 = make_pg(p_t{0.5, 0.}, p_t{1.0, 0.}, p_t{1., 1.}, p_t{0., 1.},
                        p_t{0., 0.5});

    CHECK(sr[0] == plg1);
    CHECK(sr[1] == plg0);
  }

  {  // 45 degree plane displaced by a delta = 0.25 pointing into the BR corner
    auto e    = e_t(p_t{.75, .25}, v_t{1.0, -1.0}.normalized());
    auto pll0 = make_pl(p_t{0.5, 0.0}, p_t{1., 0.5});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = make_pg(p_t{1., .5}, p_t{1., 1.}, p_t{0., 1.}, p_t{0., 0.},
                        p_t{0.5, 0.});
    auto plg1 = make_pg(p_t{0.5, 0.}, p_t{1.0, 0.}, p_t{1., .5});

    CHECK(sr[0] == plg1);
    CHECK(sr[1] == plg0);
  }

  {  // 45 degree plane displaced by a delta = 0.25 pointing into the TR corner
    auto e    = e_t(p_t{.75, .75}, v_t{.5, .5}.normalized());
    auto pll0 = make_pl(p_t{1., 0.5}, p_t{0.5, 1.});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = make_pg(p_t{0.5, 1.}, p_t{0., 1.}, p_t{0.0, 0.0}, p_t{1., 0.},
                        p_t{1., 0.5});
    auto plg1 = make_pg(p_t{1., 0.5}, p_t{1.0, 1.}, p_t{0.5, 1.});

    CHECK(sr[0] == plg1);
    CHECK(sr[1] == plg0);
  }

  {  // 45 degree plane displaced by a delta = 0.25 pointing into the TL corner
    auto e    = e_t(p_t{.25, .75}, v_t{-.5, .5}.normalized());
    auto pll0 = make_pl(p_t{0.5, 1.}, p_t{0., .5});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 2_u);

    auto plg0 = make_pg(p_t{0., 0.5}, p_t{0., 0.}, p_t{1., 0.}, p_t{1., 1.},
                        p_t{0.5, 1.});
    auto plg1 = make_pg(p_t{0.5, 1.}, p_t{0.0, 1.}, p_t{0., 0.5});

    CHECK(sr[0] == plg1);
    CHECK(sr[1] == plg0);
  }

  {  // plane aligned with the left axis: L
    auto e    = e_t(p_t{.0, .5}, v_t{1., .0});
    auto pll0 = make_pl(p_t{0., 1.}, p_t{0., 0.});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 1_u);

    CHECK(sr[0] == pl0);
  }

  {  // plane aligned with the right axis: R
    auto e    = e_t(p_t{1., .5}, v_t{1., .0});
    auto pll0 = make_pl(p_t{1., 0.}, p_t{1., 1.});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }

  {  // plane aligned with the top axis: T
    auto e    = e_t(p_t{.5, 1.}, v_t{0., 1.});
    auto pll0 = make_pl(p_t{1., 1.}, p_t{0., 1.});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }

  {  // plane aligned with the bottom axis: B
    auto e    = e_t(p_t{.5, 0.}, v_t{0., 1.});
    auto pll0 = make_pl(p_t{0., 0.}, p_t{1., 0.});
    CHECK(sd::intersection(pl0, e) == mpl(pll0));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }
  {  // plane at TR:
    auto e = e_t(p_t{1., 1.}, v_t{1., 1.}.normalized());
    CHECK(sd::intersection(pl0, e) == mpp(p_t{1., 1.}));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }

  {  // plane at TL:
    auto e = e_t(p_t{0., 1.}, v_t{1., -1.}.normalized());
    CHECK(sd::intersection(pl0, e) == mpp(p_t{0., 1.}));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }

  {  // plane at BR:
    auto e = e_t(p_t{1., 0.}, v_t{1., -1.}.normalized());
    CHECK(sd::intersection(pl0, e) == mpp(p_t{1., 0.}));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }

  {  // plane at BL:
    auto e = e_t(p_t{0., 0.}, v_t{-1., -1.}.normalized());
    CHECK(sd::intersection(pl0, e) == mpp(p_t{0., 0.}));

    auto sr = sd::split(pl0, e);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }

  {  // two planes B and L (corner-like sdf)
    auto e0         = e_t(p_t{.5, 0.}, v_t{0., 1.});  // B
    auto e1         = e_t(p_t{.0, .5}, v_t{1., .0});  // L
    auto corner_sdf = sd::neg_union(e0, e1);
    auto pll0       = make_pl(p_t{0., 1.}, p_t{0., 0.}, p_t{1., 0.});
    CHECK(sd::intersection(pl0, corner_sdf) == mpl(pll0));

    auto sr = sd::split(pl0, corner_sdf);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }

  {  // two planes BL and TR (parallel-like sdf)
    auto e0           = e_t(p_t{.0, 0.}, v_t{1., 1.}.normalized());    // BL
    auto e1           = e_t(p_t{1., 1.}, v_t{-1., -1.}.normalized());  // TR
    auto parallel_sdf = sd::neg_union(e0, e1);

    CHECK(sd::intersection(pl0, parallel_sdf) == mpp(p_t{0., 0.}, p_t{1., 1.}));

    auto sr = sd::split(pl0, parallel_sdf);
    CHECK(sr.size() == 1_u);
    CHECK(sr[0] == pl0);
  }

  {  // two planes BL and TR (parallel-like sdf) shifted by 0.25 inside
    auto e0           = e_t(p_t{.25, .25}, v_t{1., 1.}.normalized());    // BL
    auto e1           = e_t(p_t{.75, .75}, v_t{-1., -1.}.normalized());  // TR
    auto parallel_sdf = sd::neg_union(e0, e1);

    auto pll0 = make_pl(p_t{0.5, 0.0}, p_t{0.0, 0.5});  // BL
    auto pll1 = make_pl(p_t{1., 0.5}, p_t{0.5, 1.});
    CHECK(sd::intersection(pl0, parallel_sdf) == mpl(pll0, pll1));

    auto sr = sd::split(pl0, parallel_sdf);
    CHECK(sr.size() == 3_u);

    auto plg0 = make_pg(p_t{0., 0.5}, p_t{0., 0.}, p_t{0.5, 0.0});
    auto plg1 = make_pg(p_t{.5, 1.}, p_t{0., 1.}, p_t{0., .5}, p_t{0.5, 0.},
                        p_t{1., 0.}, p_t{1., 0.5});
    auto plg2 = make_pg(p_t{1., 0.5}, p_t{1., 1.}, p_t{.5, 1.});

    CHECK(sr[0] == plg0);
    CHECK(sr[1] == plg2);
    CHECK(sr[2] == plg1);
  }
  return test::result();
}
