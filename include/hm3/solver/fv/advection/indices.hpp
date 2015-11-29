#pragma once
/// \file
///
///
#include <hm3/types.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace advection {

template <uint_t Nd>  //
struct indices {
  static constexpr suint_t nvars() noexcept { return 1; }
  static constexpr auto variables() noexcept { return view::iota(0, nvars()); }
  static constexpr sidx_t marker() noexcept { return 0; }
};

}  // namespace advection
}  // namespace fv
}  // namespace solver
}  // namespace hm3
