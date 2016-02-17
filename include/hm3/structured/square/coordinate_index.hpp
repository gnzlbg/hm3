#pragma once
/// \file
///
/// Coordinate / Index pair
#include <hm3/structured/square/coordinate.hpp>
#include <hm3/structured/square/index.hpp>

namespace hm3 {
namespace structured {
namespace square {

template <uint_t Nd, uint_t Nic, uint_t Nhl>  //
struct cidx {
  using idx_t = idx<Nd, Nic, Nhl>;
  using x_t   = coordinate<Nd, Nic, Nhl>;

  idx_t idx;
  x_t x;

  cidx(idx_t idx_) : idx(idx_), x(std::move(idx_)) {}
  cidx(x_t x_) : idx(x_.index()), x(std::move(x_)) {}

  explicit operator idx_t() const noexcept { return idx; }
  explicit operator x_t() const noexcept { return x; }
};

}  // namespace square
}  // namespace structured
}  // namespace hm3
