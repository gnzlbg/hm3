#pragma once
/// \file types.hpp
#ifdef HM3_ENABLE_VTK
#include <hm3/grid/types.hpp>
#include <hm3/tree/tree.hpp>
#include <hm3/grid/hc/single.hpp>
#include <hm3/grid/hc/multi.hpp>

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
