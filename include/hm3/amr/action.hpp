#pragma once
/// \file action.hpp Adaptive mesh refinement action
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace amr {

/// Adaptive mesh refinement action
///
/// - none: do nothing
/// - refine: refine a node
/// - coarsen: coarsen a node
enum class action : char { coarsen = -1, none = 0, refine = 1 };

struct combine_fn {
  action operator()(action a, action b) const noexcept {
    if (a == action::refine || b == action::refine) {
      return action::refine;
    } else if (a == action::coarsen and b == action::coarsen) {
      return action::coarsen;
    } else {
      return action::none;
    }
  }
};

/// Combines amr::actions
///
/// If one of the actions is refine => refine,
/// If both actions are coarsen => coarsen,
/// Otherwise => none.
namespace {
constexpr auto&& combine = static_const<combine_fn>::value;
}

}  // namespace amr
}  // namespace hm3
