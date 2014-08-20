#pragma once
////////////////////////////////////////////////////////////////////////////////
/// \file Error-handling utilities
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <hom3/core/error.hpp>
////////////////////////////////////////////////////////////////////////////////

namespace hom3 {
namespace io {

struct error : std::runtime_error {
  using runtime_error::runtime_error;
};

}  // namespace io

}  // namespace hom3
////////////////////////////////////////////////////////////////////////////////
