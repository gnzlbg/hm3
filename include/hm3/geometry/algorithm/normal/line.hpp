#pragma once
/// \file
///
/// Normal of a line
#include <hm3/geometry/algorithm/normal/vector.hpp>
#include <hm3/geometry/concept/line.hpp>

namespace hm3::geometry {

namespace normal_line_detail {

struct normal_line_fn {
  /// Counter-clockwise normal vector of the line \p l.
  template <typename T>
  constexpr auto operator()(T&& t) const noexcept {
    static_assert(Line<uncvref_t<T>, 2>{});
    return normal_vector(t.direction());
  }
};

}  // namespace normal_line_detail

namespace {
constexpr auto const& normal_line
 = static_const<normal_line_detail::normal_line_fn>::value;
}  // namespace

}  // namespace hm3::geometry
