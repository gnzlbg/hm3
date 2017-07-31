#pragma once
/// \file
///
/// Concatenate a polyline and a segment.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/collinear.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/merge.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/tolerance.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace concatenate_segment_segment_detail {

struct concatenate_segment_segment_fn {
  /// Concatenates collinear adjacent segments:
  template <typename S0, typename S1, typename US0 = uncvref_t<S0>,
            typename US1 = uncvref_t<S1>,
            typename R   = associated::concatenated_t<US0>>
  constexpr optional<R> operator()(S0&& s0, S1&& s1, num_t abs_tol,
                                   num_t rel_tol) const noexcept {
    static_assert(Segment<US0>{});
    static_assert(Segment<US1>{});
    static_assert(
     Same<associated::concatenated_t<US0>, associated::concatenated_t<US1>>{});
    using c_t = associated::concatenated_t<US0>;
    // not adjacent -> cannot be concatenated:
    if (not approx_point(s0.x(1), s1.x(0), abs_tol, rel_tol)) { return {}; }
    // if collinear and can be merged into a single segment, do that:
    if (collinear(s0, s1)) {
      auto m = merge(s0, s1);
      if (m) { return c_t{view::single(m.value())}; }
    }
    // merge them into a polyline
    return c_t{view::concat(view::single(s0), view::single(s1))};
  }
};

}  // namespace concatenate_segment_segment_detail

namespace {
constexpr auto const& concatenate_segment_segment
 = static_const<with_default_tolerance<
  concatenate_segment_segment_detail::concatenate_segment_segment_fn>>::value;
}

}  // namespace hm3::geometry
