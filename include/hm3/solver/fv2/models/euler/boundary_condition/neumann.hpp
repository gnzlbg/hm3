#pragma once
/// \file
///
/// Neumann Boundary Condition

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

struct neumann {
  template <typename State, typename Block, typename CIdx, typename Surface>
  void rhs(){};

  template <typename State, typename Block, typename CIdx, typename Surface>
  void surface_gradients(){};
};

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
