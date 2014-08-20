#pragma once
/// \file default.hpp Default location
//#include <hm3/tree/location/fast.hpp>
#include <hm3/tree/location/slim.hpp>
namespace hm3 {
namespace tree {

namespace location {

template <uint_t nd, typename T = uint_t> using default_location = slim<nd, T>;

}  // namespace location

template <uint_t nd, typename T = uint_t>
using loc_t                     = location::default_location<nd, T>;

}  // namespace tree
}  // namespace ndtree
