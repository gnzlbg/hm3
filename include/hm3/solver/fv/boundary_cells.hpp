// #pragma once
// /// \file
// ///
// ///
// #include <hm3/utility/fixed_capacity_vector.hpp>

// namespace hm3 {
// namespace solver {
// namespace fv {

// template <dim_t Ad>
// struct surface {
//   point<Ad> x_center;       ///< Surface centroid
//   num_t area;               ///< Surfae area
//   vec<Ad> normal;           ///< Surface normal
//   block_cell_idx neighbor;  ///< Neighbor,  if any
//   bc_idx bc;                ///< Boundary condition, if any
// };

// template <dim_t Ad>
// struct cut_cell {
//   static constexpr std::size_t max_no_surfaces = 5;
//   block_cell_idx idx;  ///< Cut-cell index
//   num_t volume;        ///< Cut-cell volume
//   point<Ad> x_center;  ///< Cut-cell center
//   polygon<Ad> geometry;
//   fixed_capacity_vector<surface<Ad>, max_no_surfaces>
//    surface_data;  ///< Cut-cell surfaces
// };

// template <typename State, typename Block, typename CIdx, typename BCs>
// cut_cell<Ad> cut_cell(State& s, Block& b, CIdx c, BCs&& bcs) {
//   // initialize with uncut geometry:
//   cut_cell<Ad> cell.geometry = b.geometry(c);

//   // for each boundary condition:
//   // - cut the geometry

//   // for each surface in the geometry:
//   // - if is on zero level of some bc, it is a boundary surface
//   // - otherwise it is a surface with a neighbor
// }
// };

// }  // namespace fv
// }  // namespace solver
// }  // namespace hm3
