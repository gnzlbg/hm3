#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

template <uint_t Nd> struct state {
  /// \name Physical constants
  ///@{
  num_t gamma;
  num_t gamma_m1;
  ///@}

  state(num_t gamma_) : gamma{gamma_}, gamma_m1{gamma_ - 1.0} {
    HM3_ASSERT(gamma > 0., "negative gamma");
    HM3_ASSERT(gamma_m1 > 0., "negative gamma-1.0");
  }
};

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
