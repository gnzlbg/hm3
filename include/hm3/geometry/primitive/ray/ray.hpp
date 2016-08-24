#pragma once
/// \file
///
/// Ray.
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/vec.hpp>

namespace hm3::geometry {

/// Ray primitive.
namespace ray_primitive {

/// Ray (unbounded)
///
/// \tparam Nd Number of spatial dimensions.
template <dim_t Nd>
struct ray : line<Nd> {
  using line<Nd>::line;
  using line<Nd>::operator=;
  using line<Nd>::origin;
  using line<Nd>::x;

  line<Nd> as_line() const noexcept {
    return static_cast<line<Nd> const&>(*this);
  }

  segment<Nd> as_segment(num_t ray_length = 1e200) const noexcept {
    return segment<Nd>::through(origin(), x(ray_length));
  }
};

/// Is the representation of the rays \p a and \p b equal?
template <dim_t Nd>
bool operator==(ray<Nd> const& a, ray<Nd> const& b) noexcept {
  return a.as_line() == b.as_line();
}

template <dim_t Nd>
bool operator!=(ray<Nd> const& a, ray<Nd> const& b) noexcept {
  return !(a == b);
}

}  // namespace ray_primitive

using ray_primitive::ray;

namespace concepts {

// A ray is infinite (unbounded).
template <dim_t Nd> struct is_bounded<ray<Nd>> : std::false_type {};
}

}  // namespace hm3::geometry
