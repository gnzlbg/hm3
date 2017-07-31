#pragma once
/// \file
///
///
#include <hm3/solver/geometry/types.hpp>

namespace hm3::solver::geometry {

/// Types of solver surfaces
enum class surface_t { internal, boundary, geometry };

template <typename OStream>
OStream& to_ascii(OStream& o, surface_t const& v, ascii_fmt::solver_geometry) {
  switch (v) {
    case surface_t::internal: {
      o << "internal surface";
      break;
    }
    case surface_t::boundary: {
      o << "boundary surface";
      break;
    }
    case surface_t::geometry: {
      o << "geometry (only) surface";
      break;
    }
    default: { HM3_UNREACHABLE(); }
  }
  return o;
}

// clang-format off

template <dim_t Ad>
using cell_surface_type = meta::if_c<Ad == 1, hg::point<1>, 
                          meta::if_c<Ad == 2, hg::segment<2>,
                          meta::if_c<Ad == 3, hg::polygon<3>,
                                     void>>>;

// clang-format on

}  // namespace hm3::solver::geometry
