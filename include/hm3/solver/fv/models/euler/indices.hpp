#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::solver::fv::euler {

template <dim_t Ad>
struct indices {
  static constexpr suint_t nvars() noexcept { return Ad + 2; }
  static constexpr auto variables() noexcept { return view::iota(0, nvars()); }
  /// \name Conservative variables
  ///@{
  static constexpr sidx_t rho_u(sidx_t d) noexcept { return d; }
  static constexpr sidx_t rho() noexcept { return Ad; }
  static constexpr sidx_t rho_E() noexcept { return Ad + 1; }
  ///@}  // Convervative variables

  /// \name Primitive variables
  ///@{

  // Note: primitive rho() == convervative rho()

  static constexpr sidx_t u(sidx_t d) noexcept { return d; }
  static constexpr sidx_t p() noexcept { return Ad + 1; }

  ///@}  // Primitive variables
};

}  // namespace hm3::solver::fv::euler
