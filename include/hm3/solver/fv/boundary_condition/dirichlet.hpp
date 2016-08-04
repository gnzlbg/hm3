// #pragma once
// /// \file

// namespace hm3 {
// namespace solver {
// namespace fv {

// struct dirichlet {
//   Gradient boundary_cell_gradient(CellValue v_c, CellCenter x_c,
//                                   SurfaceValue v_s, Surface s) {
//     // compute the corrected position of the cell center:

//     // add its position and value to the interpolation stencil
//     vector<points_t> xs;
//     vector<values> vs;

//     // compute the cut point in normal direction from corrected cell center
//     // and surface

//     // add values of all centers of neighboring cells and surface cutpoints:
//     // create interpolant:

//     // compute gradient of the interpolant at the cell center on each spatial
//     // direction:
//   }
// };

// struct neumann {
//   Gradient boundary_cell_gradients(...) {
//     // compute the corrected position of the cell center

//     // add values of all corrected neighbor cells cell surface centroids +
//     // corrected centers of
//     // neighboring cells
//     vector<point_t> xs;
//     vector<values> vs;

//     // put the gradient of each surface at the corrected cell center, and
//     // extrapolate
//   }
// };

// }  // namespace fv
// }  // namespace solver
// }  // namespace hm3
