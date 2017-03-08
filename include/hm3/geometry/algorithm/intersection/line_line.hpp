#pragma once
/// \file
///
/// Intersection of lines.
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/algorithm/line_intersection_parameter/line_line.hpp>
#include <hm3/geometry/algorithm/parallel/line.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/ext/variant.hpp>

namespace hm3::geometry {

namespace intersection_line_line_detail {

template <typename L, typename UL = uncvref_t<L>,
          CONCEPT_REQUIRES_(Line<UL, 1>{})>
constexpr bool intersect_at_point(L const&, L const&, num_t, num_t) noexcept {
  return false;
}

template <typename L, typename UL = uncvref_t<L>,
          CONCEPT_REQUIRES_(Line<UL, 2>{})>
constexpr bool intersect_at_point(L const& a, L const& b, num_t abs_tol,
                                  num_t rel_tol) noexcept {
  return !parallel_line(a, b, abs_tol, rel_tol);
}

template <typename L, typename UL = uncvref_t<L>,
          CONCEPT_REQUIRES_(Line<UL, 3>{})>
inline bool intersect_at_point(L const& a, L const& b, num_t abs_tol,
                               num_t rel_tol) noexcept {
  if (parallel_line(a, b, abs_tol, rel_tol)) { return false; }

  auto ps = intersection_parameter_line_line(a, b);
  if (!ps) { return false; }

  auto t = first(ps.value());
  auto u = second(ps.value());

  auto pia = a.x(t);
  auto pib = b.x(u);
  for (dim_t d = 0; d < 3; ++d) {
    if (!approx(pia(d), pib(d), abs_tol, rel_tol)) { return false; }
  }
  return true;
}

struct intersection_test_line_line_fn {
  /// Do the lines \p a and \p b intersect?
  template <typename L, typename UL = uncvref_t<L>,
            CONCEPT_REQUIRES_(Line<UL>{})>
  constexpr bool operator()(L const& a, L const& b, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    if (parallel_line(a, b, abs_tol, rel_tol)) {
      if (coincidental_lines(a, b, abs_tol, rel_tol)) { return true; }
      return false;
    }
    if (intersect_at_point(a, b, abs_tol, rel_tol)) { return true; }
    return false;
  }
};

struct intersection_line_line_fn {
  /// Intersection between the lines \p a and \p b.
  ///
  /// \returns Intersection. That is, either empty, point, or line.
  template <typename L, typename UL = uncvref_t<L>,
            typename PT = associated::point_t<UL>,
            CONCEPT_REQUIRES_(Line<UL>{})>
  inline variant<monostate, PT, UL> operator()(L const& a, L const& b,
                                               num_t abs_tol,
                                               num_t rel_tol) const noexcept {
    if (coincidental_lines(a, b, abs_tol, rel_tol)) { return a; }
    auto ps = intersection_parameter_line_line(a, b);
    if (!ps) { return monostate{}; }

    auto t = first(ps.value());
    auto s = second(ps.value());

    auto a_p = a.x(t);
    auto b_p = b.x(s);
    if (approx(distance.centroid(a_p, b_p), 0., abs_tol, rel_tol)) {
      return a_p;
    }
    return monostate{};
  }
};

}  // namespace intersection_line_line_detail

namespace {
static constexpr auto const& intersection_test_line_line
 = static_const<with_default_tolerance<
  intersection_line_line_detail::intersection_test_line_line_fn>>::value;

static constexpr auto const& intersection_line_line
 = static_const<with_default_tolerance<
  intersection_line_line_detail::intersection_line_line_fn>>::value;
}

}  // namespace hm3::geometry
