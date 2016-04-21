#pragma once
/// \file
///
/// Adaptive mesh refinement action
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace grid {
namespace hierarchical {
namespace amr {

/// Adaptive mesh refinement action
///
/// - none: do nothing
/// - refine: refine a node
/// - coarsen: coarsen a node
enum class action : char { coarsen = -1, none = 0, refine = 1 };

struct combine_fn {
  action operator()(action a, action b) const noexcept {
    if (a == action::refine || b == action::refine) { return action::refine; }
    if (a == action::coarsen and b == action::coarsen) {
      return action::coarsen;
    }
    return action::none;
  }
};

/// Combines amr::actions
///
/// If one of the actions is refine => refine,
/// If both actions are coarsen => coarsen,
/// Otherwise => none.
namespace {
constexpr auto&& combine = static_const<combine_fn>::value;
}  // namespace

}  // namespace amr
}  // namespace hierarchical
}  // namespace grid
}  // namespace hm3
