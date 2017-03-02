#pragma once
/// \file
///
/// Are two numbers approximately equal
#include <hm3/geometry/tolerance.hpp>
#include <hm3/utility/math.hpp>
#include <type_traits>

namespace hm3::geometry {

namespace approx_number_detail {

struct approx_number_fn {
  template <typename T>
  constexpr bool operator()(T a, T b, T abs_tol, T rel_tol) const noexcept {
    static_assert(std::is_floating_point<uncvref_t<T>>{});
    return math::abs(a - b)
           <= math::max(abs_tol,
                        rel_tol * math::max(math::abs(a), math::abs(b)));
  }
};

}  // namespace direction_detail

namespace {
static constexpr auto const& approx_number = static_const<
 with_default_tolerance<approx_number_detail::approx_number_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
