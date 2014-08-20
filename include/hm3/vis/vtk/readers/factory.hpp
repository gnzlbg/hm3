#pragma once
/// \file
///
/// VTK Reader factory
#ifdef HM3_ENABLE_VTK
#include <hm3/vis/vtk/readers/grid/factory.hpp>
#include <hm3/vis/vtk/readers/solver/level_set/factory.hpp>

namespace hm3 {
namespace vis {
namespace vtk {

/// Is the block b readable?
inline bool is_readable(io::json const& b) {
  return grid::is_readable(b) || level_set::is_readable(b);
}

/// Creates a reader from a json block of a file group
inline auto make(io::json const& b)
 -> std::unique_ptr<::hm3::vis::vtk::reader> {
  string type_str = io::read_field(b, "type", HM3_AT_);
  if (grid::is_readable(b)) {
    return grid::make(b);
  } else if (level_set::is_readable(b)) {
    return level_set::make(b);
  } else {
    HM3_FATAL_ERROR("Trying to make a reader of unknown type \"{}\"\n\n"
                    "Block dump:\n\n"
                    "{}",
                    type_str, b.get<string>());
  }
}

}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
