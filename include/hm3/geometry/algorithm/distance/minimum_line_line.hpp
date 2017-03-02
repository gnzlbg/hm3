#pragma once
/// \file
///
/// Minimum distance between two lines.
#include <hm3/geometry/algorithm/approx/line.hpp>
#include <hm3/geometry/algorithm/direction/line.hpp>
#include <hm3/geometry/algorithm/parallel/vector.hpp>

namespace hm3::geometry {

namespace minimum_distance_line_line_detail {

struct minimum_distance_line_line_fn {
  /// Minimum distance between two lines in 1D.
  template <typename L, CONCEPT_REQUIRES_(Line<L, 1>{})>
  constexpr auto operator()(L const&, L const&, num_t, num_t) const noexcept {
    return 0.;
  }

  /// Minimum distance between the line \p a and the line \p b.
  template <typename L, CONCEPT_REQUIRES_(Line<L, 2>{})>
  constexpr auto operator()(L const& a, L const& b, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    // TODO: optimize some common computation of parallel and coincidental
    if (parallel_vector(direction_line(a), direction_line(b), abs_tol,
                        rel_tol)) {
      if (coincidental_lines(a, b, abs_tol, rel_tol)) {
        return 0.;
      }  // equal => zero distance
      // parallel but not coincidental:
      auto b_o = b.origin();
      auto p   = a().projection(b_o());
      return (p - b_o()).norm();
    }
    // not parallel: intersect in one point => zero distance
    return 0.;
  }

  /// Minimum distance between the line \p a and the line \p b.
  template <typename L, CONCEPT_REQUIRES_(Line<L, 3>{})>
  constexpr auto operator()(L const& a, L const& b, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    auto da = direction_line(a);
    auto db = direction_line(b);
    if (parallel_vector(da, db, abs_tol, rel_tol)) {
      if (coincidental_lines(a, b, abs_tol, rel_tol)) {
        return 0.;
      }  // equal => zero distance
      // parallel but not coincidental:
      auto b_o = b.origin();
      auto p   = a().projection(b_o());
      return (p - b_o()).norm();
    }
    // might intersect or be skewed

    auto da_cross_db = da().cross(db());
    auto oa          = a.origin();
    auto ob          = b.origin();
    return std::abs((da_cross_db / da_cross_db.norm()).dot(oa() - ob()));
  }
};

}  // namespace minimum_distance_line_line_detail

namespace {
static constexpr auto const& minimum_distance_line_line
 = static_const<with_default_tolerance<
  minimum_distance_line_line_detail::minimum_distance_line_line_fn>>::value;
}

}  // namespace hm3::geometry
