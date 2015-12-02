#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace heat {

struct time_step_fn {
  template <typename V, typename S>
  constexpr num_t operator()(V&&, S const&, num_t length, num_t cfl) const
   noexcept {
    return cfl * 0.5 * std::pow(length, 2.);
  }
};

/// Default time step method
namespace {
constexpr auto&& time_step = static_const<time_step_fn>::value;
}  // namespace

}  // namespace heat
}  // namespace fv
}  // namespace solver
}  // namespace hm3
