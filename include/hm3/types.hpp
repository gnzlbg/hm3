#pragma once
/// \file
///
/// HM3's primitive types (these are used everywhere except when doing I/O)
#include <cstddef>
#include <string>
#include <type_traits>

#define HM3_USE_FAST_SIZED_INTS
//#define HM3_USE_SIZED_INTS
#define HM3_USE_DOUBLE_PRECISION
//#define HM3_USE_SINGLE_PRECISION

namespace hm3 {

/// TODO: put rvref where it belongs
template <typename T>
static constexpr bool rvref = std::is_rvalue_reference<T>::value;

/// \name Primitive type aliases
///@{

#if defined(HM3_USE_FAST_SIZED_INTS)
using int_t   = int_fast64_t;
using uint_t  = uint_fast64_t;
using sint_t  = int_fast32_t;
using suint_t = uint_fast32_t;
#elif defined(HM3_USE_SIZED_INTS)
using int_t   = int64_t;
using uint_t  = uint64_t;
using sint_t  = int32_t;
using suint_t = uint32_t;
#else
#pragma error "No choice of primitive integer types was specified"
#endif

using f32_t = float;
using f64_t = double;

#if defined(HM3_USE_DOUBLE_PRECISION)
using num_t = f64_t;
#elif defined(HM3_USE_SINGLE_PRECISION)
using num_t = f32_t;
#else
#pragma error "No choice of floating point precision was specified"
#endif

using dim_t  = suint_t;
using string = std::string;

/// \name Index types
///@{
using idx_t  = int_t;
using sidx_t = sint_t;
///@}  // Index types

constexpr int_t operator"" _i(unsigned long long int i) {
  return static_cast<int_t>(i);
}

constexpr uint_t operator"" _u(unsigned long long int i) {
  return static_cast<uint_t>(i);
}

constexpr suint_t operator"" _su(unsigned long long int i) {
  return static_cast<suint_t>(i);
}

constexpr std::size_t operator"" _z(unsigned long long int t) { return t; }

constexpr dim_t operator"" _d(unsigned long long int i) {
  return static_cast<dim_t>(i);
}

template <dim_t Ad>
struct dim_vt : std::integral_constant<dim_t, Ad> {};

template <dim_t Ad>
static constexpr dim_vt<Ad> dim_v{};

///@}  // Primitive type aliases

struct fallback {};

struct preferred : fallback {};

}  // namespace hm3
