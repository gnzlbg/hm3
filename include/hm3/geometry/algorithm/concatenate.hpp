#pragma once
/// \file
///
/// Concatenates two primitives that only overlap at their boundary.
///
/// If the primitives are oriented, concatenation preserves orientation.
#include <hm3/geometry/algorithm/concatenate/point_point.hpp>
#include <hm3/geometry/algorithm/concatenate/polyline_point.hpp>
#include <hm3/geometry/algorithm/concatenate/polyline_polyline.hpp>
#include <hm3/geometry/algorithm/concatenate/segment_point.hpp>
#include <hm3/geometry/algorithm/concatenate/segment_polyline.hpp>
#include <hm3/geometry/algorithm/concatenate/segment_segment.hpp>
#include <hm3/geometry/algorithm/direction.hpp>

namespace hm3::geometry {

namespace concatenate_detail {

// TODO: use static_assert instead of SFINAE for better error messages
// TODO: reconsider the design of the concatenation algorithm,
//       associated::concatenated_t, etc.

struct concatenate_fn {
  template <typename T, typename U,
            CONCEPT_REQUIRES_(MutablePolyline<T>{} and MutablePolyline<U>{})>
  static constexpr auto concatenate_impl(T&& t, U&& u, num_t abs_tol,
                                         num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_polyline_polyline(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol));

  template <typename T, typename U,
            CONCEPT_REQUIRES_(Segment<T>{} and MutablePolyline<U>{})>
  static constexpr auto concatenate_impl(T&& t, U&& u, num_t abs_tol,
                                         num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_segment_polyline(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol));

  template <typename T, CONCEPT_REQUIRES_(Segment<T>{})>
  static constexpr auto concatenate_impl(T const& t, T const& u, num_t abs_tol,
                                         num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_segment_segment(t, u,
                                                                    abs_tol,
                                                                    rel_tol));

  template <typename S, typename P,
            CONCEPT_REQUIRES_(Segment<S>{} and Point<P>{})>
  static constexpr auto concatenate_impl(S const& s, P const& p, num_t abs_tol,
                                         num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_segment_point(s, p, abs_tol,
                                                                  rel_tol));

  template <typename PL, typename P,
            CONCEPT_REQUIRES_(Polyline<PL>{} and Point<P>{})>
  static constexpr auto concatenate_impl(PL const& pl, P const& p,
                                         num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_polyline_point(pl, p,
                                                                   abs_tol,
                                                                   rel_tol));

  template <typename T, CONCEPT_REQUIRES_(Point<T>{})>
  static constexpr auto concatenate_impl(T const& t, T const& u, num_t abs_tol,
                                         num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_point_point(t, u, abs_tol,
                                                                rel_tol));

  template <typename T, typename U>
  static constexpr auto concatenate_impl_(T&& t, U&& u, num_t abs_tol,
                                          num_t rel_tol, preferred)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_impl(std::forward<T>(t),
                                                         std::forward<U>(u),
                                                         abs_tol, rel_tol));

  template <typename T, typename U>
  static constexpr auto concatenate_impl_(T&& t, U&& u, num_t abs_tol,
                                          num_t rel_tol, fallback)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_impl(std::forward<U>(u),
                                                         std::forward<T>(t),
                                                         abs_tol, rel_tol));

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(concatenate_impl_(
    std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol, dispatch));

  /// Direction independent concatenation (does not preserve orientation).
  template <typename T, typename U>
  static constexpr auto direction_independent(T&& t, U&& u, num_t abs_tol,
                                              num_t rel_tol) {
    auto r = concatenate_fn{}(t, u, abs_tol, rel_tol);
    if (r) { return r; }
    return concatenate_fn{}(t, direction.invert(u), abs_tol, rel_tol);
  }

  template <typename T, typename U>
  static constexpr auto direction_independent(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(direction_independent(
    std::forward<T>(t), std::forward<U>(u), default_tolerance.absolute(),
    default_tolerance.relative()));

  template <typename Rng, typename Vec>
  static constexpr void range(Rng&& rng, Vec& result) noexcept {
    using T   = ranges::range_value_t<Rng>;
    using c_t = associated::concatenated_t<T>;

    static_assert(Same<ranges::range_value_t<Vec>, c_t>{});

    HM3_ASSERT(result.empty(), "cannot concatenate into a non-empty vector!");
    if (ranges::size(rng) == 0) { return; }

    if constexpr (element_dimension_v<T> == 0) {  // points
      HM3_ASSERT(ranges::size(rng) != 1,
                 "cannot concatenate range of 1 points into a polyline");
      using e_t = associated::edge_t<c_t>;
      // Construct the first polyline from the first two points:
      result.push_back(c_t{view::single(e_t{index(rng, 0), index(rng, 1)})});

      auto rest = rng | view::drop(2);
      for (auto&& p : rest) {
        auto r = concatenate_polyline_point(result[0], p);
        if (r) { result[0] = r.value(); }
      }
      return;
    }

    if constexpr (element_dimension_v<T> == 1) {  // lines
      // First we append all the primitives to the result, trying to merge as
      // much of them as possible:
      for (auto&& p : rng) {
        // create a target primitive containing one of the input primitives.
        auto t = c_t(edges(p));

        // If its the first one, we append it and are done:
        if (result.empty()) {
          result.push_back(t);
          continue;
        }

        // Otherwise, we try to concatenate it in a direction independent way
        // with any of the primitives in the result:
        bool merged = false;
        for (auto&& r : result) {
          auto c = direction_independent(r, t);
          if (not c) { continue; }
          r      = c.value();
          merged = true;
          break;
        }

        // We can only land here if it wasn't possible to concatenate it with
        // any primitive in the result, so we append it:
        if (not merged) { result.push_back(t); }
      }

      // Now we try to concatenate all the primitives in the result with each
      // other until no more concatenations are possible:
      bool done = false;
      while (not done) {
        // TODO: this loop doesn't scale well to larger numbers of primitives.
        // A more efficient way would be to build up a flat_set of edges
        done = true;
        for (suint_t i = 0; i < result.size(); ++i) {
          for (suint_t j = i + 1; j < result.size(); ++j) {
            if (i == j) { continue; }
            auto c = direction_independent(ranges::index(result, i),
                                           ranges::index(result, j));
            if (not c) { continue; }
            ranges::index(result, i) = c.value();
            result.erase(ranges::begin(result) + j);
            done = false;
          }
        }
      }
      return;
    }

    if constexpr (element_dimension_v<T> == 2) {  // surfaces
      static_assert(always_false<T>{}, "concatenate surfaces not implemented");
    }
  }

  template <typename Rng>
  static constexpr auto range(Rng&& rng) noexcept {
    using T   = ranges::range_value_t<Rng>;
    using c_t = associated::concatenated_t<T>;

    small_vector<c_t, 4> result;
    range(std::forward<Rng>(rng), result);
    return result;
  }
};

}  // namespace concatenate_detail

namespace {
constexpr auto const& concatenate = static_const<
 with_default_tolerance<concatenate_detail::concatenate_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
