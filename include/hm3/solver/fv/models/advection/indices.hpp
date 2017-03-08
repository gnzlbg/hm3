#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::solver::fv::advection {

template <dim_t Ad>
struct indices {
  static constexpr suint_t nvars() noexcept { return 1; }
  static constexpr auto variables() noexcept { return view::iota(0, nvars()); }
  static constexpr sidx_t marker() noexcept { return 0; }
};

}  // namespace hm3::solver::fv::advection
