#pragma once
/// \file
///
/// Default location to use
#include <hm3/grid/hierarchical/tree/location/interleaved.hpp>
namespace hm3::tree {

namespace location {

template <dim_t Ad, typename T = uint_t>
using default_location = interleaved<Ad, T>;

}  // namespace location

template <dim_t Ad, typename T = uint_t>
using loc_t = location::default_location<Ad, T>;

}  // namespace hm3::tree
