#pragma once
/// \file
///
///
#include <hm3/utility/inline_vector.hpp>

namespace hm3 {
namespace solver {
namespace fv {
/*
/// Types of cell
enum class cell_type : char {
internal          = 0,
boundary          = 1,
boundary_neighbor = 2,
unknown           = 3
};

template <dim_t Nd>  //
struct surface : segment<Nd> {
point<Nd> x_center;  ///< Surface centroid
num_t area;          ///< Surfae area
sidx_t neighbor;     ///< Neighbor,  if any
bc_idx bc;           ///< Boundary condition, if any
};

struct boundary_cell {
static constexpr std::size_t max_no_surfaces = 5;
sidx_t idx;          ///< Boundary cell index
num_t volume;        ///< Boundary cell volume
point<Nd> x_center;  ///< Boundary cell center
inline_vector<surface<Nd>, max_no_surfaces> surfaces;  ///< Boundary surfaces
};
*/
}  // namespace fv
}  // namespace solver
}  // namespace hm3
