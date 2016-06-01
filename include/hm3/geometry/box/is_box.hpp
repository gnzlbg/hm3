#pragma once
/// \file
///
/// Is a Box a Box?
#include <hm3/geometry/box/box.hpp>

namespace hm3 {
namespace geometry {

/// Is a Box a Box? (yes, always)
template <dim_t Nd>  //
constexpr bool is_box(box<Nd> const&) {
  return true;
}

}  // namespace geometry
}  // namespace hm3
