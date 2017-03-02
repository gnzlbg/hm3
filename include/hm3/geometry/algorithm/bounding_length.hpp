#pragma once
/// \file
///
/// Bounding length.
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>
#include <hm3/geometry/algorithm/bounding_length/box.hpp>
#include <hm3/geometry/algorithm/bounding_length/polyline_polygon.hpp>
#include <hm3/geometry/algorithm/bounding_length/segment.hpp>

namespace hm3::geometry {

namespace bounding_length_detail {

struct bounding_length_fn {
  /// \name Bounding length: Segment implementations
  ///@{

  /// Bounding length \p d of segment \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, dim_t d, trait::segment<Ad>) noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    return bounding_length_segment(std::forward<P>(p), d);
  }

  /// Largest bounding length of segment \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto max_impl(P&& p, trait::segment<Ad>) noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    return bounding_length_segment.max(std::forward<P>(p));
  }

  /// All bounding lengths of segment \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto all_impl(P&& p, trait::segment<Ad>) noexcept {
    static_assert(Segment<uncvref_t<P>>{});
    return bounding_length_segment.all(std::forward<P>(p));
  }

  ///@}  // Segment implementation

  /// \name Polygon implementation
  ///@{

  /// Bounding length \p d of polygon \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, dim_t d, trait::polygon<Ad>) noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    return bounding_length_polyline(std::forward<P>(p), d);
  }

  /// All bounding lengths of polygon \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto all_impl(P&& p, trait::polygon<Ad>) noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    return bounding_length_polyline.all(std::forward<P>(p));
  }

  /// Max bounding lengths of polygon \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto max_impl(P&& p, trait::polygon<Ad>) noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    return bounding_length_polyline.max(std::forward<P>(p));
  }

  ///@}  // Polygon implementation

  /// \name Polyline implementation
  ///@{

  /// Bounding length \p d of polyline \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, dim_t d, trait::polyline<Ad>) noexcept {
    static_assert(Polyline<uncvref_t<P>>{});
    return bounding_length_polyline(std::forward<P>(p), d);
  }

  /// All bounding lengths of polyline \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto all_impl(P&& p, trait::polyline<Ad>) noexcept {
    static_assert(Polyline<uncvref_t<P>>{});
    return bounding_length_polyline.all(std::forward<P>(p));
  }

  /// Max bounding lengths of polyline \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto max_impl(P&& p, trait::polyline<Ad>) noexcept {
    static_assert(Polyline<uncvref_t<P>>{});
    return bounding_length_polyline.max(std::forward<P>(p));
  }

  ///@}  // Polyline implementation

  /// \name AABB implementation
  ///@{

  /// Bounding length \p d of aabb \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, dim_t d, trait::aabb<Ad>) noexcept {
    return bounding_length_aabb(std::forward<P>(p), d);
  }

  /// All bounding lengths of aabb \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto all_impl(P&& p, trait::aabb<Ad>) noexcept {
    return bounding_length_aabb.all(std::forward<P>(p));
  }

  /// Max bounding lengths of aabb \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto max_impl(P&& p, trait::aabb<Ad>) noexcept {
    return bounding_length_aabb.max(std::forward<P>(p));
  }

  ///@}  // AABB implementation

  /// \name Box implementation
  ///@{

  /// Bounding length \p d of box \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, dim_t d, trait::box<Ad>) noexcept {
    return bounding_length_box(std::forward<P>(p), d);
  }

  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& p, trait::box<Ad>) noexcept {
    return bounding_length_box(std::forward<P>(p), 0);
  }

  /// All bounding lengths of box \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto all_impl(P&& p, trait::box<Ad>) noexcept {
    return bounding_length_box.all(std::forward<P>(p));
  }

  /// Max bounding lengths of box \p p
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto max_impl(P&& p, trait::box<Ad>) noexcept {
    return bounding_length_box.max(std::forward<P>(p));
  }

  //@}  // Box implementation

  /// Max bounding length of \p t.
  template <typename T>
  static constexpr auto max(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(max_impl(std::forward<T>(t),
                                                 associated::v_<T>));

  /// All bounding lengths of \p t.
  template <typename T>
  static constexpr auto all(T&& t)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(all_impl(std::forward<T>(t),
                                                 associated::v_<T>));

  /// Bounding length across dimension \p d of \p t.
  template <typename T>
  constexpr auto operator()(T&& t, dim_t d) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t), d,
                                             associated::v_<T>));
  /// Bounding length of \p t (when t has a single bounding length).
  template <typename T>
  constexpr auto operator()(T&& t) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(impl(std::forward<T>(t),
                                             associated::v_<T>));
};

}  // namespace bounding_length_detail

namespace {
static constexpr auto const& bounding_length
 = static_const<bounding_length_detail::bounding_length_fn>::value;
}

}  // namespace hm3::geometry
