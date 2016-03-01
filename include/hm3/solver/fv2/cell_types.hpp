#pragma once
/// \file
///
///
#include <hm3/utility/inline_vector.hpp>

namespace hm3 {
namespace solver {
namespace fv {

/// Types of cell
enum class cell_type : char {
  internal          = 0,
  boundary          = 1,
  boundary_neighbor = 2,
  unknown           = 3
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
