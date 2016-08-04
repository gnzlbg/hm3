#pragma once
/// \file
///
/// Does one primitive intersects another?
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/polygon/small_polygon.hpp>
//#include <hm3/geometry/primitive/polyhedra/small_polyhedra.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/geometry/sd/concept.hpp>
#include <hm3/utility/inline_vector.hpp>
#include <hm3/utility/range.hpp>
#include <hm3/utility/small_vector.hpp>

namespace hm3 {
namespace geometry {

// clang-format off

template <dim_t Nd> //
struct volume_intersection {
  using domain_t = meta::if_c<Nd == 1, segment<Nd>,
                   meta::if_c<Nd == 2, small_polygon<Nd, 6>,
                // meta::if_c<Nd == 3, small_polyhedra<Nd, 12>,
                              void
                //           >
                   >>;

  domain_t volume;
  inline_vector<sidx_t, 2> idx;  // original volume index
  small_vector<sidx_t, 2> face_ids; // intersection face indices
  bool inside;
};

template <dim_t Nd> //
struct face_intersection {
  using domain_t = meta::if_c<Nd == 1, point<Nd>,
                   meta::if_c<Nd == 2, segment<Nd>,
                   meta::if_c<Nd == 3, small_polygon<Nd, 9>,
                              void>>> ;

  domain_t face;
  optional_index<sidx_t, struct original_face> idx;  // original volume/face idx
  inline_vector<sidx_t, 2> volume_ids;  // intersection volume indices
};

// clang-format on

template <dim_t Nd>  //
struct intersection_result {
  using volume_t  = volume_intersection<Nd>;
  using face_t    = face_intersection<Nd>;
  using volumes_t = small_vector<volume_t, 3>;
  using faces_t   = small_vector<face_t, 3>;

  volumes_t volumes;
  faces_t faces;

  bool empty() const noexcept { return volumes.empty() and faces.empty(); }
};

namespace detail {

struct intersection_fn {
  template <typename T, typename U,
            CONCEPT_REQUIRES_(Primitive<T>{} and Primitive<U>{})>
  static constexpr auto intersection_test_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test(std::forward<U>(u),
                                                          std::forward<T>(t)));

  template <typename T, typename U,
            CONCEPT_REQUIRES_(Primitive<T>{} and Primitive<U>{})>
  static constexpr auto intersection_test_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_test(std::forward<T>(t),
                                                          std::forward<U>(u)));

  template <typename T, typename U,
            CONCEPT_REQUIRES_(Primitive<T>{} and Primitive<U>{})>
  static constexpr auto intersection_impl(T&& t, U&& u, long)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection(std::forward<U>(u),
                                                     std::forward<T>(t)));

  template <typename T, typename U,
            CONCEPT_REQUIRES_(Primitive<T>{} and Primitive<U>{})>
  static constexpr auto intersection_impl(T&& t, U&& u, int)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection(std::forward<T>(t),
                                                     std::forward<U>(u)));

  template <typename T, typename U,
            CONCEPT_REQUIRES_(Primitive<T>{} and Primitive<U>{})>
  static constexpr auto test(T&& t, U&& u) RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
   intersection_test_impl(std::forward<T>(t), std::forward<U>(u), 0));

  template <typename T, typename U,
            CONCEPT_REQUIRES_(Primitive<T>{} and Primitive<U>{})>
  constexpr auto operator()(T&& t, U&& u) const
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(intersection_impl(std::forward<T>(t),
                                                          std::forward<U>(u)));

  /// Intersection of a discrete primitive with a signed distance function.
  template <typename T, typename U, dim_t Nd = dimension_v<T>,
            CONCEPT_REQUIRES_(Primitive<T>{} and SignedDistance<U, Nd>{})>
  static constexpr auto signed_distance(T&& t, U&& u)
   RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT(
    signed_distance_intersection(std::forward<T>(t), std::forward<U>(u)));
};

}  // namespace detail

namespace {
static constexpr auto const& intersection
 = static_const<detail::intersection_fn>::value;
}  // namespace

}  // namespace geometry
}  // namespace hm3
