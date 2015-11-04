#pragma once
/// \file
///
///
#include <hm3/types.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

template <uint_t Nd>  //
struct indices {
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
