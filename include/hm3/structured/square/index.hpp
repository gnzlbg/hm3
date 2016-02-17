#pragma once
/// \file
///
/// Structured grid cell index
///
#include <hm3/types.hpp>
#include <hm3/utility/bounded.hpp>

namespace hm3 {
namespace structured {

namespace square {

/// Index of a cell within a square structured grid
template <suint_t Nd, suint_t Nic, suint_t Nhl>
using idx = bounded<sidx_t, sidx_t{0}, sidx_t{bounds<Nd, Nic, Nhl>::size()},
                    struct idx_tag>;

constexpr idx operator"" _c(unsigned long long int i) {
  return idx{static_cast<sidx_t>(i)};
}

/// Invalidates an index
///
/// \todo: since idx is a bounded type users typically expect the indices to be
/// valid so maybe a better solution is to use a compact_optional<bounded<T>>.
template <suint_t Nd, suint_t Nic, suint_t Nhl>
auto invalidate(idx<Nd, Nic, Nhl> v) {
  v.value = std::numeric_limits<sidx_t>::max();
  return v;
}

}  // namespace square
}  // namespace structured
}  // namespace hm3
