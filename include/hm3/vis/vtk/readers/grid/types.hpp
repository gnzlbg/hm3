#pragma once
/// \file
///
/// VTK Grid read types
#ifdef HM3_ENABLE_VTK
#include <hm3/grid/hc/multi.hpp>
#include <hm3/grid/hc/single.hpp>
#include <hm3/grid/hierarchical/tree/tree.hpp>
#include <hm3/grid/types.hpp>

namespace hm3 {
namespace vis {
namespace vtk {
namespace grid {

inline string tree_type() { return type(tree::tree<1>{}); }
inline string single_type() { return type(::hm3::grid::hc::single<1>{}); }
inline string multi_type() { return type(::hm3::grid::hc::multi<1>{}); }

}  // namespace grid
}  // namespace vtk
}  // namespace vis
}  // namespace hm3
#endif
