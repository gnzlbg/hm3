#pragma once
#ifdef HM3_ENABLE_VTK
/// \file
///
/// VTK Reader factory for grid types
///
/// \todo Reader for a grid::hc::single is missing
#include <hm3/vis/vtk/readers/grid/tree.hpp>
// #include <hm3/vis/vtk/readers/grid/single.hpp>
#include <hm3/vis/vtk/readers/grid/multi.hpp>

namespace hm3 {
namespace vis {
namespace vtk {
namespace grid {

/// Is the block b readable by the grid readers?
inline bool is_readable(io::json const& b) {
  string type_str = io::read_field(b, "type", HM3_AT_);
  if (type_str == tree_type()) {
    return true;
    // } else if (type_str == single_type()) {
    //   return true;
  } else if (type_str == multi_type()) {
    return true;
  } else {
    return false;
  }
}

/// Creates a grid reader from a json block of grid type
inline auto make(io::json b) -> std::unique_ptr<::hm3::vis::vtk::reader> {
  string type_str = io::read_field(b, "type", HM3_AT_);
  if (type_str == tree_type()) {
    return make_tree(b);
    // } else if (type_str == single_type()) {
    //   return make_single(b);
  } else if (type_str == multi_type()) {
    return make_multi(b);
  } else {
    HM3_FATAL_ERROR("Trying to make a grid of unknown type \"{}\"\n\n"
                    "Block dump:\n\n{}",
                    type_str, b.get<string>());
  }
}

}  // namespace grid
}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif  // HM3_ENABLE_VTK
