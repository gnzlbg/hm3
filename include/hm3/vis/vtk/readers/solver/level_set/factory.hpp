#pragma once
/// \file
///
/// VTK level-set reader factory
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/readers/solver/level_set/reader.hpp>

namespace hm3 {
namespace vis {
namespace vtk {
namespace level_set {

/// Is the block b readable by the grid readers?
inline bool is_readable(io::json const& b) {
  string type = io::read_field(b, "type", HM3_AT_);
  return type == "level_set";
}

}  // namespace level_set
}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
