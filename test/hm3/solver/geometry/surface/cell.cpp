#include <hm3/solver/geometry/surface/boundary_cell.hpp>
#include <hm3/utility/test.hpp>

template struct hm3::solver::geometry::boundary_cell_surface<1>;
template struct hm3::solver::geometry::boundary_cell_surface<2>;
template struct hm3::solver::geometry::boundary_cell_surface<3>;

using namespace hm3;

int main() {
  {  // 1D
    static constexpr dim_t ad = 1;

    using s_t = solver::geometry::boundary_cell_surface<ad>;

    static_assert(geometry::AmbientDimension<s_t>{});
    static_assert(geometry::AmbientDimension<s_t, ad>{});

    static_assert(geometry::ElementDimension<s_t>{});
    static_assert(geometry::ElementDimension<s_t, ad>{});
    static_assert(geometry::ElementDimension<s_t, ad, 0>{});

    static_assert(geometry::GeometryObject<s_t>{});
    static_assert(geometry::GeometryObject<s_t, ad>{});
    static_assert(geometry::GeometryObject<s_t, ad, 0>{});

    static_assert(ranges::SemiRegular<s_t>{});
    static_assert(ranges::Regular<s_t>{});

    static_assert(geometry::VertexAccess<s_t>{});
    static_assert(geometry::Vertex<s_t>{});
    static_assert(geometry::Vertex<s_t, ad>{});

    static_assert(geometry::Point<s_t>{});
    static_assert(geometry::Point<s_t, ad>{});
    static_assert(geometry::Simplex<s_t>{});
  }

  {  // 2D
    static constexpr dim_t ad = 2;

    using s_t = solver::geometry::boundary_cell_surface<ad>;

    static_assert(geometry::AmbientDimension<s_t>{});
    static_assert(geometry::AmbientDimension<s_t, ad>{});

    static_assert(geometry::ElementDimension<s_t>{});
    static_assert(geometry::ElementDimension<s_t, ad>{});
    static_assert(geometry::ElementDimension<s_t, ad, 1>{});

    static_assert(geometry::GeometryObject<s_t>{});
    static_assert(geometry::GeometryObject<s_t, ad>{});
    static_assert(geometry::GeometryObject<s_t, ad, 1>{});

    static_assert(!geometry::Line<s_t>{});
    static_assert(!geometry::Line<s_t, ad>{});

    static_assert(geometry::Segment<s_t>{});
    static_assert(geometry::Segment<s_t, ad>{});

    static_assert(geometry::Simplex<s_t>{});
  }

  {  // 3D
    static constexpr dim_t ad = 3;

    using s_t = solver::geometry::boundary_cell_surface<ad>;

    static_assert(geometry::AmbientDimension<s_t>{});
    static_assert(geometry::AmbientDimension<s_t, ad>{});

    static_assert(geometry::ElementDimension<s_t>{});
    static_assert(geometry::ElementDimension<s_t, ad>{});
    static_assert(geometry::ElementDimension<s_t, ad, 2>{});

    static_assert(geometry::GeometryObject<s_t>{});
    static_assert(geometry::GeometryObject<s_t, ad>{});
    static_assert(geometry::GeometryObject<s_t, ad, 2>{});

    static_assert(geometry::Polygon<s_t>{});
    static_assert(geometry::Polygon<s_t, ad>{});
  }

  return 0;
}
