#pragma once
/// \file
///
/// Square structured grid tile
#include <hm3/structured/square/coordinate.hpp>

namespace hm3 {
namespace structured {
namespace square {

template <suint_t Nd, suint_t Nic, suint_t Nhl>  //
struct tile {
  using coordinate_t = coordinate<Nd, Nic, Nhl>;
  coordinate_t min, max;

  tile(coordinate_t min_, coordinate_t max_) : min(min_), max(max_) {
    // TODO: validation e.g. is tile within grid
  }
};

}  // namespace square
}  // namespace structured
}  // namespace hm3
