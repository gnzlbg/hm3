#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

template <dim_t Nd>
struct indices {
  static constexpr suint_t nvars() noexcept { return Nd + 2; }
  static constexpr auto variables() noexcept { return view::iota(0, nvars()); }
  /// \name Conservative variables
  ///@{
  static constexpr sidx_t rho_u(sidx_t d) noexcept { return d; }
  static constexpr sidx_t rho() noexcept { return Nd; }
  static constexpr sidx_t rho_E() noexcept { return Nd + 1; }
  ///@}  // Convervative variables

  /// \name Primitive variables
  ///@{

  // Note: primitive rho() == convervative rho()

  static constexpr sidx_t u(sidx_t d) noexcept { return d; }
  static constexpr sidx_t p() noexcept { return Nd + 1; }

  ///@}  // Primitive variables
};

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
