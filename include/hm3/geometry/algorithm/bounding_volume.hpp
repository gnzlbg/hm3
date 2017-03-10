#pragma once
/// \file
///
/// Bounding volume.
#include <hm3/geometry/algorithm/bounding_volume/aabb.hpp>
#include <hm3/geometry/algorithm/bounding_volume/box.hpp>
#include <hm3/geometry/algorithm/bounding_volume/point.hpp>
#include <hm3/geometry/algorithm/bounding_volume/polyline.hpp>
#include <hm3/geometry/algorithm/bounding_volume/segment.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/fwd.hpp>

namespace hm3::geometry {

namespace bounding_volume_detail {

struct bounding_volume_fn {
  /// \name Point implementation
  ///@{

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto aabb_impl(T&& t, trait::point<Ad>) {
    static_assert(Point<uncvref_t<T>>{});
    return aabb_of_point(std::forward<T>(t));
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto box_impl(T&& t, trait::point<Ad>) {
    static_assert(Point<uncvref_t<T>>{});
    return box_of_point(std::forward<T>(t));
  }

  ///@}  // Point implementation

  /// \name Segment implementation
  ///@{

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto aabb_impl(T&& t, trait::segment<Ad>) {
    static_assert(Segment<uncvref_t<T>>{});
    return aabb_of_segment(std::forward<T>(t));
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto box_impl(T&& t, trait::segment<Ad>) {
    static_assert(Segment<uncvref_t<T>>{});
    return box_of_segment(std::forward<T>(t));
  }

  ///@}  // Segment  implementation

  /// \name Polyline implementation
  ///@{

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto aabb_impl(T&& t, trait::polyline<Ad>) {
    static_assert(Polyline<uncvref_t<T>>{});
    return aabb_of_polyline(std::forward<T>(t));
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto box_impl(T&& t, trait::polyline<Ad>) {
    static_assert(Polyline<uncvref_t<T>>{});
    return box_of_polyline(std::forward<T>(t));
  }

  ///@}  // Polyline implementation

  /// \name Polygon implementation
  ///@{

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto aabb_impl(T&& t, trait::polygon<Ad>) {
    static_assert(Polygon<uncvref_t<T>>{});
    return aabb_of_polygon(std::forward<T>(t));
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto box_impl(T&& t, trait::polygon<Ad>) {
    static_assert(Polygon<uncvref_t<T>>{});
    return box_of_polygon(std::forward<T>(t));
  }

  ///@}  // Polygon implementation

  /// \name Polyhedron implementation
  ///@{

  template <typename T>
  static constexpr auto aabb_impl(T&& t, trait::polyhedron) {
    static_assert(Polyhedron<uncvref_t<T>>{});
    return aabb_of_polyhedron(std::forward<T>(t));
  }

  template <typename T>
  static constexpr auto box_impl(T&& t, trait::polyhedron) {
    static_assert(Polyhedron<uncvref_t<T>>{});
    return box_of_polyhedron(std::forward<T>(t));
  }

  ///@}  // Polyhedron implementation

  /// \name AABB implementation
  ///@{

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto aabb_impl(T&& t, trait::aabb<Ad>) {
    return aabb_of_aabb(std::forward<T>(t));
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto box_impl(T&& t, trait::aabb<Ad>) {
    return box_of_aabb(std::forward<T>(t));
  }

  ///@}  // AABB implementation

  /// \name Box implementation
  ///@{

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto aabb_impl(T&& t, trait::box<Ad>) {
    return aabb_of_box(std::forward<T>(t));
  }

  template <typename T, dim_t Ad = ad_v<T>>
  static constexpr auto box_impl(T&& t, trait::box<Ad>) {
    return box_of_box(std::forward<T>(t));
  }

  ///@}  // Box implementation

  /// \name AABB dispatch (customization point)
  ///@{

  /// Axis-Aligned Bounding Box
  template <typename T>
  static constexpr auto aabb_dispatch(T&& t, fallback)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(aabb_impl(std::forward<T>(t),
                                                  associated::v_<T>));

  /// Axis-Aligned Bounding Box
  template <typename T>
  static constexpr auto aabb_dispatch(T&& t, preferred)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    axis_aligned_bounding_box(std::forward<T>(t)));

  ///@}

  /// Axis-Aligned Bounding Box
  template <typename T, CONCEPT_REQUIRES_(AmbientDimension<T>{})>
  static constexpr auto aabb(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(aabb_dispatch(std::forward<T>(t),
                                                      dispatch));

  /// Square Bounding Box
  template <typename T>
  static constexpr auto box(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(box_impl(std::forward<T>(t),
                                                 associated::v_<T>));

  /// Axis-Aligned Bounding Box of a range of primitives.
  template <typename Rng,
            CONCEPT_REQUIRES_(!AmbientDimension<Rng>{} and Range<Rng>{})>
  static constexpr auto aabb(Rng&& rng) {
    using aabb_t = associated::aabb_t<ranges::range_value_t<Rng>>;
    using p_t    = associated::point_t<aabb_t>;
    auto r_min   = p_t::constant(math::highest<num_t>);
    auto r_max   = p_t::constant(math::lowest<num_t>);
    for (auto&& o : rng) {
      // compute aabb of each element of the range:
      auto bb = bounding_volume_fn::aabb(o);
      // update aabb for all elements:
      for (dim_t d = 0; d < ad_v<p_t>; ++d) {
        r_min(d) = std::min(r_min(d), x_min(bb)(d));
        r_max(d) = std::max(r_max(d), x_max(bb)(d));
      }
    }
    return aabb_t(r_min, r_max);
  }
};

}  // namespace bounding_volume_detail

namespace {
constexpr auto const& bounding_volume
 = static_const<bounding_volume_detail::bounding_volume_fn>::value;
}

}  // namespace hm3::geometry
