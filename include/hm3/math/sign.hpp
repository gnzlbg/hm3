#pragma once
/// \file
///
/// Sign and signum.
#include <cmath>
#include <hm3/math/constexpr.hpp>

namespace hm3::math {

/// Sign type (negative or positive)
struct sign_t {
  enum class type : sint_t { negative = -1, positive = 1 };
  static constexpr type negative() { return type::negative; };
  static constexpr type positive() { return type::positive; };
  type value{};

  operator type() const noexcept { return value; }
  constexpr sign_t(type t) noexcept : value{t} {};

  constexpr sign_t()              = default;
  constexpr sign_t(sign_t const&) = default;
  constexpr sign_t(sign_t&&)      = default;
  constexpr sign_t& operator=(sign_t const&) = default;
  constexpr sign_t& operator=(sign_t&&) = default;
};

constexpr bool operator==(sign_t const& a, sign_t const& b) noexcept {
  return a.value == b.value;
}

constexpr bool operator!=(sign_t const& a, sign_t const& b) noexcept {
  return !(a == b);
}

template <typename OStream>
OStream& operator<<(OStream& os, sign_t const& v) {
  switch (v.value) {
    case sign_t::type::negative: {
      os << "-";
      break;
    }
    case sign_t::type::positive: {
      os << "+";
      break;
    }
  }
  return os;
}

/// Signum type (negative, zero, or positive)
struct signum_t {
  enum class type : sint_t {
    negative = static_cast<sint_t>(sign_t::type::negative),
    zero     = 0,
    positive = static_cast<sint_t>(sign_t::type::positive)
  };
  static constexpr signum_t negative() { return type::negative; };
  static constexpr signum_t positive() { return type::positive; };
  static constexpr signum_t zero() { return type::zero; };
  type value{};

  operator type() const noexcept { return value; }
  constexpr signum_t(type t) noexcept : value{t} {};

  constexpr signum_t()                = default;
  constexpr signum_t(signum_t const&) = default;
  constexpr signum_t(signum_t&&)      = default;
  constexpr signum_t& operator=(signum_t const&) = default;
  constexpr signum_t& operator=(signum_t&&) = default;
};

constexpr bool operator==(signum_t const& a, signum_t const& b) noexcept {
  return a.value == b.value;
}

constexpr bool operator!=(signum_t const& a, signum_t const& b) noexcept {
  return !(a == b);
}

template <typename OStream>
OStream& operator<<(OStream& os, signum_t const& v) {
  switch (v.value) {
    case signum_t::type::negative: {
      os << "-";
      break;
    }
    case signum_t::type::positive: {
      os << "+";
      break;
    }
    case signum_t::type::zero: {
      os << "0";
      break;
    }
  }
  return os;
}

/// Returns the signum of \p x, i.e. -1 if x < 0, +1 if x > 0, and 0
/// otherwise
template <typename T,
          CONCEPT_REQUIRES_(std::is_arithmetic<T>{} and std::is_signed<T>{})>
static constexpr signum_t signum(const T& x) noexcept {
  return static_cast<signum_t::type>((T{0} < x) - (x < T{0}));
}

/// Returns the signum of \p x, i.e. -1 if x < 0, +1 if x > 0, and 0
/// otherwise
template <typename T,
          CONCEPT_REQUIRES_(std::is_arithmetic<T>{} and !std::is_signed<T>{})>
static constexpr signum_t signum(const T& x) noexcept {
  return static_cast<signum_t::type>(T{0} < x);
}

/// Sign of a floating point number (doesn't handle NaNs)
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr sign_t fast_sign(Float f) noexcept {
  return (f < Float{0}) ? sign_t::negative() : sign_t::positive();
}

/// Sign of a floating point number (handles NaNs)
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr sign_t robust_sign(Float f) noexcept {
  return std::signbit(f) ? sign_t::negative() : sign_t::positive();
}

/// Sign of a floating point number
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr sign_t sign(Float f) noexcept {
  HM3_ASSERT(!is_nan(f), "");
  return fast_sign(f);
}

}  // namespace hm3::math
