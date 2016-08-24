#pragma once
/// \file
///
/// Maps different cell type geometries
#include <hm3/geometry/primitive/any.hpp>

namespace hm3 {
namespace vis {
namespace vtk {

/// TODO: rename to vtk geometry
/// put in .vtk type and delete this file
template <dim_t Nd>
using geometries = geometry::any<Nd>;

}  // namespace vtk
}  // namespace vis
}  // namespace hm3
