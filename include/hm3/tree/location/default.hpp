#pragma once
/// \file
///
/// Default location to use
#include <hm3/tree/location/slim.hpp>
namespace hm3 {
namespace tree {

namespace location {

template <uint_t Nd, typename T = uint_t> using default_location = slim<Nd, T>;

}  // namespace location

template <uint_t Nd, typename T = uint_t>
using loc_t = location::default_location<Nd, T>;

}  // namespace tree
}  // namespace hm3
