#include <hm3/solver/geometry/boundary_cell/surface.hpp>
#include <hm3/utility/test.hpp>

template struct hm3::solver::geometry::boundary_cell::surface<1>;
template struct hm3::solver::geometry::boundary_cell::surface<2>;
template struct hm3::solver::geometry::boundary_cell::surface<3>;

using namespace hm3;

namespace bc = solver::geometry::boundary_cell;

template <typename S>
void check_concepts() {
  static constexpr dim_t ad = geometry::ambient_dimension_v<S>;

  static_assert(geometry::AmbientDimension<S>{});
  static_assert(geometry::AmbientDimension<S, ad>{});

  static_assert(geometry::ElementDimension<S>{});
  static_assert(geometry::ElementDimension<S, ad>{});
  static_assert(geometry::ElementDimension<S, ad, ad - 1>{});

  static_assert(geometry::GeometryObject<S>{});
  static_assert(geometry::GeometryObject<S, ad>{});
  static_assert(geometry::GeometryObject<S, ad, ad - 1>{});

  static_assert(ranges::SemiRegular<S>{});
  static_assert(ranges::Regular<S>{});

  static_assert(geometry::Simplex<S>{});
  static_assert(geometry::VertexAccess<S>{});

  if constexpr (ad == 1) {
    static_assert(geometry::Vertex<S>{});
    static_assert(geometry::Vertex<S, ad>{});

    static_assert(geometry::Point<S>{});
    static_assert(geometry::Point<S, ad>{});
  }

  if constexpr (ad == 2) {
    static_assert(!geometry::Line<S>{});
    static_assert(!geometry::Line<S, ad>{});

    static_assert(geometry::Segment<S>{});
    static_assert(geometry::Segment<S, ad>{});
  }

  if constexpr (ad == 3) {
    static_assert(geometry::Polygon<S>{});
    static_assert(geometry::Polygon<S, ad>{});
  }

  {  // construct from internal surface
    using isurface_t = typename S::internal_surface_t;
    isurface_t isurface{};
    S s{isurface};
    CHECK(s.type() == solver::geometry::surface_t::internal);
  }
  {  // construct from boundary surface
    using bsurface_t = typename S::boundary_surface_t;
    bsurface_t bsurface{};
    S s{bsurface};
    CHECK(s.type() == solver::geometry::surface_t::boundary);
  }
  {  // construct from geometry
    using gsurface_t = typename S::surface_geometry;
    gsurface_t gsurface{};
    S s{gsurface};
    CHECK(s.type() == solver::geometry::surface_t::geometry);
  }
  {
    S s{};
    CHECK(s.type() == solver::geometry::surface_t::geometry);
  }
}

int main() {
  {  // 1D
    static constexpr dim_t ad = 1;

    using s_t = bc::surface<ad>;
    check_concepts<s_t>();
  }

  {  // 2D
    static constexpr dim_t ad = 2;

    using s_t = bc::surface<ad>;
    check_concepts<s_t>();
  }

  {  // 3D
    static constexpr dim_t ad = 3;

    using s_t = bc::surface<ad>;
    check_concepts<s_t>();
  }

  return 0;
}
