#pragma once
/// \file
///
/// Heat conduction indices
#include <hm3/solver/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::solver::fv::heat {

template <dim_t Ad>
struct indices {
  static constexpr vidx_t nvars() noexcept { return 1; }
  static constexpr auto variables() noexcept { return view::iota(0, nvars()); }
  static constexpr vidx_t temperature() noexcept { return 0; }
};

}  // namespace hm3::solver::fv::heat
