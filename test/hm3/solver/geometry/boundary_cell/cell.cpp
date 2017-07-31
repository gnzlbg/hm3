#include <hm3/solver/geometry/boundary_cell/cell.hpp>
#include <hm3/solver/geometry/domain/discrete.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace geometry;
using solver::geometry::boundary_surface;
using solver::geometry::discrete_domain;
using namespace solver::geometry;

int main() {
  {  // 1D segment
    static constexpr dim_t ad = 1;

    using bs_t = boundary_surface<ad>;
    static_assert(geometry::VertexAccess<bs_t>{});
    static_assert(geometry::Vertex<bs_t>{});
    static_assert(geometry::Point<bs_t>{});
    static_assert(Simplex<bs_t>{});

    using p_t = geometry::point<ad>;

    p_t x0 = {0.};
    p_t x1 = {2.};

    bs_t ps[2] = {bs_t{x0, 3}, bs_t{x1, 5}};

    discrete_domain<ad> mesh(ps);

#ifdef HM3_ENABLE_VTK
    solver::geometry::vtk::serialize(mesh, "cdll_discrete_domain_1d");
#endif

    namespace bc = boundary_cell;

    using bcsurface = bc::surface<ad>;
    using isurface  = typename bcsurface::internal_surface_t;
    using bsurface  = typename bcsurface::boundary_surface_t;

    // construct boundary cell from ranges of internal and boundary surfaces:
    auto xM = p_t{-1.};
    auto xP = p_t{+1.};
    isurface L0{xM, 2};
    isurface R0{xP, 4};
    isurface is0[2] = {L0, R0};

    using bcell = bc::cell<ad>;
    bcell c0{is0};

    {  // test split & clip: c0[-1, +1] | {2, 4}
      auto s = mesh.split(c0);

      CHECK(s.size() == 2_u);
      bcsurface cMs[2] = {bcsurface{L0}, bcsurface{ps[0]}};
      bcsurface cPs[2] = {bcsurface{ps[0]}, bcsurface{R0}};
      bcell cM(cMs);
      bcell cP(cPs);
      CHECK(s[0] == cM);
      CHECK(s[1] == cP);

      bcell mP(cP);
      data(mP) = bc::data{3};
      CHECK(s[1] != mP);
      data(mP) = bc::data{};
      CHECK(s[1] == mP);

      // auto c = mesh.clip(c0);
      // CHECK(c.size() == 1);
      // CHECK(c[0] == cP);
    }

    // bsurface L1{xM, 3};
    // bsurface R1{xP, 7};
    // bsurface is1[2] = {L1, R1};

    // bcell c1{is1};
  }
  return test::result();
}
