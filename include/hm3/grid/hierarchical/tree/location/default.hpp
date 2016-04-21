#pragma once
/// \file
///
/// Default location to use
#include <hm3/grid/hierarchical/tree/location/interleaved.hpp>
namespace hm3 {
namespace tree {

namespace location {

template <dim_t Nd, typename T = uint_t>
using default_location = interleaved<Nd, T>;

}  // namespace location

template <dim_t Nd, typename T = uint_t>
using loc_t = location::default_location<Nd, T>;

}  // namespace tree
}  // namespace hm3
