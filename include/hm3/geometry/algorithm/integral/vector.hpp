#pragma once
/// \file
///
/// Integral of a Vector.
#include <hm3/geometry/concept/vector.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace integral_vector_detail {

struct integral_vector_fn {
  template <typename T>
  constexpr associated::num_type_t<T> operator()(
   T&& t, trait::path_integral<T>) const {
    static_assert(Vector<uncvref_t<T>>{});
    return t().norm();
  }
};

}  // namespace integral_vector_detail

namespace {
constexpr auto const& integral_vector
 = static_const<integral_vector_detail::integral_vector_fn>::value;
}  // namespace

}  // namespace hm3::geometry
