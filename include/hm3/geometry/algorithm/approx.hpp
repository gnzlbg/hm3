#pragma once
/// \file
///
/// Are geometric primitives approximately equal?
#include <hm3/geometry/algorithm/approx/aabb.hpp>
#include <hm3/geometry/algorithm/approx/box.hpp>
#include <hm3/geometry/algorithm/approx/line.hpp>
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/approx/polyline_polygon.hpp>
#include <hm3/geometry/algorithm/approx/segment.hpp>
#include <hm3/geometry/concept/ray.hpp>
#include <hm3/geometry/tolerance.hpp>

namespace hm3::geometry {

namespace approx_detail {

struct approx_fn {
  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::point<Ad>) {
    static_assert(Point<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return approx_point(std::forward<T>(t), std::forward<U>(u), abs_tol,
                        rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_geq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                    trait::point<Ad>) {
    static_assert(Point<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return approx_point.geq(std::forward<T>(t), std::forward<U>(u), abs_tol,
                            rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_leq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                    trait::point<Ad>) {
    static_assert(Point<uncvref_t<T>>{});
    static_assert(Point<uncvref_t<U>>{});
    return approx_point.leq(std::forward<T>(t), std::forward<U>(u), abs_tol,
                            rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::vector<Ad>) {
    static_assert(Vector<uncvref_t<T>>{});
    static_assert(Vector<uncvref_t<U>>{});
    return approx_vector(std::forward<T>(t), std::forward<U>(u), abs_tol,
                         rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_geq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                    trait::vector<Ad>) {
    static_assert(Vector<uncvref_t<T>>{});
    static_assert(Vector<uncvref_t<U>>{});
    return approx_vector.geq(std::forward<T>(t), std::forward<U>(u), abs_tol,
                             rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_leq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                    trait::vector<Ad>) {
    static_assert(Vector<uncvref_t<T>>{});
    static_assert(Vector<uncvref_t<U>>{});
    return approx_vector.leq(std::forward<T>(t), std::forward<U>(u), abs_tol,
                             rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::line<Ad>) {
    static_assert(Line<uncvref_t<T>>{});
    static_assert(Line<uncvref_t<U>>{});
    return approx_line(std::forward<T>(t), std::forward<U>(u), abs_tol,
                       rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::ray<Ad>) {
    static_assert(Ray<uncvref_t<T>>{});
    static_assert(Ray<uncvref_t<U>>{});
    return approx_point(t.origin(), u.origin(), abs_tol, rel_tol)
           and approx_line(t.line(), u.line(), abs_tol, rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::segment<Ad>) {
    static_assert(Segment<uncvref_t<T>>{});
    static_assert(Segment<uncvref_t<U>>{});
    return approx_segment(std::forward<T>(t), std::forward<U>(u), abs_tol,
                          rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::polyline<Ad>) {
    static_assert(Polyline<uncvref_t<T>>{});
    static_assert(Polyline<uncvref_t<U>>{});
    return approx_polyline(std::forward<T>(t), std::forward<U>(u), abs_tol,
                           rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::polygon<Ad>) {
    static_assert(Polygon<uncvref_t<T>>{});
    static_assert(Polygon<uncvref_t<U>>{});
    return approx_polygon(std::forward<T>(t), std::forward<U>(u), abs_tol,
                          rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::aabb<Ad>) {
    return approx_aabb(std::forward<T>(t), std::forward<U>(u), abs_tol,
                       rel_tol);
  }

  template <typename T, typename U, dim_t Ad = ad_v<T>>
  static constexpr auto default_eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol,
                                   trait::box<Ad>) {
    return approx_box(std::forward<T>(t), std::forward<U>(u), abs_tol, rel_tol);
  }

  template <typename T, typename U>
  static constexpr auto eq_impl(T&& t, U&& u, num_t abs_tol, num_t rel_tol) {
    if
      constexpr(std::is_floating_point<uncvref_t<T>>{}
                and std::is_floating_point<uncvref_t<U>>{}) {
        return approx_number(std::forward<T>(t), std::forward<U>(u), abs_tol,
                             rel_tol);
      }
    else {
      static_assert(UCVSame<T, U>, "T and U must be equal");
      return default_eq(std::forward<T>(t), std::forward<U>(u), abs_tol,
                        rel_tol, associated::v_<T>);
    }
  }

  template <typename T, typename U>
  static constexpr auto leq_impl(T&& t, U&& u, num_t abs_tol, num_t rel_tol) {
    if
      constexpr(std::is_floating_point<uncvref_t<T>>{}
                and std::is_floating_point<uncvref_t<U>>{}) {
        return t <= u
               or approx_number(std::forward<T>(t), std::forward<U>(u), abs_tol,
                                rel_tol);
      }
    else {
      static_assert(UCVSame<T, U>, "T and U must be equal");
      return default_leq(std::forward<T>(t), std::forward<U>(u), abs_tol,
                         rel_tol, associated::v_<T>);
    }
  }

  template <typename T, typename U>
  static constexpr auto geq_impl(T&& t, U&& u, num_t abs_tol, num_t rel_tol) {
    if
      constexpr(std::is_floating_point<uncvref_t<T>>{}
                and std::is_floating_point<uncvref_t<U>>{}) {
        return t >= u
               or approx_number(std::forward<T>(t), std::forward<U>(u), abs_tol,
                                rel_tol);
      }
    else {
      static_assert(UCVSame<T, U>, "T and U must be equal");
      return default_geq(std::forward<T>(t), std::forward<U>(u), abs_tol,
                         rel_tol, associated::v_<T>);
    }
  }

  template <typename T, typename U>
  static constexpr auto eq(T&& t, U&& u, num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(eq_impl(std::forward<T>(t),
                                                std::forward<U>(u), abs_tol,
                                                rel_tol));
  template <typename T, typename U>
  constexpr auto operator()(T&& t, U&& u, num_t abs_tol, num_t rel_tol) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(eq(std::forward<T>(t),
                                           std::forward<U>(u), abs_tol,
                                           rel_tol));

  template <typename T, typename U>
  static constexpr auto leq(T&& t, U&& u, num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(leq_impl(std::forward<T>(t),
                                                 std::forward<U>(u), abs_tol,
                                                 rel_tol));

  template <typename T, typename U>
  static constexpr auto geq(T&& t, U&& u, num_t abs_tol, num_t rel_tol)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(geq_impl(std::forward<T>(t),
                                                 std::forward<U>(u), abs_tol,
                                                 rel_tol));

  template <typename T, typename U>
  static constexpr auto leq(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(leq(std::forward<T>(t),
                                            std::forward<U>(u),
                                            default_tolerance.absolute(),
                                            default_tolerance.relative()));

  template <typename T, typename U>
  static constexpr auto geq(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(geq(std::forward<T>(t),
                                            std::forward<U>(u),
                                            default_tolerance.absolute(),
                                            default_tolerance.relative()));
};

}  // namespace approx_detail

namespace {
constexpr auto const& approx
 = static_const<with_default_tolerance<approx_detail::approx_fn>>::value;
}

}  // namespace hm3::geometry
