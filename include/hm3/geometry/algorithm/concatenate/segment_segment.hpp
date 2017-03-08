#pragma once
/// \file
///
/// Concatenate a polyline and a segment.
#include <hm3/geometry/algorithm/edge.hpp>
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
            CONCEPT_REQUIRES_(Segment<US0>{} and Segment<US1>{}
                              and Same<US0, US1>{})>
  constexpr optional<US0> operator()(S0&& s0, S1&& s1, num_t abs_tol,
                                     num_t rel_tol) const noexcept {
    // not adjacent:
    if (not approx_point(s0.x(1), s1.x(0), abs_tol, rel_tol)) { return {}; }
    // not collinear:
    if (not collinear(s0, s1)) { return {}; }
    return merge_collinear_adjacent_segments(s0, s1);
  }
};

}  // namespace concatenate_segment_segment_detail

namespace {
static constexpr auto const& concatenate_segment_segment
 = static_const<with_default_tolerance<
  concatenate_segment_segment_detail::concatenate_segment_segment_fn>>::value;
}

}  // namespace hm3::geometry
