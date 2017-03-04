#pragma once
/// \file
///
/// AMR target utilities

namespace hm3::grid::hierarchical::amr {

template <typename T>
using amr_node_idx_t = typename T::amr_node_idx;

/// Implements an empty log for a non-loggable amr target
struct non_loggable {
  template <typename... Args>
  static void log(Args&&...) noexcept {}
};

}  // namespace hm3::grid::hierarchical::amr
