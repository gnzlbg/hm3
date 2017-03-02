#pragma once
/// \file
///
/// Value of the line parameter of a given point
#include <hm3/geometry/algorithm/line_intersection_parameter/line_line.hpp>
#include <hm3/geometry/algorithm/line_intersection_parameter/line_point.hpp>
#include <hm3/geometry/algorithm/line_intersection_parameter/segment_point.hpp>

namespace hm3::geometry {

namespace line_intersection_parameter_detail {

struct line_intersection_parameter_fn {
  template <typename T, typename U,
            CONCEPT_REQUIRES_(Point<T>{} and Vector<U>{})>
  static constexpr auto impl(T const& p, U const& v, T const& p2, U const& v2,
                             num_t abs_tol, num_t rel_tol) {
    return intersection_parameter_line_line(p, v, p2, v2, abs_tol, rel_tol);
  }

  template <typename T, typename U, CONCEPT_REQUIRES_(Line<T>{} and Point<U>{})>
  static constexpr auto impl(T const& l, U const& p, num_t abs_tol,
                             num_t rel_tol) {
    return intersection_parameter_line_point(l, p, abs_tol, rel_tol);
  }

  template <typename T, typename U, typename K,
            CONCEPT_REQUIRES_(Segment<T>{} and Point<U>{})>
  static constexpr auto impl(T const& l, U const& p, K&& k, num_t abs_tol,
                             num_t rel_tol) {
    return intersection_parameter_segment_point(l, p, std::forward<K>(k),
                                                abs_tol, rel_tol);
  }

  template <typename T, CONCEPT_REQUIRES_(Line<T>{})>
  static constexpr auto impl(T const& l, T const& p, num_t abs_tol,
                             num_t rel_tol) {
    return intersection_parameter_line_line(l, p, abs_tol, rel_tol);
  }

  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u), abs_tol,
                                             rel_tol));

  template <typename T, typename U, typename K>
  constexpr auto operator()(T&& t, U&& u, K&& k, num_t abs_tol,
                            num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             std::forward<U>(u),
                                             std::forward<K>(k), abs_tol,
                                             rel_tol));

  template <typename T, typename U, typename V, typename W>
  constexpr auto operator()(T&& t, U&& u, V&& v, W&& w, num_t abs_tol,
                            num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    impl(std::forward<T>(t), std::forward<U>(u), std::forward<V>(v),
         std::forward<W>(w), abs_tol, rel_tol));
};

}  // namespace line_intersection_parameter_detail

namespace {
static constexpr auto const& line_intersection_parameter
 = static_const<with_default_tolerance<
  line_intersection_parameter_detail::line_intersection_parameter_fn>>::value;
}

}  // namespace hm3::geometry
