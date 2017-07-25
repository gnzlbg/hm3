#include <hm3/solver/geometry/surface/boundary_cell.hpp>
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
    solver::geometry::vtk::serialize(mesh, "discrete_domain_1d");
#endif

    namespace bc = boundary_cell;

    using bcsurface    = bc::surface<ad>;
    using isurface = typename bcsurface::internal_surface_t;
    using bsurface = typename bcsurface::boundary_surface_t;

    isurface L{p_t{-1.}, 2};
    isurface R{p_t{+1.}, 4};
    isurface is[2] = {L, R};
  }

  return 0;
}
